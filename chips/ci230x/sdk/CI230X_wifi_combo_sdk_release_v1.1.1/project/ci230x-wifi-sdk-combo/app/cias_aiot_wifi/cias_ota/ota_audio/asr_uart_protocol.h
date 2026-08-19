#ifndef __ASR_UART_PROTOCOL_H__
#define __ASR_UART_PROTOCOL_H__


#define VMUP_MSG_DATA_MAX_SIZE (8)


/*header*/
#define VMUP_MSG_HEAD_LOW  (0xA5)
#define VMUP_MSG_HEAD_HIGH (0xFC)
#define VMUP_MSG_HEAD   ((VMUP_MSG_HEAD_HIGH<<8)|VMUP_MSG_HEAD_LOW)


/*tail*/
#define VMUP_MSG_TAIL   (0xFB)


/*msg_type*/
#define VMUP_MSG_TYPE_CMD_UP   (0xA0)
#define VMUP_MSG_TYPE_CMD_DOWN (0xA1)
#define VMUP_MSG_TYPE_ACK      (0xA2)
#define VMUP_MSG_TYPE_NOTIFY   (0xA3)


/*msg_cmd*/
#define VMUP_MSG_CMD_ASR_RESULT    (0x91)
#define VMUP_MSG_CMD_PLAY_VOICE    (0x92)
#define VMUP_MSG_CMD_GET_FLASHUID  (0x93)
#define VMUP_MSG_CMD_GET_VERSION   (0x94)
#define VMUP_MSG_CMD_RESET_MODULE  (0x95)
#define VMUP_MSG_CMD_SET_CONFIG    (0x96)
#define VMUP_MSG_CMD_ENTER_OTA_MODE (0x97)
#define VMUP_MSG_CMD_NOTIFY_STATUS (0x9A)
#define VMUP_MSG_CMD_ACK_COMMON    (0xAA)
/* !!! if user want add please add form VMUP_MSG_CMD_USER_START*/
#define VMUP_MSG_CMD_USER_START    (0xB0)

// typedef unsigned short  unsigned short;
// typedef unsigned char   unsigned char;

#pragma pack(1)
typedef struct
{
    unsigned short header;
    unsigned short data_length;
    unsigned char msg_type;
    unsigned char msg_cmd;
    unsigned char msg_seq;
    unsigned char msg_data[VMUP_MSG_DATA_MAX_SIZE];
    /*unsigned short chksum; send add auto*/
    /*unsigned char tail; send add auto*/
}sys_msg_com_data_t;
#pragma pack()

int vmup_enter_ota_mode(int baudrate);

int vmup_reset_device();

#endif

