/**
 * @file cias_http_download.c
 * @brief HTTP长连接会话通信(适合嵌入式设备下载http大文件)
 * @author JasonChan (x@chencc.cc)
 * @date 2021-04-20
 * @copyright Copyright (c) 2021
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "cias_freertos_common.h"
#include "cias_http_download.h"
#include "cias_log.h"  
/**
 * @brief socket关闭fd
 * 
 * @param fd: fd指针
 */
static void _socket_close_fd(int *fd)
{
    if((*fd) >= 0)
    {
        close(*fd);
    }
    (*fd) = -1;
}
/**
 * @description: socket 发送函数
 * @param { fd :已经建立连接的socket fd
 *          data:发送数据
 *          len:发送数据长度} 
 * @return {-1 :err
 *          0  :successful}
 */
static int _socket_send(int fd, void *data, unsigned int len)
{
    int ret, w_offset = 0, w_block;

    if(data == NULL)
    {
        return 0;
    }
    
    while (w_offset < len)
    {
        w_block = len - w_offset;
        ret = send(fd, data + w_offset, w_block, 0);
        CIAS_ASSERT_RETURN(ret >= 0);
        w_offset += ret;
    }

    return 0;
}
/**
 * @brief socket接收指定长度的数据
 * 
 * @param fd: fd
 * @param data: 缓存buffer
 * @param len: 接收数据长度
 * @retval -1: error
 * @retval >=0: 接收到的数据长度
 */
static int _socket_recv(int fd, void *data, unsigned int len)
{
    int remain_len = len;
    int rc;

    while (1)
    {
        rc = recv(fd, data, remain_len, 0);
        CIAS_ASSERT_RETURN(rc>=0);
        data+=rc;
        remain_len-=rc;

        if(remain_len <= 0)  //接收完毕
            break;
    }

    return (len-remain_len);
}

/**
 * @description: 使用url建立socket连接.返回socket fd
 * @param { url :目标url
 *          port:目标端口} 
 * @return {-1 :err
 *          其他 :successful}
 */
static int _open_socket_by_url(char *url, int port)
{
    int fd = -1, rc;
    struct sockaddr_in addr_info;
    struct addrinfo *g_addr_res = NULL, *ipv4_info = NULL;
    struct addrinfo g_hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
    struct timeval timeout = {3,0};

    rc = getaddrinfo(url, NULL, &g_hints, &g_addr_res);
    CIAS_ASSERT_GOFAIL(rc==0);
    //寻找一个ipv4地址
    ipv4_info = g_addr_res;
    while (ipv4_info != NULL)
    {
        if (ipv4_info->ai_family == AF_INET) //ipv4
            break;
        ipv4_info = ipv4_info->ai_next;
    }
    CIAS_ASSERT_GOFAIL(ipv4_info!=NULL); 
    //找到一个ipv4地址
    addr_info.sin_addr = ((struct sockaddr_in*)(ipv4_info->ai_addr))->sin_addr;
    addr_info.sin_family = AF_INET;
    addr_info.sin_port = htons(port);
    //创建socket
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CIAS_ASSERT_GOFAIL(fd!=-1);
    rc = connect(fd, (struct sockaddr*)&addr_info, sizeof(struct sockaddr)); //默认127s超时
    CIAS_ASSERT_GOFAIL(rc==0);
    //设置超时时间3s
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    goto __success;
__fail:
    _socket_close_fd(&fd);
__success:
    if(g_addr_res != NULL)
    {
        freeaddrinfo(g_addr_res);
        g_addr_res = NULL;
    }
    return fd;
}

/**
 * @description: HTTP的请求
 * @param { fd :已经建立连接的socket fd
 *          req_line:请求行
 *          headers:请求头部
 *          body:请求body
 *          body_len:请求body长度} 
 * @return {-1 :err
 *          0  :successful}
 */
static int _http_request(int fd, char *req_line, char *headers, void *body, unsigned int body_len)
{
    int req_line_len = strlen(req_line);
    int headers_len = strlen(headers);
    int rc;

    //限制请求url小于2K
    CIAS_ASSERT_RETURN((req_line_len + headers_len) < 2048);
    //发送 req_line
    rc = _socket_send(fd, req_line, req_line_len);
    CIAS_ASSERT_RETURN(rc!=-1);
    //发送 headers
    rc = _socket_send(fd, headers, headers_len);
    CIAS_ASSERT_RETURN(rc!=-1);
    //发送 body
    rc = _socket_send(fd, body, body_len);
    CIAS_ASSERT_RETURN(rc!=-1);

    return 0;
}
/**
 * @brief http response的回应行段关键字解析
 * 
 * @param resp_header: 回应行
 * @param key: 关键字
 * @param value: 关键字的值解析结果
 * @param value_len: 关键字缓存长度
 * @retval -1: error
 * @retval 0: successful
 */
static int _http_response_header_str_parse(char *resp_header, char *key, char *value, unsigned int value_len)
{
    char *tmp_p1 = NULL, *tmp_p2 = NULL;

    tmp_p1 = strstr(resp_header, key);//找到key出现的位置
    CIAS_ASSERT_RETURN(tmp_p1!=NULL);
    tmp_p1+=strlen(key);
    while ((*tmp_p1)==':' || (*tmp_p1)==' ')  //跳过空格和冒号
    {
        tmp_p1++;
    }
    tmp_p2 = strstr(tmp_p1, "\r");  //找到末尾的'\r'
    CIAS_ASSERT_RETURN(tmp_p2!=NULL);
    CIAS_ASSERT_RETURN((tmp_p2-tmp_p1)<value_len);
    memcpy(value, tmp_p1, tmp_p2-tmp_p1);
    value[tmp_p2-tmp_p1] = '\0';
    
    return 0;
}
/**
 * @description: HTTP的响应头
 * @param { http_handle : } 
 * @return {-1 :err
 *          0:successful}
 */
static int _http_response_header(cias_http_t *http_handle)
{
    // 字符串长度256
    // "Accept-Ranges: bytes\r\n"
    // "ETag: W/\"8256035-1600502177000\""
    // "Last-Modified: Sat, 19 Sep 2020 07:56:17 GMT"
    // "Content-Range: bytes 0-1/8256035"
    // "Content-Type: application/octet-stream"
    // "Content-Length: 2"
    // "Date: Wed, 04 Nov 2020 02:19:55 GMT"
    // "Keep-Alive: timeout=20"
    // "Connection: keep-alive"
    // "\r\n"  
    #define resp_header_LMT 450     //响应头buffer长度限制  根据具体响应头长度设置
    #define value_str_LMT 50        //储存键值的字符串长度限制
    char header_buff[resp_header_LMT];
    char *p_header = header_buff;
    char *tmp_p1 = NULL;
    char value_str[value_str_LMT];
    int rc, recv_one_packet_len = 4;

    //接收响应头
    while (1)
    {
        rc = recv(http_handle->socket_fd, p_header, recv_one_packet_len, 0);
        CIAS_ASSERT_RETURN(rc>=0);
        p_header+=rc;

        tmp_p1 = p_header-1; //最后一byte
        recv_one_packet_len = 4;
        while (recv_one_packet_len > 0 && ((*tmp_p1)=='\r' || (*tmp_p1)=='\n'))
        {
            recv_one_packet_len--;
            tmp_p1--;
        }
        if (recv_one_packet_len == 0)  //找到"\r\n\r\n"
            break;
        CIAS_ASSERT_RETURN((p_header-header_buff)<resp_header_LMT);//限制接收头大小
    }
    p_header = '\0'; //停止符
    
    //解析响应头
    rc = _http_response_header_str_parse(header_buff, "Content-Length", value_str, value_str_LMT);
    CIAS_ASSERT_RETURN(rc==0);
    http_handle->resp->header.Content_Length = atoi(value_str);

    rc = _http_response_header_str_parse(header_buff, "Content-Range", value_str, value_str_LMT);
    CIAS_ASSERT_RETURN(rc==0);
        tmp_p1 = strstr(value_str, "/");
    CIAS_ASSERT_RETURN(tmp_p1!=NULL);
        tmp_p1+=1; //跳过'/'
    // CIAS_LOG_DEBUG(HTTP_LOG"file len:%s", tmp_p1);
        http_handle->resp->header.Content_Range = atoi(tmp_p1);
    
    return 0;
}
/**
 * @description: HTTP的url解析，从url中解析出host、port、path
 *              例如：url: http://121.199.52.33:8080/ci1103_0.bin
 *              解析结果：host: 121.199.52.33. port: 8080. path: ci1103_0.bin
 * @param { http_handle :http结构体
 *          url:url} 
 * @return {-1 :err
 *          0  :successful}
 */
static int _http_url_parse(cias_http_t *http_handle, const char *url)
{
    int rc, req_line_len;
    const char *p_url = url;
    const char *path_ptr = NULL;
    const char *port_ptr = NULL;
    char port_str[7];

    //剔除http://
    rc = strncmp(p_url, "http://", 7);
    CIAS_ASSERT_RETURN(rc==0);
    p_url += 7;
    //找到host
    port_ptr = strstr(p_url, ":");
    CIAS_ASSERT_RETURN(port_ptr!=NULL);
    path_ptr = strstr(p_url, "/");
    CIAS_ASSERT_RETURN(path_ptr!=NULL);
        CIAS_ASSERT_RETURN(port_ptr<path_ptr);

    http_handle->req->header.Host = cias_malloc(sizeof(char)*((port_ptr-p_url)+1));
    CIAS_ASSERT_RETURN(http_handle->req->header.Host != NULL);
    memcpy(http_handle->req->header.Host, p_url, port_ptr-p_url);
    http_handle->req->header.Host[port_ptr-p_url] = '\0';
        //找到port
        if((path_ptr-port_ptr) < 6) //限制port长度
        {
            memcpy(port_str, port_ptr+1, path_ptr-port_ptr-1);
            port_str[path_ptr-port_ptr] = '\0';
            http_handle->port = atoi(port_str);
        }
        else
        {
          CIAS_LOG_WARN(HTTP_LOG"input port err, use default port(80)");
        }
    //找到路径
    req_line_len = strlen(url)-(path_ptr-url)+strlen(HTTP_REQUEST_LINE);
    http_handle->req->line = cias_malloc(sizeof(char)*req_line_len);
    if(http_handle->req->line == NULL)
    {
        cias_free(http_handle->req->header.Host);
        CIAS_ASSERT_RETURN(http_handle->req->line != NULL);
    }
    sprintf(http_handle->req->line, HTTP_REQUEST_LINE, path_ptr);

    return 0;
}
/**
 * @brief 创建http会话
 * 
 * @param url: url
 * @retval NULL: error
 * @retval !NULL: successful
 */
cias_http_t* cias_http_session_create(char *url)
{
    int rc;
    char tmp_buff[3];
    cias_http_t *http_handle = cias_malloc(sizeof(cias_http_t));

    //http 初始化
    CIAS_ASSERT_GOFAIL(http_handle!=NULL);
    http_handle->socket_fd = -1;
    http_handle->port = 80; /* default port of 80(http) */
    http_handle->file_offset = -1;
    http_handle->req = cias_malloc(sizeof(request_t));
    CIAS_ASSERT_GOFAIL(http_handle->req!=NULL);
    http_handle->resp = cias_malloc(sizeof(response_t));
    CIAS_ASSERT_GOFAIL(http_handle->resp!=NULL);
    //http request初始化

    http_handle->req->body = NULL;
    http_handle->req->line = NULL;
    http_handle->req->header.Host = NULL;
    //http response初始化
    http_handle->resp->body = NULL;
    http_handle->resp->status = 0;
    http_handle->resp->header.Content_Range = 0;
    //解析url
    rc = _http_url_parse(http_handle, url);
    CIAS_ASSERT_GOFAIL(rc==0);
    //先发送一次http请求以获取文件大小
    rc = cias_http_download_file(http_handle, tmp_buff, 0, 1);
    CIAS_ASSERT_GOFAIL(rc!=-1);
    goto __success;
__fail:
    if (http_handle != NULL)
    {
        if (http_handle->req != NULL)
        {
            cias_free(http_handle->req->header.Host);
            cias_free(http_handle->req->line);
            cias_free(http_handle->req);
        }
        if (http_handle->resp != NULL)
        {
            cias_free(http_handle->resp);
        }
        cias_free(http_handle);
    }
__success:
    return http_handle;
}
/**
 * @brief 关闭http会话，断开socket连接
 * 
 * @param http_handle: 会话句柄
 * @retval -1: error
 * @retval 0: successful
 */
int cias_http_session_close(cias_http_t *http_handle)
{
    CIAS_ASSERT_RETURN(http_handle!=NULL);
    _socket_close_fd(&(http_handle->socket_fd));
    if (http_handle->req != NULL)
    {
        cias_free(http_handle->req->header.Host);
        cias_free(http_handle->req->line);
        cias_free(http_handle->req);
    }
    if (http_handle->resp != NULL)
    {
        cias_free(http_handle->resp);
    }
    cias_free(http_handle);

    return 0;
}

/**
 * @description: HTTP文件下载, need_len必须小于等于in_buff大小，不然会出现越界
 * @param { http_handle :http结构体
 *          in_buff:接收缓存，下载的数据将存入里面
 *          offset:文件下载偏移
 *          need_len:下载文件块大小} 
 * @return {-1 :err
 *          其他 :实际下载文件块大小}
 */
int cias_http_download_file(cias_http_t *http_handle, void *in_buff, unsigned int offset,unsigned int need_len)
{
    char *req_header = NULL;
    int rc;

    CIAS_ASSERT_RETURN(http_handle!=NULL);
    CIAS_ASSERT_RETURN(in_buff!=NULL);
    CIAS_ASSERT_RETURN(need_len>0);
    if (http_handle->resp->header.Content_Range != 0)
    {
        CIAS_ASSERT_RETURN((offset+need_len)<=http_handle->resp->header.Content_Range);
    }

    if((http_handle->socket_fd == -1) || (offset != http_handle->file_offset)) //如果http关闭，则进行重连.如果偏移不一致  则发送http请求从新设置偏移
    {
        _socket_close_fd(&(http_handle->socket_fd));
        http_handle->socket_fd = _open_socket_by_url(http_handle->req->header.Host, http_handle->port);
        CIAS_ASSERT_RETURN(http_handle->socket_fd != -1);
        CIAS_LOG_HL(HTTP_LOG"Connected. Host:%s. Port:%d.", http_handle->req->header.Host, http_handle->port);
        http_handle->file_offset = -1; //清除记录偏移

        req_header = cias_malloc(strlen(HTTP_REQUEST_HEADER)+strlen(http_handle->req->header.Host)+20);
        CIAS_ASSERT_RETURN(req_header!=NULL);
        sprintf(req_header, HTTP_REQUEST_HEADER, http_handle->req->header.Host, offset);
        rc = _http_request(http_handle->socket_fd, http_handle->req->line, req_header, NULL, 0);
        cias_free(req_header);
        CIAS_ASSERT_GOFAIL(rc!=-1);

        rc = _http_response_header(http_handle);
        CIAS_ASSERT_GOFAIL(rc==0);

    }

    rc = _socket_recv(http_handle->socket_fd, in_buff, need_len);
    CIAS_ASSERT_GOFAIL(rc!=-1);
    http_handle->file_offset = offset+rc;
    
    return rc;

__fail:
    _socket_close_fd(&(http_handle->socket_fd));
    return -1;
}
