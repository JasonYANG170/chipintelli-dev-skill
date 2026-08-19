/**
 * @file    cias_http_client_ops.c
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */
#include <stdarg.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "cias_http_client.h"
#include "cias_freertos_debug.h"
#include "cias_freertos_common.h"

static int socket_init(void *socket_args)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        CIAS_PRINT_WARN("socket create failed: %d\n", fd);
    }

    return fd;
}

static int socket_open(int socket_handle)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        CIAS_PRINT_WARN("socket create failed: %d\n", fd);
    }

    return fd;
}

static int socket_connect(int *socket_handle, const char *host, const int port)
{
    *socket_handle = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_handle < 0)
    {
        CIAS_PRINT_WARN("socket create failed: %d\n", *socket_handle);
        *socket_handle = -1;
        return -1;
    }
    struct addrinfo hints, *addr_list, *cur;
    int ret = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char cport[10] = {0};
    snprintf(cport, sizeof(cport), "%d", port);
    if (getaddrinfo(host, cport, &hints, &addr_list) != 0)
    {
        CIAS_PRINT_WARN("getaddrinfo != 0, return HTTPCLIENT_UNRESOLVED_DNS\n");

        close(*socket_handle);
        *socket_handle = -1;

        ret = -2;
    }

    /* Try the sockaddrs until a connection succeeds */
    for (cur = addr_list; cur != NULL; cur = cur->ai_next)
    { 
        if (connect(*socket_handle, cur->ai_addr, (int)cur->ai_addrlen) == 0)
        {
            //  lwip_set_recv_timeout(*socket_handle, 3000);
            CIAS_PRINT_WARN("HTTP OPS: Connected to server\n");
            ret = 0;
            break;
        }

        close(*socket_handle);
        *socket_handle = -1;
        CIAS_PRINT_WARN("HTTP OPS: Connect to server failed!\n");
        ret = -3;
    }

    freeaddrinfo(addr_list);

    return ret;
}

static void socket_set_blocking(int socket_handle, int blocking)
{
}

static void socket_set_timeout(int socket_handle, int send_timeout)
{
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0) // lwip 1.5.0
    struct timeval timeout;
    timeout.tv_sec = send_timeout / 1000;
    timeout.tv_usec = (send_timeout % 1000) * 1000;
    setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#else
    setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &send_timeout, sizeof(send_timeout));
#endif
}

static int socket_recv(int socket_handle, void *data, unsigned size)
{
    int ret = 0;

    ret = recv(socket_handle, data, size, 0);
    if (ret < 0)
    {
        CIAS_PRINT_WARN("HTTP OPS: Recv failed %d\n", ret);
    }

    return ret;
}

static int socket_send(int socket_handle, const void *data, unsigned size)
{
    int ret = 0;

    ret = send(socket_handle, data, size, 0);
    if (ret == -1)
    {
        CIAS_PRINT_INFO("HTTP OPS: Send failed %d\n", ret);
    }
    else
    {
        CIAS_PRINT_INFO("HTTP OPS: Send succeeded\n");
    }

    return ret;
}

static int socket_close(int socket_handle)
{
    int ret = 0;

    ret = close(socket_handle);
    if (ret < 0)
    {
        CIAS_PRINT_WARN("HTTP OPS: Close failed %d\n", ret);
    }

    return 0;
}

static void socket_destroy(int socket_handle)
{
    int ret = close(socket_handle);
    if (ret < 0)
    {
        CIAS_PRINT_WARN("HTTP OPS: Close failed %d\n", ret);
    }

    return;
}

static struct http_client_socket_ops ops =
    {
        .init = socket_init,
        .open = socket_open,
        .connect = socket_connect,
        .set_blocking = socket_set_blocking,
        .set_timeout = socket_set_timeout,
        .recv = socket_recv,
        .send = socket_send,
        .close = socket_close,
        .destroy = socket_destroy,
};
static const int HC_CHUNK_SIZE = 1025;
http_client_c *cloud_create_http_client(void)
{
    int ret = 0;
    http_client_c *client = NULL;

    client = (http_client_c *)malloc(sizeof(*client));
    if (client == NULL)
    {
        CIAS_PRINT_WARN("OTA Downloader: Malloc failed\n");
        goto out;
    }

    ret = cias_http_client_init(client);
    if (ret != HTTP_OK)
    {
        CIAS_PRINT_WARN("HTTP OPS:Init http client failed\n");
        goto init_client_err;
    }

    ret = cias_http_client_init_socket_ops(client, &ops, NULL);
    if (ret != HTTP_OK)
    {
        CIAS_PRINT_WARN("HTTP OPS:Init socket ops failed\n");

        goto init_socket_ops_err;
    }

    client->p_chunk_buf = (char *)cias_malloc(HC_CHUNK_SIZE);
    if (client->p_chunk_buf == NULL)
    {
        goto init_socket_ops_err;
    }

    goto out;

init_socket_ops_err:
init_client_err:
    cias_free(client);
    client = NULL;
out:
    return client;
}
