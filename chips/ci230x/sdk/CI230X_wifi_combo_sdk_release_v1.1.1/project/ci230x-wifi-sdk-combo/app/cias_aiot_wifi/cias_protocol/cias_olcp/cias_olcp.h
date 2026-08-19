#ifndef _CIAS_OLCP_H_
#define _CIAS_OLCP_H_

#ifdef __cplusplus
    extern "C" {
#endif
/**
 * OLCP协议帧头
*/
#define OLCP_FRAME_HEAD_0    0xAA    
#define OLCP_FRAME_HEAD_1    0x0F
/**
 * flags位的mask
*/
#define CHECK_SUM_MASK       0x01 //第一位
#define CHECK_CRC_MASK       0x02 //第二位
#define IS_NEED_ACK_MASK     0x04 //第三位
#define RECV_ACK_MASK        0x08 //第四位
/**
 * typedef
*/
typedef void (*olcp_send_t) (unsigned char* data, unsigned int len);
typedef void (*olcp_hook_t) (unsigned short recv_count, unsigned short cmd, unsigned char* data, unsigned short len);
/**
 * olcp帧头长度(包括 头、check_head、flags、recv_count、cmd、data_length)
*/
#define OLCP_HEAD_LEN        10
/**
 * 枚举解码状态
*/
typedef enum
{
    eOLCP_HEAD_0 = 0,     //第一个帧头状态
    eOLCP_HEAD_1,         //第二个帧头状态
    eOLCP_DATA_HEAD,      //接受数据头状态
    eOLCP_CHECK_HEAD,     //计算头校验位状态
    eOLCP_DATA,           //接受数据状态
    eOLCP_CHECK_DATA      //计算数据校验状态
}cias_olcp_state_e;
/**
 * 编码发送函数类型
*/
typedef enum
{
    eCMD = 0,              //只发送cmd
    eCMD_ACK,              //发送cmd和需要应答标志
    eCMD_RESPONSE_ACK,     //发送应答ack
    eCMD_DATA,             //发送cmd和data段
    eCMD_DATA_ACK,         //发送cmd、data和需要应答标志
    eCMD_DATA_CHECK,       //发送cmd和data段，需要校验
    eCMD_DATA_CHECK_ACK    //发送cmd、data和需要应答标志，需要校验
}cias_olcp_send_func_e;
/**
 * 解码数据状态和储存解析数据结构体
*/
#pragma pack(1) //对齐数值为一个字节
struct cias_olcp_def
{
    /**帧的数据头*/
    unsigned char           check_head;     //头校验位
    /*标志位。   低两位表示是否需要校验(10:8位CRC校验、01:简单求和校验、00:关闭校验)。
                第三位表示是否需要ack(0:不需要，1:需要)
                第四位表示接收ack位*/
    unsigned char           flags;     
    unsigned short          recv_count;     //接收帧计数变量
    unsigned short          cmd;            //命令字段
    unsigned short          data_length;    //数据字段长度
    /**帧的数据段*/
    unsigned char           *data;          //数据
    unsigned char           *p_write;       //指向数据位置
    /**帧的解析状态*/
    cias_olcp_state_e       state;          //解析状态
    unsigned short          send_count;     //发送帧计数变量
    unsigned char           *send_buff;     //发送buff
    /**校验相关*/
    unsigned char           check_data;     //数据校验位 
    /**ack相关*/
    unsigned char           recv_ack;       //接收ack标志
    unsigned short          recv_ack_cmd;   //接收ack命令
    /**回调发送函数*/
    olcp_send_t             hal_send;
    olcp_hook_t             olcp_hook;
};
#pragma pack() //取消指定对齐，恢复缺省对齐
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
int cias_olcp_encode_after_send(struct cias_olcp_def* olcp_dec, unsigned short cmd, cias_olcp_send_func_e send_choose, ...);

#ifdef __cplusplus
}
#endif
#endif
