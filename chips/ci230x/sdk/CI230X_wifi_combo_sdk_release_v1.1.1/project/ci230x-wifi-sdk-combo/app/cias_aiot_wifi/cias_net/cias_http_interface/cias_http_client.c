/**
 * @file    cias_http_client.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#include <stdarg.h>
#include "cias_http_client.h"
#include <string.h>
#include <stdbool.h>
#include "cias_freertos_common.h"
#include "cias_freertos_debug.h"

#define HC_CHECK_ERR(p_client, ret, log_str, http_res)         \
    do                                                         \
    {                                                          \
        if (ret)                                               \
        {                                                      \
            p_client->ops.close(p_client->ops.n_handle);       \
            p_client->ops.n_handle = -1;                       \
            CIAS_PRINT_WARN(log_str "socket error (%d)", ret); \
            return http_res;                                   \
        }                                                      \
    } while (0)

static const int HC_CR_LF_LEN = 2; //strlen("\r\n")
static const int HC_SCHEME_LEN_MAX = 8;
static const int HC_HOST_LEN_MAX = 255;
static const int HC_REDIRECTION_MAX = 30;
static const int HC_HTTP_TIME_OUT = (2 * 1000); //TIME OUT IN MILI-SECOND
static const int HC_HTTP_TIME_OUT_TRIES = 30;
static const int HC_REQUEST_CMD_STR_LEN_MAX = 6;
static const int HC_CHUNK_SIZE = 1025;
static const int HC_RSP_CHUNK_SIZE = 1024;
static const int HC_HEADER_CHUNK_SIZE = 512;
static const int HC_MAX_RETRY_RESUME_COUNT = 10;
static const int HC_MAX_RANGE_LEN = 10;

const char *http_request_cmd[] =
    {
        "GET",    //HTTP_GET
        "POST",   //HTTP_POST
        "PUT",    //HTTP_PUT
        "DELETE", //HTTP_DELETE
        "HEAD",   //HTTP_HEAD
};

static volatile http_client_statistic_t s_http_client_statistic;

int cias_http_client_init_socket_ops(http_client_c *p_client, socket_ops *p_ops, void *socket_args)
{
    if (!p_client || !p_ops)
    {
        CIAS_PRINT_WARN("Args err!\n");
        return HTTP_FAILED;
    }
    memcpy(&p_client->ops, p_ops, sizeof(socket_ops));
    return HTTP_OK;
}

int cias_http_client_init(http_client_c *p_client)
{
    if (!p_client)
    {
        CIAS_PRINT_WARN("Args err: p_client is null!\n");
        return HTTP_FAILED;
    }
    memset(p_client, 0, sizeof(http_client_c));
    return HTTP_OK;
}

void cias_http_client_destroy(http_client_c *p_client)
{
    if (!p_client)
    {
        CIAS_PRINT_WARN("Args err: p_client is null!\n");
        return;
    }

    if (p_client->p_location != NULL)
    {
        cias_free(p_client->p_location);
    }
    p_client->p_location = NULL;
    if (p_client->p_chunk_buf != NULL)
    {
        cias_free(p_client->p_chunk_buf);
    }
    p_client->p_chunk_buf = NULL;
    p_client->ops.destroy(p_client->ops.n_handle);
    p_client->ops.n_handle = -1;

    if (p_client != NULL)
    {
        cias_free(p_client);
    }
    p_client = NULL;

    return;
}

void cias_http_client_set_cust_headers(http_client_c *p_client, const char **headers, size_t pairs)
{
    if (!p_client)
    {
        CIAS_PRINT_WARN("Args err: p_client is null!\n");
        return;
    }
    p_client->pps_custom_headers = headers;
    p_client->sz_headers_count = pairs;
}

void cias_http_client_reg_data_hdlr_cb(http_client_c *p_client, data_out_handler_cb data_hdlr_cb, void *p_usr_ctx)
{
    if (!p_client)
    {
        CIAS_PRINT_WARN("Args err: p_client is null!\n");
        return;
    }
    p_client->data_hdlr_cb = data_hdlr_cb;
    p_client->p_data_hdlr_ctx = p_usr_ctx;
}

void cias_http_client_reg_cb_to_get_url(http_client_c *p_client, get_url_cb_t cb)
{
    if (!p_client)
    {
        CIAS_PRINT_WARN("Args err: p_client is null!\n");
        return;
    }
    p_client->get_url_cb = cb;
}

void cias_http_client_reg_stop_notify_cb(http_client_c *p_client, check_stop_notify_cb_t chk_stp_cb)
{
    if (!p_client)
    {
        CIAS_PRINT_WARN("Args err: p_client is null!\n");
        return;
    }
    p_client->check_stop_notify_cb = chk_stp_cb;
}

int cias_http_client_get_resp_code(http_client_c *p_client)
{
    if (!p_client)
    {
        CIAS_PRINT_WARN("Args err: p_client is null!\n");
        return -1;
    }
    return p_client->n_http_response_code;
}

static e_http_result http_client_parse_url(const char *url, char *scheme, size_t scheme_len,
                                           char *host, unsigned short *port,
                                           char *path, size_t path_len, bool is_relative_url)
{
    size_t r_host_len = 0;
    size_t r_path_len = 0;
    char *p_path = NULL;
    char *p_fragment = NULL;
    char *p_host = NULL;
    char *p_parmas = NULL;
    char *p_scheme = (char *)url;

    if (is_relative_url)
    {
        p_path = (char *)url;
    }
    else
    {
        p_parmas = (char *)strstr(url, "?");
        p_host = (char *)strstr(url, "://");
        if (p_host == NULL)
        {
            CIAS_PRINT_WARN("Could not find host, url=%s\n", url);
            return HTTP_PARSE; //URL is invalid
        }

        //to find http scheme
        if ((int)scheme_len < p_host - p_scheme + 1)
        { //including NULL-terminating char
            CIAS_PRINT_WARN("Scheme str is too small (%d >= %d)\n", scheme_len, p_host - p_scheme + 1);
            return HTTP_PARSE;
        }
        memmove(scheme, p_scheme, p_host - p_scheme);
        scheme[p_host - p_scheme] = '\0';

        //to find port
        p_host += strlen("://");
        char *p_port = strchr(p_host, ':');

        if ((p_parmas != NULL) && (p_port != NULL))
        {
            if (p_port > p_parmas)
            {
                CIAS_PRINT_INFO("http_client_parse_url p_port = %s\n", p_port);
                *port = 0;
            }
            else
            {
                r_host_len = p_port - p_host;
                p_port++;
                if (sscanf(p_port, "%hu", port) != 1)
                {
                    CIAS_PRINT_WARN("Could not find port\n");
                    return HTTP_PARSE;
                }
            }
        }
        else
        {
            if (p_port != NULL)
            {
                r_host_len = p_port - p_host;
                p_port++;
                if (sscanf(p_port, "%hu", port) != 1)
                {
                    CIAS_PRINT_WARN("Could not find port\n");
                    return HTTP_PARSE;
                }
            }
            else
            {
                *port = 0;
            }
        }

        //to find host
        p_path = strchr(p_host, '/');
        if (!p_path)
        {
            CIAS_PRINT_WARN("Could not find path\n");
            return HTTP_PARSE;
        }

        if (r_host_len == 0)
        {
            r_host_len = p_path - p_host;
        }

        if (r_host_len > HC_HOST_LEN_MAX)
        {
            CIAS_PRINT_WARN("Host is too long(%d > %d)!\n", r_host_len, HC_HOST_LEN_MAX);
            return HTTP_PARSE;
        }

        memcpy(host, p_host, r_host_len);
        host[r_host_len] = '\0';
    }

    //to find path
    p_fragment = strchr(p_host, '#');
    if (p_fragment != NULL)
    {
        r_path_len = p_fragment - p_path;
    }
    else
    {
        r_path_len = strlen(p_path);
    }

    if (path_len < r_path_len + 1)
    { //including NULL-terminating char
        CIAS_PRINT_WARN("Path str is too small (%d >= %d)\n", path_len, r_path_len + 1);
        return HTTP_PARSE;
    }
    memmove(path, p_path, r_path_len);
    path[r_path_len] = '\0';

    return HTTP_OK;
}

static e_http_result http_client_send(http_client_c *p_client, const char *buf, size_t len)
{
    size_t written_len = 0;
    int ret = 0;

    if (len == 0)
    {
        len = strlen(buf);
    }

    p_client->ops.set_blocking(p_client->ops.n_handle, true);
    ret = p_client->ops.send(p_client->ops.n_handle, buf, len);
    if (ret > 0)
    {
        written_len += ret;
    }
    else if (ret == 0)
    {
        CIAS_PRINT_WARN("Connection was closed by server\n");
        return HTTP_CLOSED;
    }
    else
    {
        CIAS_PRINT_WARN("Connection error (send returned %d)\n", ret);
        return HTTP_CONN;
    }

    return HTTP_OK;
}

static e_http_result http_client_recv(http_client_c *p_client, char *buf, size_t max_len, size_t *p_read_len, int to_try)
{
    int ret = 0;
    int n_try_times = 0;
    size_t start_timestamp = 0;
    size_t time_consume = 0;
    size_t sz_read_len = 0;
    e_http_result res = HTTP_OK;

    while (sz_read_len < max_len)
    {
        // CIAS_PRINT_DEBUG("Trying(%d ) times to read at most %4d bytes [Not blocking] %d,%d",
        //           n_try_times, max_len - sz_read_len, max_len, sz_read_len);
        p_client->ops.set_blocking(p_client->ops.n_handle, true);
        p_client->ops.set_timeout(p_client->ops.n_handle, HC_HTTP_TIME_OUT);
        ret = p_client->ops.recv(p_client->ops.n_handle, buf + sz_read_len, max_len - sz_read_len);

        if (ret > 0)
        {
            sz_read_len += ret;
            break;
        }
        else if (ret == 0)
        {
            break;
        }
        else
        {
            res = HTTP_CONN;
            break;
        }
    }
    buf[sz_read_len] = '\0';
    *p_read_len = sz_read_len;
    return res;
}

static e_http_result http_client_send_request(http_client_c *p_client, e_http_meth method, char *path, char *host, unsigned short port)
{
    int ret = 0;
    int req_fmt_len = 0;
    char *p_request_buf = NULL;

    req_fmt_len = strlen("%s %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-\r\n");
    req_fmt_len += HC_REQUEST_CMD_STR_LEN_MAX + strlen(path) + strlen(host) + HC_MAX_RANGE_LEN;

    p_request_buf = (char *)cias_malloc(req_fmt_len);
    if (!p_request_buf)
    {
        CIAS_PRINT_WARN("Mallco memory(size:%d) for request buffer Failed!\n", req_fmt_len);
        return HTTP_FAILED;
    }

    memset(p_request_buf, 0, req_fmt_len);
    snprintf(p_request_buf, req_fmt_len, "%s %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-\r\n",
             http_request_cmd[method], path, host, p_client->recv_size);

    ret = http_client_send(p_client, p_request_buf, 0);
    free(p_request_buf);
    if (ret != HTTP_OK)
    {
        p_client->ops.close(p_client->ops.n_handle);
        p_client->ops.n_handle = -1;
        CIAS_PRINT_WARN("Could not write request\n");
        return HTTP_CONN;
    }

    return HTTP_OK;
}

static e_http_result http_client_send_headers(http_client_c *p_client, char *buff, int buff_size)
{
    size_t nh = 0;
    int header_len = 0;
    e_http_result ret = HTTP_OK;

    for (nh = 0; nh < p_client->sz_headers_count * 2; nh += 2)
    {
        CIAS_PRINT_DEBUG("hdr[%2d] %s: %s\n", nh, p_client->pps_custom_headers[nh],
                         p_client->pps_custom_headers[nh + 1]);

        header_len = strlen("%s: %s\r\n") + strlen(p_client->pps_custom_headers[nh]) + strlen(p_client->pps_custom_headers[nh + 1]);
        if (header_len >= buff_size)
        {
            CIAS_PRINT_DEBUG("Http header (%d) is too long!\n", header_len);
            p_client->ops.close(p_client->ops.n_handle);
            p_client->ops.n_handle = -1;
            return HTTP_FAILED;
        }

        snprintf(buff, buff_size, "%s: %s\r\n", p_client->pps_custom_headers[nh],
                 p_client->pps_custom_headers[nh + 1]);
        ret = http_client_send(p_client, buff, 0);
        HC_CHECK_ERR(p_client, ret, "Could not write headers", HTTP_CONN);
        CIAS_PRINT_DEBUG("send() returned %d\n", ret);
    }

    ret = http_client_send(p_client, "\r\n", 0);
    HC_CHECK_ERR(p_client, ret, "Close http headers failed!", ret);

    return HTTP_OK;
}

static void http_client_set_content_len(http_client_c *p_client, int content_len)
{
    p_client->n_http_content_len = content_len;
}

static void http_client_set_content_type(http_client_c *p_client, char *p_content_type)
{
    if (p_content_type)
    {
        snprintf(p_client->p_http_content_type, HC_CONTENT_TYPE_LEN_MAX, "%s", p_content_type);
        if (strlen(p_content_type) >= HC_CONTENT_TYPE_LEN_MAX)
        {
            p_client->p_http_content_type[HC_CONTENT_TYPE_LEN_MAX - 1] = '\0';
        }
    }
}

static e_http_result http_client_recv_rsp_and_parse(http_client_c *p_client, char *buf, size_t max_len, size_t *p_read_len)
{
    int ret = 0;
    size_t cr_lf_pos = 0;
    size_t rev_len = 0;
    char *p_cr_lf = NULL;

    ret = http_client_recv(p_client, buf, max_len, &rev_len, 1);
    HC_CHECK_ERR(p_client, ret, "Receiving  http response error!", HTTP_CONN);

    buf[rev_len] = '\0';
    CIAS_PRINT_DEBUG("Received: \r\n(%s\r\n)", buf);

    p_cr_lf = strstr(buf, "\r\n");
    if (!p_cr_lf)
    {
        HC_CHECK_ERR(p_client, HTTP_PRTCL, "Response Protocol error", HTTP_PRTCL);
    }
    cr_lf_pos = p_cr_lf - buf;
    buf[cr_lf_pos] = '\0';

    //to parse HTTP response
    if (sscanf(buf, "HTTP/%*d.%*d %d %*[^\r\n]", &p_client->n_http_response_code) != 1)
    {
        //cannot match string, error
        CIAS_PRINT_WARN("Not a correct HTTP answer : {%s}\n", buf);
        HC_CHECK_ERR(p_client, HTTP_PRTCL, "Response Protocol error", HTTP_PRTCL);
    }

    if ((p_client->n_http_response_code < 200) || (p_client->n_http_response_code >= 400))
    {
        //Did not return a 2xx code;
        //TODO fetch headers/(&data?) anyway and implement a mean of writing/reading headers
        CIAS_PRINT_DEBUG("Response code %d\n", p_client->n_http_response_code);
        HC_CHECK_ERR(p_client, HTTP_PRTCL, "Response Protocol error", HTTP_PRTCL);
    }

    rev_len -= (cr_lf_pos + HC_CR_LF_LEN);
    //Be sure to move NULL-terminating char as well
    memmove(buf, &buf[cr_lf_pos + HC_CR_LF_LEN], rev_len + 1);

    if (p_read_len)
    {
        *p_read_len = rev_len;
    }

    return HTTP_OK;
}

static e_http_result http_client_recv_headers_and_parse(http_client_c *p_client, char *buf, size_t max_len, size_t *p_read_len)
{
    int i = 0;
    char *key = NULL;
    char *value = NULL;
    char *p_cr_lf = NULL;
    size_t len = 0;
    size_t cr_lf_pos = 0;
    size_t rev_len = 0;
    size_t new_rev_len = 0;
    e_http_result res = HTTP_OK;
    int http_content_len = 0;

    HC_CHECK_ERR(p_client, !p_read_len, "arg 'p_read_len' is null!", HTTP_FAILED);

    rev_len = *p_read_len;
    p_client->n_is_chunked = false;

    while (true)
    {
        p_cr_lf = strstr(buf, "\r\n");

        if (p_cr_lf == NULL)
        {
            if (rev_len < max_len)
            {
                res = http_client_recv(p_client, buf + rev_len, max_len - rev_len, &new_rev_len, 0);
                rev_len += new_rev_len;
                buf[rev_len] = '\0';
                HC_CHECK_ERR(p_client, res, "Receiving  http headers error!", res);
                continue;
            }
            else
            {
                HC_CHECK_ERR(p_client, HTTP_PRTCL, "Headers Protocol error", HTTP_PRTCL);
            }
        }

        cr_lf_pos = p_cr_lf - buf;

        //end of headers to break
        if (cr_lf_pos == 0)
        {
            CIAS_PRINT_DEBUG("All Http Headers readed over!\n");
            rev_len -= HC_CR_LF_LEN;
            //Be sure to move NULL-terminating char as well
            memmove(buf, &buf[HC_CR_LF_LEN], rev_len + 1);
            break;
        }

        key = NULL;
        value = NULL;
        buf[cr_lf_pos] = '\0';

        // parse the header
        for (i = 0; i < cr_lf_pos; i++)
        {
            if (buf[i] == ':')
            {
                buf[i] = '\0';
                key = buf;
                value = &buf[i + 1];
                break;
            }
        }

        if ((key != NULL) && (value != NULL))
        {
            while ((*value != '\0') && (*value == ' '))
            {
                value++;
            }
            if (!strncmp(key, "Content-Length", max_len))
            {
                sscanf(value, "%d", &http_content_len);
                http_client_set_content_len(p_client, http_content_len + p_client->recv_size);
            }
            else if (!strncmp(key, "Transfer-Encoding", max_len))
            {
                if (!strncmp(value, "Chunked", max_len - (value - buf)) || !strncmp(value, "chunked", max_len - (value - buf)))
                {
                    p_client->n_is_chunked = true;
                    CIAS_PRINT_DEBUG("Http client data is chunked transfored\n");
                }
            }
            else if (!strncmp(key, "Content-Type", max_len))
            {
                http_client_set_content_type(p_client, value);
            }
            else if (!strncmp(key, "Location", max_len))
            {
                if (p_client->p_location)
                {
                    cias_free(p_client->p_location);
                }
                p_client->p_location = NULL;
                len = strlen(value);
                p_client->p_location = (char *)cias_malloc(len + 1);
                HC_CHECK_ERR(p_client, !p_client->p_location, "Malloc memory failed for p_location!", HTTP_FAILED);

                snprintf(p_client->p_location, len + 1, "%s", value);
                HC_CHECK_ERR(p_client, HTTP_REDIRECTTION, "Taking http redirection!", HTTP_REDIRECTTION);
            }
            else
            {
                // do nothing
            }

            rev_len -= (cr_lf_pos + HC_CR_LF_LEN);
            //Be sure to move NULL-terminating char as well
            memmove(buf, &buf[cr_lf_pos + HC_CR_LF_LEN], rev_len + 1);
        }
        else
        {
            HC_CHECK_ERR(p_client, HTTP_PRTCL, "Could not parse header", HTTP_PRTCL);
        }
    }
    *p_read_len = rev_len;
    return HTTP_OK;
}

static e_http_result http_client_recv_chunked_size(http_client_c *p_client, char *buf, size_t max_len, size_t *p_read_len)
{
    int n = 0;
    int found_cr_lf = false;
    size_t rev_len = 0;
    size_t new_rev_len = 0;
    size_t cr_lf_pos = 0;
    e_http_result res = HTTP_OK;

    HC_CHECK_ERR(p_client, !p_read_len, "arg 'p_read_len' is null!", HTTP_FAILED);

    rev_len = *p_read_len;

    while (!found_cr_lf)
    {
        // to check chunked size
        if (rev_len >= HC_CR_LF_LEN)
        {
            for (cr_lf_pos = 0; cr_lf_pos < rev_len - 2; cr_lf_pos++)
            {
                if (buf[cr_lf_pos] == '\r' && buf[cr_lf_pos + 1] == '\n')
                {
                    found_cr_lf = true;
                    CIAS_PRINT_DEBUG("Found cr&lf at index %d\n", cr_lf_pos);
                    break;
                }
            }
        }

        if (found_cr_lf)
        {
            break;
        }

        //Try to read more
        if (rev_len < max_len)
        {
            res = http_client_recv(p_client, buf + rev_len, max_len - rev_len, &new_rev_len, 0);
            rev_len += new_rev_len;
            buf[rev_len] = '\0';
            HC_CHECK_ERR(p_client, res, "Receiving  http chunk headers error!", res);
        }
        else
        {
            HC_CHECK_ERR(p_client, HTTP_PRTCL, "Get http chunked size failed:not found cr lf in size line!", HTTP_PRTCL);
        }
    }

    buf[cr_lf_pos] = '\0';

    //read chunked size
    n = sscanf(buf, "%x", &p_client->sz_chunk_size);
    if (n != 1)
    {
        HC_CHECK_ERR(p_client, HTTP_PRTCL, "Get http chunked size failed: Could not read chunk length!", HTTP_PRTCL);
    }

    CIAS_PRINT_DEBUG("HTTP chunked size:%d\n", p_client->sz_chunk_size);

    rev_len -= (cr_lf_pos + HC_CR_LF_LEN);
    memmove(buf, &buf[cr_lf_pos + HC_CR_LF_LEN], rev_len + 1); //Be sure to move NULL-terminating char as well
    *p_read_len = rev_len;

    return HTTP_OK;
}

static e_http_result http_client_recv_chunk(http_client_c *p_client, char *buf, size_t max_len, size_t data_len_in_buf)
{
    size_t need_to_read = 0;
    size_t rev_len = 0;
    size_t temp = 0;
    size_t new_rev_len = 0;
    size_t chunk_buff_len = 0;
    size_t max_read_len = 0;
    e_http_result res = HTTP_OK;

    if (data_len_in_buf > max_len)
    {
        CIAS_PRINT_DEBUG("Args err, data over buffer(%d > %d):\n", data_len_in_buf, max_len);
        HC_CHECK_ERR(p_client, HTTP_FAILED, "Args err!", HTTP_FAILED);
    }
    rev_len = data_len_in_buf;

    while (true)
    {
        if (p_client->check_stop_notify_cb && p_client->check_stop_notify_cb())
        {
            CIAS_PRINT_INFO("Stopped getting media data from url by stop flag!\n");
            goto RET;
        }

        res = http_client_recv_chunked_size(p_client, buf, max_len, &rev_len);
        if (res != HTTP_OK)
        {
            CIAS_PRINT_WARN("Receiving http chunked size error:res=%d\n", res);
            goto RET;
        }

        CIAS_PRINT_DEBUG("*************HTTP chunk size is %d************\n", p_client->sz_chunk_size);

        //last chunk to return
        if (p_client->sz_chunk_size == 0)
        {
            CIAS_PRINT_DEBUG("Receive the last HTTP chunk!!!");
            //to flush the last chunk buffer in the "RET"
            if (chunk_buff_len)
            {
                //add NULL terminate for print string
                p_client->p_chunk_buf[chunk_buff_len] = '\0';
            }
            goto RET;
        }

        //when chunk size is less than max_len of "buf"
        if (p_client->sz_chunk_size + HC_CR_LF_LEN <= max_len)
        {
            if (rev_len < p_client->sz_chunk_size + HC_CR_LF_LEN)
            {
                if (p_client->check_stop_notify_cb && p_client->check_stop_notify_cb())
                {
                    CIAS_PRINT_INFO("Stopped getting media data from url by stop flag!\n");
                    goto RET;
                }

                res = http_client_recv(p_client, buf + rev_len, max_len - rev_len, &new_rev_len, 1);
                rev_len += new_rev_len;
                buf[rev_len] = '\0';

                if (res != HTTP_OK)
                {
                    CIAS_PRINT_WARN("Receiving  http chunked data error:res=%d\n", res);
                    goto RET;
                }
            }

            if (rev_len < p_client->sz_chunk_size + HC_CR_LF_LEN)
            {
                res = HTTP_PRTCL;
                CIAS_PRINT_WARN("No CR&LF after http data chunk: HTTP_PRTCL\n");
                goto RET;
            }

            //when received data is more than a chunk with "\r\n"
            if (rev_len >= p_client->sz_chunk_size + HC_CR_LF_LEN)
            {
                //for -1 reserve the last bytes to do nothing in p_chunk_buf
                if (chunk_buff_len + p_client->sz_chunk_size >= HC_CHUNK_SIZE - 1)
                {
                    temp = HC_CHUNK_SIZE - 1 - chunk_buff_len;
                    memmove(p_client->p_chunk_buf + chunk_buff_len, buf, temp);
                    //add NULL terminate for print string
                    p_client->p_chunk_buf[HC_CHUNK_SIZE - 1] = '\0';
                    chunk_buff_len += temp;
                    //call callback to output

                    if (p_client->data_hdlr_cb)
                    {
                        p_client->data_hdlr_cb(p_client->p_data_hdlr_ctx, p_client->data_pos,
                                               p_client->p_chunk_buf, chunk_buff_len,
                                               p_client->p_http_content_type);
                        p_client->recv_size += chunk_buff_len;
                    }
                    p_client->data_pos = DATA_MID;
                    memmove(p_client->p_chunk_buf, buf + temp, p_client->sz_chunk_size - temp);
                    chunk_buff_len = p_client->sz_chunk_size - temp;
                }
                else
                {
                    memmove(p_client->p_chunk_buf + chunk_buff_len, buf,
                            p_client->sz_chunk_size);
                    chunk_buff_len += p_client->sz_chunk_size;
                }

                //remove "\r\n"
                if (buf[p_client->sz_chunk_size] != '\r' || buf[p_client->sz_chunk_size + 1] != '\n')
                {
                    res = HTTP_PRTCL;
                    CIAS_PRINT_WARN("No CR&LF after http data chunk when remove cr&lf: HTTP_PRTCL\n");
                    goto RET;
                }

                rev_len -= (p_client->sz_chunk_size + HC_CR_LF_LEN);
                //Be sure to move NULL-terminating char as well
                memmove(buf, &buf[p_client->sz_chunk_size + HC_CR_LF_LEN], rev_len + 1);
            }
        }
        else
        { //when chunk size with "\r\n" is more than max_len of "buf"
            CIAS_PRINT_DEBUG("Here to read large HTTP chunk (%d) bytes!\n", p_client->sz_chunk_size);
            if (p_client->sz_chunk_size <= rev_len)
            {
                res = HTTP_FAILED;
                CIAS_PRINT_DEBUG("Receiving  http large chunked data error: sz_chunk_size <= rev_len!\n");
                goto RET;
            }

            //read a large chunk data, (1)read all chunk data first
            need_to_read = p_client->sz_chunk_size - rev_len;

            while (need_to_read)
            {
                if (p_client->check_stop_notify_cb && p_client->check_stop_notify_cb())
                {
                    CIAS_PRINT_INFO("Stopped getting media data from url by stop flag!\n");
                    goto RET;
                }

                if (max_len - rev_len > need_to_read)
                {
                    max_read_len = need_to_read;
                }
                else
                {
                    max_read_len = max_len - rev_len;
                }

                res = http_client_recv(p_client, buf + rev_len, max_read_len, &new_rev_len, 1);
                rev_len += new_rev_len;
                need_to_read -= new_rev_len;
                buf[rev_len] = '\0';
                CIAS_PRINT_DEBUG("Receive %d chars in buf: \n", new_rev_len);

                if (res != HTTP_OK)
                {
                    CIAS_PRINT_WARN("Receiving  http large chunk data error:res=%d, p_client->recv_size = %d\n", res, p_client->recv_size);
                    goto RET;
                }

                //for sub 1 reserve '\0' the last bytes to do nothing in p_chunk_buf
                if (chunk_buff_len + rev_len >= HC_CHUNK_SIZE - 1)
                {
                    temp = HC_CHUNK_SIZE - 1 - chunk_buff_len;
                    memmove(p_client->p_chunk_buf + chunk_buff_len, buf, temp);
                    //add NULL terminate for print string
                    p_client->p_chunk_buf[HC_CHUNK_SIZE - 1] = '\0';
                    chunk_buff_len += temp;
                    //call callback to output

                    if (p_client->data_hdlr_cb)
                    {
                        //wait_ms(1000); use for latency test
                        p_client->data_hdlr_cb(p_client->p_data_hdlr_ctx, p_client->data_pos,
                                               p_client->p_chunk_buf, chunk_buff_len,
                                               p_client->p_http_content_type);
                        p_client->recv_size += chunk_buff_len;
                    }
                    p_client->data_pos = DATA_MID;
                    memmove(p_client->p_chunk_buf, buf + temp, rev_len - temp);
                    chunk_buff_len = rev_len - temp;
                }
                else
                {
                    memmove(p_client->p_chunk_buf + chunk_buff_len, buf, rev_len);
                    chunk_buff_len += rev_len;
                }
                rev_len = 0;
            }
            res = http_client_recv(p_client, buf, HC_CR_LF_LEN, &new_rev_len, 1);
            buf[new_rev_len] = '\0';

            if (res != HTTP_OK)
            {
                CIAS_PRINT_WARN("Receiving cr&lf http large chunked data error:res=%d,  p_client->recv_size = %d\n", res, p_client->recv_size);
                goto RET;
            }

            if ((new_rev_len != HC_CR_LF_LEN) || buf[0] != '\r' || buf[1] != '\n')
            {
                res = HTTP_PRTCL;
                CIAS_PRINT_WARN("No CR&LF after http large data chunk:HTTP_PRTCL\n");
                goto RET;
            }
            // to reset rev_len when run here
            rev_len = 0;
        }
    } //end of while(true)

RET:
    if (res == HTTP_OK)
    {
        p_client->data_pos = DATA_MID;
        if (p_client->data_hdlr_cb)
        {
            p_client->data_hdlr_cb(p_client->p_data_hdlr_ctx,
                                   p_client->data_pos,
                                   p_client->p_chunk_buf,
                                   chunk_buff_len,
                                   p_client->p_http_content_type);
            p_client->recv_size += chunk_buff_len;
        }
    }

    return res;
}

static e_http_result http_client_recv_content_data(http_client_c *p_client, char *buf, size_t max_len, size_t data_len_in_buf)
{
    size_t need_to_read = p_client->n_http_content_len - p_client->recv_size;
    size_t rev_len = data_len_in_buf;
    size_t new_rev_len = 0;
    e_http_result res = HTTP_OK;

    if (data_len_in_buf > max_len || data_len_in_buf > need_to_read)
    {
        CIAS_PRINT_WARN("Args err, data over buffer(%d > %d or  %d):\n", data_len_in_buf, max_len, need_to_read);
        HC_CHECK_ERR(p_client, HTTP_FAILED, "Args err!", HTTP_FAILED);
    }
    need_to_read -= rev_len;

    while (need_to_read || rev_len)
    {
        if (p_client->check_stop_notify_cb && p_client->check_stop_notify_cb())
        {
            CIAS_PRINT_INFO("Stopped getting media data from url by stop flag!\n");
            break;
        }

        res = http_client_recv(
            p_client, buf + rev_len,
            (max_len - rev_len) > need_to_read ? need_to_read : (max_len - rev_len),
            &new_rev_len,
            1);
        // CIAS_PRINT_INFO("-----read rev_len = %d, new_rev_len = %d\r\n", rev_len, new_rev_len);
        rev_len += new_rev_len;
        need_to_read -= new_rev_len;
        buf[rev_len] = '\0';

        if (rev_len)
        {
            if (!p_client->data_hdlr_cb)
            {
                CIAS_PRINT_WARN("Data handler callback is NULL!\n");
                res = HTTP_FAILED;
                goto RET;
            }

            p_client->data_hdlr_cb(p_client->p_data_hdlr_ctx, p_client->data_pos, buf,
                                   rev_len, p_client->p_http_content_type);
            p_client->recv_size += rev_len;
            p_client->data_pos = DATA_MID;
        }

        if (res != HTTP_OK)
        {
            CIAS_PRINT_WARN("Receive http data finished !errcode= %d when remaining %d bytes to receive\n", res, need_to_read);
            break;
        }

        if (need_to_read > 0 && rev_len == 0)
        {
            CIAS_PRINT_WARN("Remaining %d bytes cannot receive,and give up\n", need_to_read);
            res = HTTP_FAILED;
            break;
        }
        rev_len = 0;
    }

RET:
    return res;
}

void cias_http_client_get_download_progress(http_client_c *p_client, size_t *total_size, size_t *recv_size)
{
    if (!p_client || !total_size || !recv_size)
    {
        CIAS_PRINT_WARN("Args error!\n");
        return;
    }

    *total_size = p_client->n_http_content_len;
    *recv_size = p_client->recv_size;
}

static e_http_result http_client_parse_redirect_location(http_client_c *p_client, const char *p_path, const size_t path_len, bool *is_relative_url)
{
    size_t len = 0;

    if ((!p_client) || (!is_relative_url) || (!p_path))
    {
        CIAS_PRINT_WARN("Invalid param!\n");
        return HTTP_PARSE;
    }

    if (!p_client->p_location)
    {
        CIAS_PRINT_WARN("Invalid location!\n");
        return HTTP_PARSE;
    }

    if (p_client->p_location[0] == 'h')
    {
        *is_relative_url = false;
        return HTTP_OK;
    }

    *is_relative_url = true;

    // the relative path's format may be "./path", "../path"
    if (p_client->p_location[0] != '/')
    {
        // one more byte for '/', one more byte for '\0'
        len = strlen(p_path) + strlen(p_client->p_location) + 2;
        char new_path[1024] = {0};
        snprintf(new_path, len, "%s", p_path);

        // for example: the previous path is "p1/p2", the relative path is "../",
        // the new path should be "p1/p2/../", we should add a '/'
        if (p_path[strlen(p_path)] != '/')
        {
            strncat(new_path, "/", 1);
        }

        strncat(new_path, p_client->p_location, HC_CHUNK_SIZE);
        CIAS_PRINT_INFO("new path: %s\n", new_path);
        free(p_client->p_location);
        p_client->p_location = NULL;
        p_client->p_location = (char *)malloc(strlen(new_path) + 1);
        strcpy(p_client->p_location, new_path);
    }

    return HTTP_OK;
}

static e_http_result http_client_connect(http_client_c *p_client, const char *url, e_http_meth method, size_t *remain_size_in_buf)
{
    char scheme[HC_SCHEME_LEN_MAX];
    char *p_path = NULL;
    char *host = NULL;
    bool is_relative_url = false;
    int path_len_max = 0;
    int ret = 0;
    int redirect_count = 0;
    size_t rev_len = 0;
    unsigned short port = 0;
    e_http_result res = HTTP_OK;

    p_client->redirections_too_more = false;
    host = (char *)cias_malloc(HC_HOST_LEN_MAX + 1); //including NULL-terminating char
    if (host == NULL)
    {
        CIAS_PRINT_WARN("No memory!\n");
        return HTTP_PARSE;
    }

    while (redirect_count <= HC_REDIRECTION_MAX)
    {
        if (!url)
        {
            CIAS_PRINT_WARN("url is NULL!\n");
            res = HTTP_FAILED;
            goto RET;
        }

        if (HC_CHUNK_SIZE <= HC_RSP_CHUNK_SIZE || HC_CHUNK_SIZE <= HC_HEADER_CHUNK_SIZE)
        {
            CIAS_PRINT_WARN("http client chunk size config error!!!\n");
            res = HTTP_FAILED;
            goto RET;
        }

        path_len_max = strlen(url) + 1;
        p_path = (char *)cias_malloc(path_len_max);
        if (!p_path)
        {
            CIAS_PRINT_WARN("Mallco memory(size:%d) Failed!\n", path_len_max);
            res = HTTP_FAILED;
            goto RET;
        }

        //to parse url
        res = http_client_parse_url(url, scheme, sizeof(scheme), host, &port, p_path, path_len_max, is_relative_url);
        if (res != HTTP_OK)
        {
            CIAS_PRINT_WARN("parseURL returned %d\n", res);
            res = HTTP_FAILED;
            goto RET;
        }

        //to set default port
        if (port == 0)
        { //to do handle HTTPS->443
            port = 80;
        }

        CIAS_PRINT_INFO("Scheme: %s\n", scheme);
        CIAS_PRINT_INFO("Host: %s\n", host);
        CIAS_PRINT_INFO("Port: %d\n", port);
        CIAS_PRINT_INFO("Path: %s\n", p_path);
        CIAS_PRINT_INFO("********************>>>>>Heap left: %d <1> min:%d<<<<<********************\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
        //to connect server
        ret = p_client->ops.connect(&(p_client->ops.n_handle), host, port);
        if (ret < 0)
        {
            if (p_client->ops.n_handle > 0)
            {
                p_client->ops.close(p_client->ops.n_handle);
            }
            p_client->ops.n_handle = -1;
            CIAS_PRINT_WARN("Could not connect(%d): %s:%d\n", ret, host, port);
            res = HTTP_CONN;
            goto RET;
        }
        CIAS_PRINT_DEBUG("Connecting socket to server success:%d\n", ret);

        //to send http request
        res = http_client_send_request(p_client, method, p_path, host, port);
        if (res != HTTP_OK)
        {
            goto RET;
        }

        //to send http headers
        res = http_client_send_headers(p_client, p_client->buf, HC_CHUNK_SIZE);
        if (res != HTTP_OK)
        {
            goto RET;
        }

        //to receive http response
        res = http_client_recv_rsp_and_parse(p_client, p_client->buf, HC_RSP_CHUNK_SIZE, &rev_len);
        CIAS_PRINT_INFO("res = %d\n", res);
        if (res != HTTP_OK)
        {
            goto RET;
        }
        //to receive http headers and pasrse it
        res = http_client_recv_headers_and_parse(p_client, p_client->buf, HC_HEADER_CHUNK_SIZE, &rev_len);
        if (res == HTTP_REDIRECTTION)
        {
            res = http_client_parse_redirect_location(p_client, p_path, path_len_max, &is_relative_url);
            if (res != HTTP_OK)
            {
                goto RET;
            }
            cias_free(p_path);
            p_path = NULL;
            url = p_client->p_location;
            redirect_count++;
            CIAS_PRINT_INFO("Taking %d redirections to [%s]\n", redirect_count, url);
            continue;
        }
        else if (res != HTTP_OK)
        {
            goto RET;
        }
        else
        {
            if (p_client->get_url_cb)
            {
                p_client->get_url_cb(url);
            }
            break;
        }
    }

    if (redirect_count > HC_REDIRECTION_MAX)
    {
        CIAS_PRINT_WARN("Too many redirections!\n");
        p_client->redirections_too_more = true;
    }

RET:
    if (p_path)
    {
        cias_free(p_path);
    }

    if (host)
    {
        cias_free(host);
    }

    *remain_size_in_buf = rev_len;

    return res;
}

e_http_result cias_http_client_get_data(http_client_c *p_client, size_t remain_size_in_buf)
{
    e_http_result res = HTTP_OK;
    memset(p_client->buf,0,HC_CHUNK_SIZE);
    //to receive http chunk data
    if (p_client->n_is_chunked)
    {
        res = http_client_recv_chunk(p_client, p_client->buf, HC_CHUNK_SIZE - 1, remain_size_in_buf);
        CIAS_PRINT_INFO("1-Receiving  http chunk data\n");
    }
    else
    {
        //to receive http data content
        res = http_client_recv_content_data(p_client, p_client->buf, HC_CHUNK_SIZE - 1, remain_size_in_buf);
        CIAS_PRINT_DEBUG("2-Receiving  http data content\n");
    }
    p_client->ops.close(p_client->ops.n_handle);
    p_client->ops.n_handle = -1;
    if (res == HTTP_OK)
    {
        CIAS_PRINT_INFO("Completed HTTP transaction\n");
    }
    return res;
}

e_http_result cias_http_client_get(http_client_c *p_client, const char *url, const size_t pos, bool isTTS)
{
    size_t rev_len = 0;
    e_http_result ret = HTTP_OK;

    // CIAS_PRINT_DEBUG("http get url: %s\n", url);

    p_client->data_pos = DATA_FIRST;
    p_client->resume_retry_count = 0;
    p_client->recv_size = pos;
    http_client_set_content_len(p_client, 0);

    // CIAS_PRINT_DEBUG("cias_http_client_get heap size: %d\n", xPortGetFreeHeapSize());

    p_client->buf = (char *)malloc(HC_CHUNK_SIZE);
    CIAS_PRINT_DEBUG("http get url: %s\n", url);

    if (!p_client->buf)
    {
        ret = HTTP_FAILED;
        goto RET;
    }

    while (p_client->resume_retry_count <= HC_MAX_RETRY_RESUME_COUNT)
    {
        if (p_client->check_stop_notify_cb && p_client->check_stop_notify_cb())
        {
            CIAS_PRINT_INFO("Stopped getting media data from url by stop flag!\n");
            break;
        }
        // create http connect
        ret = http_client_connect(p_client, url, HTTP_GET, &rev_len);
        if ((ret == HTTP_CONN) || (ret == HTTP_TIMEOUT))
        {
            p_client->resume_retry_count++;
            CIAS_PRINT_INFO("Try to resume from break-point %d time\n", p_client->resume_retry_count);
            continue;
        }

        if (ret != HTTP_OK)
        {
            CIAS_PRINT_INFO("break !!!!!!!!!!!!!\n");
            break;
        }

        // get http data
        ret = cias_http_client_get_data(p_client, rev_len);
        if ((ret == HTTP_CONN) || (ret == HTTP_TIMEOUT))
        {
            if (isTTS)
            {
                if (p_client->recv_size == 0)
                {
                    break;
                }
                else
                {
                    if (p_client->n_is_chunked)
                    {
                        break;
                    }
                    else
                    {
                        p_client->resume_retry_count++;
                        if (p_client->resume_retry_count <= HC_MAX_RETRY_RESUME_COUNT)
                        {
                            CIAS_PRINT_INFO("Resume from break-point:retry %d time  p_client->recv_size = %d\n", p_client->resume_retry_count, p_client->recv_size);
                        }
                        else
                        {
                            CIAS_PRINT_WARN("Resume from break-point too many times\n");
                        }
                        continue;
                    }
                }
            }
            else
            {
                p_client->resume_retry_count++;
                if (p_client->resume_retry_count <= HC_MAX_RETRY_RESUME_COUNT)
                {
                    CIAS_PRINT_INFO("Resume from break-point:retry %d time  p_client->recv_size = %d\n", p_client->resume_retry_count, p_client->recv_size);
                }
                else
                {
                    CIAS_PRINT_WARN("Resume from break-point too many times\n");
                }
                continue;
            }
        }

        break;
    }

    CIAS_PRINT_INFO("HTTP: %d bytes received\n", p_client->recv_size);

    if (ret != HTTP_OK)
    {
    }

RET:
    if ((p_client->data_pos != DATA_FIRST) && (p_client->data_hdlr_cb))
    {
        p_client->data_hdlr_cb(p_client->p_data_hdlr_ctx, DATA_LAST, NULL, 0, p_client->p_http_content_type);
    }

    if (p_client->buf)
    {
        free(p_client->buf);
        p_client->buf = NULL;
    }

    if (p_client->ops.n_handle > 0)
    {
        p_client->ops.close(p_client->ops.n_handle);
    }
    p_client->ops.n_handle = -1;

    return ret;
}
