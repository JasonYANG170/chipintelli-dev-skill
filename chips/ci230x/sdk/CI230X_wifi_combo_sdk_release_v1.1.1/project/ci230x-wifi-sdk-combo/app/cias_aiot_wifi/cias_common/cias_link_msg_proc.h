/**
 * @file    cias_hal_uart.h
 * @author  zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_LINK_MSG_PROC_H
#define _CIAS_LINK_MSG_PROC_H

#ifdef __cplusplus
 extern "C" {
#endif

enum
{ 
	COMHEAD = 0xA5, 			// 包头
	COMTAIL = 0xA4 				//包尾
};

enum 
{ 
	COM_ANA_IDLE = 0, 		
	COM_ANA_HEAD = 1,				
	COM_ANA_HH = 2, 				
	COM_ANA_TT = 3, 
	COM_ANA_NEXT_H = 4 
};


typedef struct de_code_info_struct
{
	unsigned char status;  //解码状态机
	unsigned int bcc;    //校验
	unsigned int in_len;   //长度
}de_code_info;

enum
{
	LINK_DECODE_RET_ERROR = -1,
	LINK_DECODE_RET_NULL = 0,
	
};

unsigned int cias_link_msg_code(unsigned char * app_buffer,unsigned short app_size,
					unsigned char * link_buffer,unsigned short link_max_size) ;

			
unsigned int cias_link_msg_decode(unsigned char * uc_data,
					unsigned char * link_buffer,unsigned short link_max_size,
					de_code_info * de_code_info);

#ifdef __cplusplus
}
#endif

#endif
					
