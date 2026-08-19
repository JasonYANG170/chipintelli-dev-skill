#ifndef _CIAS_OLCP_EXPORT_H_
#define _CIAS_OLCP_EXPORT_H_

#include "cias_olcp.h"
#ifdef __cplusplus
    extern "C" {
#endif

/**
 * OLCP协议data字段buff长度定义(根据通信中data字段最长的一帧定义)
*/
#define OLCP_RECV_DATA_LMT        	1024  //最大接受数据DATA段长度
#define OLCP_SEND_DATA_LMT        	4200  //最大发送数据DATA段长度
/**
 * OLCP协议定义ack丢包重发机制
*/
#define OLCP_ACK_RESEND		 	1  		//丢包重发机制开关   1:打开 0:关闭
#define OLCP_ACK_RESEND_MAX_NUM	3  		//丢包重发最大次数
#define	OLCP_ACK_RESEND_T		100		//丢包重发时间间隔(时间间隔=OLCP_ACK_RESEND_T(ms))
/**
 * OLCP协议定义枚举CMD
*/
typedef enum{
	/****************************公用固定CMD段****************************/
	OTA_NEED_FLASH_DATA_OLCP				=  0x0801,  //[ci->wifi] ota需要flash数据
	OTA_EARSE_PROGRESS_OLCP					=  0x0802,  //[ci->wifi] ota擦除进度
	OTA_UPDATE_BLOCK_DONE_OLCP				=  0x0803,  //[ci->wifi] ota更新块完毕
	OTA_SET_PARTITION_TABLE_OLCP			=  0x0804,	//[ci->wifi] 发送CI110X分区表信息 
	
	OTA_WRITE_FLASH_DATA_OLCP				=  0x0805,  //[wifi->ci] 发送写flash数据
	OTA_ENTER_OTA_MODE_OLCP		 			=  0x0807,  //[wifi->ci] 发送使CI110X进入OTA模式
	OTA_CHECK_DEVICE_READY_OLCP				=  0x0808,  //[wifi->ci] 检查设备是否准备好
	OTA_GET_PARTITION_TABLE_OLCP			=  0x0809,	//[wifi->ci] 得到CI110X分区表信息
	OTA_NEED_ERASE_PARTITION_OLCP			=  0x080b,	//[wifi->ci] 需要擦除的分区
	OTA_SYS_RST								=  0x080f,	//[wifi->ci] 使CI110X系统重启
	OTA_VERIFY_PARTITION_OLCP				=  0x0810,	//[wifi<->ci] 校验分区
    /****************************自定义CMD段******************************/
}cias_olcp_cmd_e;

typedef struct cias_olcp_def cias_olcp_t;
/**
 * @brief 创建olcp通信句柄，申请内存
 * 
 * @param hal_send: 硬件发送函数
 * @param olcp_hook: 解析完成回调钩子
 * @retval NULL: error
 * @retval !NULL: successful
 */
cias_olcp_t* cias_olcp_create(olcp_send_t hal_send, olcp_hook_t olcp_hook);
/**
 * @brief 摧毁olcp句柄，释放内存
 * 
 * @param olcp_dec: olcp句柄
 * @retval 0: 
 */
int cias_olcp_destory(cias_olcp_t* olcp_dec);
/**
 * @brief olcp协议解析函数，解析完成会调用解析完成回调函数
 * 
 * @param olcp_dec: olcp句柄
 * @param data: 原始数据
 * @param len: 原始数据长度
 * @retval -1: error
 * @retval 0: successful
 */
int cias_olcp_decode(cias_olcp_t* olcp_dec, unsigned char *data, unsigned short len);

/****************************下面全部是协议编码的发送函数****************************/
		//只发送cmd
#define cias_olcp_send_cmd(olcp_dec, cmd)												\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD)
		//发送cmd和data段
#define cias_olcp_send_cmd_data(olcp_dec, cmd, data, data_len)							\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD_DATA, data, data_len)
		//发送cmd和data段，简单求和校验
#define cias_olcp_send_cmd_data_check_sum(olcp_dec, cmd, data, data_len)				\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD_DATA_CHECK, data, data_len, 1)
		//发送cmd和data段，crc8校验
#define cias_olcp_send_cmd_data_check_crc8(olcp_dec, cmd, data, data_len)				\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD_DATA_CHECK, data, data_len, 2) 
		
#if OLCP_ACK_RESEND
		//发送cmd和需要应答标志
#define cias_olcp_send_cmd_ack(olcp_dec, cmd)										\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD_ACK)
		//发送cmd、data和需要应答标志
#define cias_olcp_send_cmd_data_ack(olcp_dec, cmd, data, data_len)					\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD_DATA_ACK, data, data_len)
		//发送cmd、data和需要应答标志，简单求和校验
#define cias_olcp_send_cmd_data_check_sum_ack(olcp_dec, cmd, data, data_len)				\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD_DATA_CHECK_ACK, data, data_len, 1)
		//发送cmd、data和需要应答标志，crc8校验 
#define cias_olcp_send_cmd_data_check_crc8_ack(olcp_dec, cmd, data, data_len)				\
		cias_olcp_encode_after_send(olcp_dec, cmd, eCMD_DATA_CHECK_ACK, data, data_len, 2)
#endif

#ifdef __cplusplus
}
#endif
#endif
