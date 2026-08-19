#ifndef _CIAS_HTTP_DOWNLOAD_H_
#define _CIAS_HTTP_DOWNLOAD_H_


#ifdef __cplusplus
    extern "C" {
#endif

/**
 * HTTP请求行定义
*/
#define         HTTP_REQUEST_LINE       "POST %s HTTP/1.1\r\n"
/**
 * HTTP请求头定义
*/
#define         HTTP_REQUEST_HEADER     "Host: %s\r\n"\
                                        "Connection: close\r\n"\
                                        "Content-Length: 0\r\n"\
                                        "Range: bytes=%d-\r\n"\
                                        "\r\n"
/**
 * HTTP请求头 字段名对应的值定义
*/
typedef struct 
{
    char            *Host;
}req_header_t;
/**
 * HTTP响应头 字段名对应的值定义
*/
typedef struct 
{
    int             Content_Range;      //文件大小
    int             Content_Length;     //body长度
}resp_header_t;
/**
 * HTTP请求定义
*/
typedef struct 
{
    char            *line;      //请求行
    req_header_t    header;     //请求头
    char            *body;      //请求数据
}request_t;
/**
 * HTTP响应定义
*/
typedef struct 
{
    int             status;     //状态码
    resp_header_t   header;     //响应头
    char            *body;      //响应体
}response_t;

typedef struct
{
    int                 socket_fd;  //建立连接的socket fd
    int                 port;       //服务器端口
    response_t          *resp;      //响应
    request_t           *req;       //请求
    int                 file_offset;//下载文件目前偏移
}cias_http_t;
/**
 * @brief 创建http会话
 * 
 * @param url: url
 * @retval NULL: error
 * @retval !NULL: successful
 */
cias_http_t* cias_http_session_create(char *url);
/**
 * @brief 关闭http会话，断开socket连接
 * 
 * @param http_handle: 会话句柄
 * @retval -1: error
 * @retval 0: successful
 */
int cias_http_session_close(cias_http_t *http_handle);
/**
 * @description: HTTP文件下载, need_len必须小于等于in_buff大小，不然会出现越界
 * @param { http_handle :http结构体
 *          in_buff:接收缓存，下载的数据将存入里面
 *          offset:文件下载偏移
 *          need_len:下载文件块大小} 
 * @return {-1 :err
 *          其他 :实际下载文件块大小}
 */
int cias_http_download_file(cias_http_t *http_handle, void *in_buff, unsigned int offset,unsigned int need_len);


#ifdef __cplusplus
}
#endif
#endif
