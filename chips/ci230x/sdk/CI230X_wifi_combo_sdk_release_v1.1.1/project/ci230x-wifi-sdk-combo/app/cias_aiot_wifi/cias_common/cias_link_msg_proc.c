/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-07-28 09:26:20
 * @LastEditTime: 2022-08-05 10:17:05
 * @Description: 
 */
#include "cias_link_msg_proc.h"
#include <string.h>

#define MAX_PACK_SIZE (2048)

unsigned int cias_link_msg_code(unsigned char *source_buffer, unsigned short source_size,
                                unsigned char *des_buffer, unsigned short des_max_size)
{
    unsigned int i = 0;
    unsigned int b_result = 1;
    unsigned int dw_tail_len = 0;
    unsigned int size = 0;
    unsigned int dw_out_len = 0;
    unsigned int uc_bcc = 0;
    unsigned int uc_temp;
    if (source_buffer == 0 || source_size == 0 || des_buffer == 0 || des_max_size == 0)
    {
        return 0;
    }
    memset(des_buffer, 0, des_max_size);
    i = 0;
    b_result = 1;
    dw_tail_len = 0;
    size = 0;
    dw_out_len = 0;
    uc_bcc = 0;

    if (source_size < des_max_size)
    {
        uc_bcc = 0;

        des_buffer[0] = COMHEAD; //A5

        dw_out_len++;
        for (i = 0; i < source_size; i++)
        {
            uc_temp = (unsigned char)source_buffer[i];

            uc_bcc ^= uc_temp;
            des_buffer[dw_out_len] = uc_temp;
            dw_out_len++;
            if ((dw_out_len + 1) >= MAX_PACK_SIZE)
            {
                break;
            }
            else if ((uc_temp == COMHEAD) || (uc_temp == COMTAIL))
            {
                des_buffer[dw_out_len] = uc_temp;
                dw_out_len++;
                if ((dw_out_len + 1) >= MAX_PACK_SIZE)
                {
                    break;
                }
            }
        }

        if ((uc_bcc == COMHEAD) || (uc_bcc == COMTAIL))
        {
            dw_tail_len = 1;
        }

        if ((dw_out_len + 3 + dw_tail_len) < MAX_PACK_SIZE)
        {
            b_result = 0;

            des_buffer[dw_out_len] = uc_bcc;
            dw_out_len++;
            if ((uc_bcc == COMHEAD) || (uc_bcc == COMTAIL))
            {
                des_buffer[dw_out_len] = uc_bcc;
                dw_out_len++;
            }
            des_buffer[dw_out_len] = COMTAIL;
            dw_out_len++;
            des_buffer[dw_out_len] = 0x00;
            dw_out_len++;
        }
    }
    return dw_out_len;
}

unsigned int cias_link_msg_decode(unsigned char *uc_data,
                                  unsigned char *msg_buffer, unsigned short buffer_max_size,
                                  de_code_info *de_code_info)
{
    unsigned int de_code_ret = LINK_DECODE_RET_NULL;
    unsigned char *status = 0;
    unsigned int *bcc = 0;
    unsigned int *in_len = 0;
    if (buffer_max_size == 0 || msg_buffer == 0 || de_code_info == 0)
    {
        return LINK_DECODE_RET_ERROR;
    }

    status = &(de_code_info->status);
    bcc = &(de_code_info->bcc);
    in_len = &(de_code_info->in_len);

    switch (*status)
    {

    case COM_ANA_IDLE:
        if (COMHEAD == *uc_data)
        {
            *status = COM_ANA_HEAD;
            *bcc = 0;
            *in_len = 1;
            msg_buffer[0] = COMHEAD;
        }
        break;
    case COM_ANA_NEXT_H:
        *status = COM_ANA_HEAD;
        *bcc = 0;
        *in_len = 1;
        msg_buffer[0] = COMHEAD;
    case COM_ANA_HEAD:
        if (COMHEAD == *uc_data)
        {
            *status = COM_ANA_HH;
        }
        else if (COMTAIL == *uc_data)
        {
            *status = COM_ANA_TT;
        }
        else
        {
            msg_buffer[*in_len] = *uc_data;
            (*in_len)++;
            *bcc ^= *uc_data;
        }
        break;

    case COM_ANA_HH:
        if (COMHEAD == *uc_data)
        {
            msg_buffer[*in_len] = *uc_data;
            (*in_len)++;
            *bcc ^= *uc_data;
            *status = COM_ANA_HEAD;
        }
        else if (COMTAIL == *uc_data)
        {
            *status = COM_ANA_TT;
        }
        else
        {
            *status = COM_ANA_HEAD;
        }
        break;
    case COM_ANA_TT:
        if (COMTAIL == *uc_data)
        {
            msg_buffer[*in_len] = *uc_data;
            (*in_len)++;
            *bcc ^= *uc_data;
            *status = COM_ANA_HEAD;
        }
        else if ((0x00 == *uc_data) || (COMHEAD == *uc_data))
        {
            if (*bcc == 0)
            {
                if (COMHEAD == *uc_data)
                {
                    msg_buffer[*in_len] = COMTAIL;
                    (*in_len)++;
                    de_code_ret = *in_len;
                }
                else
                {
                    msg_buffer[*in_len] = COMTAIL;
                    (*in_len)++;
                    msg_buffer[*in_len] = 0x00;
                    (*in_len)++;
                    de_code_ret = *in_len;
                }
            }
            else
            {
                //校验不过，丢包了
                de_code_ret = LINK_DECODE_RET_ERROR;
            }
            if (COMHEAD == *uc_data)
            {
                *status = COM_ANA_NEXT_H;
            }
            else
            {
                *status = COM_ANA_IDLE;
            }
        }
        break;
    default:
        break;
    }

    if (*in_len >= buffer_max_size)
    {
        de_code_ret = LINK_DECODE_RET_ERROR;
        *status = COM_ANA_IDLE;
        *in_len = 0;
    }
    return de_code_ret;
}
