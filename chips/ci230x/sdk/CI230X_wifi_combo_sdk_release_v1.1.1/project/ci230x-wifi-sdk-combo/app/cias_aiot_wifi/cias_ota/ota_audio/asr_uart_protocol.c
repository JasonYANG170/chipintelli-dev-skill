
#include <string.h>
#include "asr_uart_protocol.h"
#include "cias_communication.h"

static unsigned short vmup_port_checksum(unsigned short init_val, unsigned char * data, unsigned short length)
{
    int i;
    unsigned short chk_sum = init_val;
    for (i = 0; i < length; i++)
    {
        chk_sum += data[i];
    }

    return chk_sum;
}


/********************************************************************
                     send function
********************************************************************/
static int vmup_send_packet(sys_msg_com_data_t * msg)
{
    unsigned short chk_sum;
    unsigned char *buf = (unsigned char*)msg;

    if (!msg)
    {
        return 1;
    }
    
    /*check sum*/
    chk_sum = vmup_port_checksum(0, (unsigned char*)&msg->msg_type, 3);
    chk_sum = vmup_port_checksum(chk_sum, msg->msg_data, msg->data_length);
    //*(unsigned short*)(msg->msg_data + msg->data_length) = chk_sum;
    *(unsigned char*)(msg->msg_data + msg->data_length) = (unsigned char)(chk_sum & 0x00FF);
    *(unsigned char*)(msg->msg_data + msg->data_length + 1) = (unsigned char)((chk_sum >> 8) & 0x00FF);
    *(unsigned char*)(msg->msg_data + msg->data_length + 2) = VMUP_MSG_TAIL;
    // hw_port_io_send(buf, msg->data_length + 10);
    communication_send(buf, msg->data_length + 10);

    return 0;
}

int vmup_enter_ota_mode(int baudrate)
{
    sys_msg_com_data_t msg;
    msg.header = VMUP_MSG_HEAD;
    msg.data_length = 4;
    msg.msg_type = VMUP_MSG_TYPE_CMD_DOWN;
    msg.msg_cmd = VMUP_MSG_CMD_ENTER_OTA_MODE;
    msg.msg_seq = 0;
    *(unsigned long*)msg.msg_data = baudrate;
    vmup_send_packet(&msg);
    return 0;
}


int vmup_reset_device()
{
    sys_msg_com_data_t msg;

    msg.header = VMUP_MSG_HEAD;
    msg.data_length = 5;
    msg.msg_type = VMUP_MSG_TYPE_CMD_DOWN;
    msg.msg_cmd = VMUP_MSG_CMD_RESET_MODULE;
    msg.msg_seq = 0;
    memcpy((char*)(msg.msg_data), "reset", 5);
    vmup_send_packet(&msg);
    return 0;
}

