#ifndef _CIAS_HTTP_OTA_PORT_H_
#define _CIAS_HTTP_OTA_PORT_H_

#ifdef __cplusplus
    extern "C" {
#endif

#define UPDATE_ALL_PARTITION        0       //升级全部分区(1:打开，0:关闭)  打开:将升级全部分区。关闭: 根据语音芯片分区表和文件分区表对比结果,升级需要升级的分区
#define UPDATE_FORENSIC_DOC         0       //是否升级鉴权文件(1:打开，0:关闭)   未开发

/**
 * @brief 创建OTA任务
 * 
 * @retval -1: error
 * @retval 0: successful
 */
int cias_http_ota_port_task_create(void);
/**
 * @brief oclp硬件接收函数，通过接收到的数据进行通信协议解析
 * 
 * @param buff: 原始数据
 * @param len: 数据长度
 * @retval 0: 
 */
int http_ota_olcp_recv(unsigned char* buff, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif
