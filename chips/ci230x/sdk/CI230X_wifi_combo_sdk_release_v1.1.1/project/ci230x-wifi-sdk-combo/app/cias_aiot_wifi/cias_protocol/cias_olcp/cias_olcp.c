/*
 * @Author: JasonChan
 * @Date: 2020-08-19 18:34:32
 * @LastEditTime: 2022-03-19 15:10:15
 * @Description: 串口等8bit数据协议解析
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cias_log.h"
#include "cias_crc.h"
#include "cias_olcp.h"
#include "cias_olcp_export.h"
#include "cias_freertos_common.h"
#include "cias_common.h"

#ifdef __cplusplus
    extern "C" {
#endif
/**
 * @brief 创建olcp通信句柄，申请内存
 * 
 * @param hal_send: 硬件发送函数
 * @param olcp_hook: 解析完成回调钩子
 * @retval NULL: error
 * @retval !NULL: successful
 */
cias_olcp_t* cias_olcp_create(olcp_send_t hal_send, olcp_hook_t olcp_hook)
{
    cias_olcp_t* olcp_dec = NULL;

    CIAS_ASSERT_GOFAIL(hal_send!=NULL);
    CIAS_ASSERT_GOFAIL(olcp_hook!=NULL);

    olcp_dec = (cias_olcp_t*)cias_malloc(sizeof(cias_olcp_t));
    CIAS_ASSERT_GOFAIL(olcp_dec!=NULL);
    memset(olcp_dec, 0, sizeof(cias_olcp_t));
    olcp_dec->data = (unsigned char*)cias_malloc(OLCP_RECV_DATA_LMT);
    CIAS_ASSERT_GOFAIL(olcp_dec->data!=NULL);
    olcp_dec->send_buff = (unsigned char*)cias_malloc(OLCP_HEAD_LEN+OLCP_SEND_DATA_LMT+1);
    CIAS_ASSERT_GOFAIL(olcp_dec->send_buff!=NULL);

    olcp_dec->hal_send = hal_send;
    olcp_dec->olcp_hook = olcp_hook;
    olcp_dec->p_write = NULL;
    olcp_dec->state = eOLCP_HEAD_0;

    return olcp_dec;
__fail:
    cias_olcp_destory(olcp_dec);
    return NULL;
}
/**
 * @brief 摧毁olcp句柄，释放内存
 * 
 * @param olcp_dec: olcp句柄
 * @retval 0: 
 */
int cias_olcp_destory(cias_olcp_t* olcp_dec)
{
    if (olcp_dec!=NULL)
    {
        cias_free(olcp_dec->data);
        cias_free(olcp_dec->send_buff);
        cias_free(olcp_dec);
    }

    return 0;
}
/**
 * @brief olcp协议解析函数，解析完成会调用解析完成回调函数
 * 
 * @param olcp_dec: olcp句柄
 * @param data: 原始数据
 * @param len: 原始数据长度
 * @retval -1: error
 * @retval 0: successful
 */
int cias_olcp_decode(cias_olcp_t* olcp_dec, unsigned char *data, unsigned short len)
{
    unsigned short i, j, export_data_len;
    unsigned char  *p_data = NULL, *export_data = NULL;
    unsigned short remaining_recv_data_len;
    unsigned short remaining_data_buff_len;

    CIAS_ASSERT_RETURN(olcp_dec!=NULL);
    CIAS_ASSERT_RETURN(olcp_dec->data!=NULL);
    CIAS_ASSERT_RETURN(data!=NULL);
    
    p_data = data;
    for (i = 0; i < len; i++, p_data++)
    {
        switch (olcp_dec->state)
        {
            case eOLCP_HEAD_0: 
                if (*p_data == OLCP_FRAME_HEAD_0)
                    olcp_dec->state = eOLCP_HEAD_1;
                break;
            case eOLCP_HEAD_1:
                if (*p_data == OLCP_FRAME_HEAD_1)
                {
                    olcp_dec->p_write = &(olcp_dec->check_head);
                    olcp_dec->state = eOLCP_DATA_HEAD;
                }
                else if(*p_data == OLCP_FRAME_HEAD_0)
                    olcp_dec->state = eOLCP_HEAD_1;
                else
                    olcp_dec->state = eOLCP_HEAD_0;
                break;
            case eOLCP_DATA_HEAD:
                *(olcp_dec->p_write) = *p_data;
                if(olcp_dec->p_write-&(olcp_dec->check_head) >= (OLCP_HEAD_LEN-4)) 
                    olcp_dec->state = eOLCP_CHECK_HEAD;
                olcp_dec->p_write++;
                break;
            case eOLCP_CHECK_HEAD:
                *(olcp_dec->p_write) = *p_data;
                if (olcp_dec->check_head == crc8_func(0, &(olcp_dec->flags), OLCP_HEAD_LEN-3)) //数据头校验通过
                {  
                    if (olcp_dec->data_length == 0) //只有cmd字段，数据段为空，直接执行回调
                    {
                        export_data = NULL; 
                        export_data_len = 0;
                        goto __check_successful;
                    }
                    else if(olcp_dec->data_length > OLCP_RECV_DATA_LMT)  //接受数据长度大于限制长度
                    {
                        CIAS_LOG_ERR("[OLCP] Recv data len is too long!!!");
                        goto __exit_check_statue;
                    }
                    else
                    {
                        olcp_dec->p_write = olcp_dec->data;
                        olcp_dec->state = eOLCP_DATA;
                        break;
                    }
                }
                else
                    goto __exit_check_statue;
                break;
            case eOLCP_DATA:
                remaining_recv_data_len = len-i;
                remaining_data_buff_len = olcp_dec->data_length-(olcp_dec->p_write-olcp_dec->data);
                if (remaining_recv_data_len < remaining_data_buff_len)
                {   //如果这次剩余的数据小于remaining_data_buff_len，将剩余数据全部拷贝到解码结构体里
                    memcpy(olcp_dec->p_write, p_data, remaining_recv_data_len);
                    olcp_dec->p_write += remaining_recv_data_len;
                    return 0;
                }
                else
                {
                    memcpy(olcp_dec->p_write, p_data, remaining_data_buff_len);
                    p_data += remaining_data_buff_len;
                    i += remaining_data_buff_len;
                    i--; p_data--;//与循环执行的一次++抵消。
                    olcp_dec->p_write = NULL;
                    olcp_dec->state = eOLCP_CHECK_DATA;
                }
                break;
            case eOLCP_CHECK_DATA: 
                if((olcp_dec->flags)&CHECK_SUM_MASK)
                {
                    olcp_dec->check_data = 0;
                    olcp_dec->p_write = olcp_dec->data;
                    for (j = 0; j < olcp_dec->data_length; j++)
                    { 
                        olcp_dec->check_data += *(olcp_dec->p_write);
                        (olcp_dec->p_write)++;
                    }
                    if (olcp_dec->check_data != (*p_data)) //校验不通过
                        goto __exit_check_statue;
                }
                else if((olcp_dec->flags)&CHECK_CRC_MASK)
                {
                    olcp_dec->check_data = crc8_func(0, olcp_dec->data, olcp_dec->data_length);
                    if (olcp_dec->check_data != (*p_data)) //校验不通过
                        goto __exit_check_statue;
                }
                export_data = olcp_dec->data; 
                export_data_len = olcp_dec->data_length;
__check_successful:
                olcp_dec->state = eOLCP_HEAD_0;
                #if OLCP_ACK_RESEND
                //如果是应答的ack帧，则不执行应答和回调函数
                if((olcp_dec->flags)&RECV_ACK_MASK)
                {
                    olcp_dec->recv_ack = 1;
                    olcp_dec->recv_ack_cmd = olcp_dec->cmd;
                    break;
                }
                #endif
                //解析完毕，如果需要应答，发送应答命令
                if((olcp_dec->flags)&IS_NEED_ACK_MASK)
                    cias_olcp_encode_after_send(olcp_dec, olcp_dec->recv_count, eCMD_RESPONSE_ACK);  //应答帧的cmd字段由recv_count替换
                //解析完毕，执行回调函数
                if (olcp_dec->olcp_hook != NULL)
                    olcp_dec->olcp_hook(olcp_dec->recv_count, olcp_dec->cmd, export_data, export_data_len);
                break;
__exit_check_statue:
                if (olcp_dec->state == eOLCP_CHECK_DATA) 
                {   //如果数据段校验没有通过 则从数据段开始重新解析
                    olcp_dec->state = eOLCP_HEAD_0;
                    cias_olcp_decode(olcp_dec, olcp_dec->data, olcp_dec->data_length);
                }
                else if(olcp_dec->state == eOLCP_CHECK_HEAD)
                {   //如果帧头段校验没有通过 则从check_head开始重新解析
                    olcp_dec->state = eOLCP_HEAD_0;
                    cias_olcp_decode(olcp_dec, &(olcp_dec->check_head), OLCP_HEAD_LEN-2);
                }
                break;
            default:
                olcp_dec->state = eOLCP_HEAD_0;
                break;
        }
    }

    return 0;
}

/**
 * @brief olcp编码发送函数，进行协议的编码然后通过hal_send函数发送出去.
 * 
 * @param olcp_dec: olcp句柄
 * @param cmd: 命令字段
 * @param send_choose: 选择不同的类型发送
 * @param ...:  data: 数据(缺省值为NULL)
 *              data_len: 数据长度(缺省值为0)
 *              flags: 标志位(缺省值为0)
 * @retval -1: error
 * @retval 0: successful
 */
int cias_olcp_encode_after_send(cias_olcp_t* olcp_dec, unsigned short cmd, cias_olcp_send_func_e send_choose, ...)
{
    va_list valist;
    int i, j;
    cias_olcp_t tmp_dec;

    CIAS_ASSERT_RETURN(olcp_dec!=NULL);
    CIAS_ASSERT_RETURN(olcp_dec->hal_send!=NULL);
    CIAS_ASSERT_RETURN(olcp_dec->send_buff!=NULL);

    tmp_dec.data = NULL;
    tmp_dec.data_length = 0;
    tmp_dec.flags = 0;
    va_start(valist, send_choose);
    switch (send_choose)
    {
        case eCMD: //只发送cmd
            break;
        case eCMD_DATA: //发送cmd和data段
            tmp_dec.data = va_arg(valist, unsigned char*);
            tmp_dec.data_length = va_arg(valist, int);
            break;
        case eCMD_RESPONSE_ACK:
            tmp_dec.flags |= RECV_ACK_MASK;
            break;
        case eCMD_DATA_CHECK: //发送cmd和data段，需要校验
            tmp_dec.data = va_arg(valist, unsigned char*);
            tmp_dec.data_length = va_arg(valist, int);
            tmp_dec.flags |= (va_arg(valist, int)&(CHECK_SUM_MASK|CHECK_CRC_MASK));
            break;
#if OLCP_ACK_RESEND
        case eCMD_ACK: //发送cmd和需要应答标志
            tmp_dec.flags |= IS_NEED_ACK_MASK;
            break;
        case eCMD_DATA_ACK: //发送cmd、data和需要应答标志
            tmp_dec.data = va_arg(valist, unsigned char*);
            tmp_dec.data_length = va_arg(valist, int);
            tmp_dec.flags |= IS_NEED_ACK_MASK;
            break;
        case eCMD_DATA_CHECK_ACK: //发送cmd、data和需要应答标志，需要校验
            tmp_dec.data = va_arg(valist, unsigned char*);
            tmp_dec.data_length = va_arg(valist, int);
            tmp_dec.flags |= (va_arg(valist, int)&(CHECK_SUM_MASK|CHECK_CRC_MASK));
            tmp_dec.flags |= IS_NEED_ACK_MASK;
            break;
#endif
        default:
            break;
    }
    va_end(valist);
    CIAS_ASSERT_RETURN(tmp_dec.data_length<=OLCP_SEND_DATA_LMT);
    olcp_dec->send_count++;
    tmp_dec.recv_count = olcp_dec->send_count;
    tmp_dec.cmd = cmd;

    tmp_dec.p_write = olcp_dec->send_buff;
    *(tmp_dec.p_write) = OLCP_FRAME_HEAD_0; tmp_dec.p_write++;
    *(tmp_dec.p_write) = OLCP_FRAME_HEAD_1; tmp_dec.p_write++;
    *(tmp_dec.p_write) = crc8_func(0, &(tmp_dec.flags), OLCP_HEAD_LEN-3); tmp_dec.p_write++;
    memcpy(tmp_dec.p_write, &(tmp_dec.flags), OLCP_HEAD_LEN-3); tmp_dec.p_write+=(OLCP_HEAD_LEN-3);
    tmp_dec.check_data = 0;
    if (tmp_dec.flags&CHECK_SUM_MASK)
    {
        tmp_dec.p_write = tmp_dec.data;
        for (i = 0; i < tmp_dec.data_length; i++)
        {
            tmp_dec.check_data += *(tmp_dec.p_write);
            tmp_dec.p_write++;
        }
    }
    else if(tmp_dec.flags&CHECK_CRC_MASK)
    {
        tmp_dec.check_data = crc8_func(0, tmp_dec.data, tmp_dec.data_length);
    }
    if (tmp_dec.data_length>0)
    {
        memcpy(olcp_dec->send_buff+OLCP_HEAD_LEN, tmp_dec.data, tmp_dec.data_length);
        olcp_dec->send_buff[OLCP_HEAD_LEN+tmp_dec.data_length] = tmp_dec.check_data;
        tmp_dec.data_length++;
    }
    //用来记录发送数据大小
    tmp_dec.data_length += OLCP_HEAD_LEN;

    #if OLCP_ACK_RESEND
    if(tmp_dec.flags&IS_NEED_ACK_MASK)
    {
        i = 0;
        olcp_dec->recv_ack = 0; //清空recv_ack位
    }
    #endif
__olcp_hal_send:
    i++;
    olcp_dec->hal_send(olcp_dec->send_buff, tmp_dec.data_length);

    #if OLCP_ACK_RESEND
    //如果需要ack，一直没有收到ack，将会间隔OLCP_ACK_RESEND_T(ms)重发OLCP_ACK_RESEND_MAX_NUM次帧
    if ((tmp_dec.flags&IS_NEED_ACK_MASK) && (i < OLCP_ACK_RESEND_MAX_NUM))
    {
        for (j = 0; j < OLCP_ACK_RESEND_T; j++)
        {
            if (olcp_dec->recv_ack && (olcp_dec->recv_ack_cmd == tmp_dec.recv_count)) //判断收到的应答段的cmd是否和发送count相等
                return 0;
            cias_task_delay_ms(1);
        }
        CIAS_LOG_WARN("[OLCP] Resend!!");
        goto __olcp_hal_send; //再发送一次
    }
    if (tmp_dec.flags&IS_NEED_ACK_MASK) //重发OLCP_ACK_RESEND_MAX_NUM后仍然没有收到ack
    {
        CIAS_LOG_ERR("[OLCP] Not recv ack!!!");
        return -1;
    }
    #endif

    return 0;
}

#ifdef __cplusplus
}
#endif

