#include "cias_ir_common.h"

OPERATE_RET cias_local_send_ir(IN CHAR_T *key, IN CHAR_T *head)
{
    uint32_t i = 0;
    TY_IR_CODE_S *ir_code;
    //解压缩
    tuya_ir_lib2timecode(head, key, &ir_code);
    uint8_t *ir_code_buf = cias_malloc(ir_code->code_len * 2);
    if (ir_code_buf)
    {
        int m_ir_data = 0;
        for (i = 0; i < ir_code->code_len; i++)
        {
            //PR_NOTICE("irc:%d", *(ir_code->code + i));
            m_ir_data = (*(ir_code->code + i)) / 2;
            ir_code_buf[2 * i] = (m_ir_data >> 8) & 0xff;
            ir_code_buf[2 * i + 1] = m_ir_data & 0xff;
        }
        PR_NOTICE(" ir_code->code_len = %d\r\n", ir_code->code_len);
        //发送红外发送时间码到AUIDO
        cias_message_send_interface(CIAS_IR_DATA, DEF_FILL, ir_code->code_len * 2, ir_code_buf);
        if (ir_code_buf)
        {
            cias_free(ir_code_buf);
        }
    }
    else
    {
        PR_ERR("ir_code_temp or ir_code_buf malloc error\r\n");
    }
    if (ir_code)
        tuya_ir_code_free(ir_code, 1);
}
