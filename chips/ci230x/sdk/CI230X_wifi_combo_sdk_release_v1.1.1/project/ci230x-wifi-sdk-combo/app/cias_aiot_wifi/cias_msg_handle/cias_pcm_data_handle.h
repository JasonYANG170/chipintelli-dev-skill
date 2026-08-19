/*
 * @Description: PCM原始数据处理，
 * @Author: hongchuan.wu
 * @Date: 2021-12-02 13:34:02
 * @LastEditTime: 2021-12-16 16:11:16
 * @LastEditors: hongchuan.wu
 * @Reference: 
 */



#ifndef __CIAS_PCM_DATA_DEAL_H__
#define __CIAS_PCM_DATA_DEAL_H__
#include <FreeRTOS.h>


typedef struct    //原始数据缓冲区
{
    unsigned char* pcm_data_read;
    unsigned char* pcm_data_write;
    unsigned char* pcm_data_head;   //unsigned char* pcm_data_head;
    unsigned char* pcm_data_tail;
    uint32_t pcm_data_writ_count;  //write    
    uint32_t pcm_data_read_count;  //read 指针越过pcm_data_tail的次数
 
}pam_data_buff_t;

int32_t cias_send_pcm_finish(cias_raw_speech_t send_msg_speech,cias_standard_head_t *phead,uint8_t *msg_buf);
int32_t cias_send_pcm_middle(cias_raw_speech_t send_msg_speech,cias_standard_head_t *phead,uint8_t *msg_buf);
int32_t get_free_space(pam_data_buff_t *p);
int32_t get_used_space(pam_data_buff_t *p);
uint32_t write_pcm_data(unsigned char*date,uint32_t len);
void pcm_data_deal(void);



#endif //__CIAS_PCM_DATA_DEAL_H__