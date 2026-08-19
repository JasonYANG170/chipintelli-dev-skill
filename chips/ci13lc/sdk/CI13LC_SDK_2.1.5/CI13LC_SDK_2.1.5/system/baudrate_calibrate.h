
#ifndef __BAUDRATE_CALIBRATE_H__
#define __BAUDRATE_CALIBRATE_H__


#include "sdk_default_config.h"
#include "ci_system.h"



// 执行一次波特率校准
void baudrate_calibrate(UART_TypeDef *UARTx);

#if BAUDRATE_SCAN_EN
typedef void(*send_sync_req_func_t)(void);

// 波特率校准模块初始化函数
void baudrate_calibrate_init(UART_TypeDef *UARTx, uint32_t baudrate, send_sync_req_func_t send_sync_req_func);

// 启动一次波特率校准
void baudrate_calibrate_start(void);

// 如果正在进行波特率校准，停止这次波特率校准，下一次自动恢复
void baudrate_calibrate_stop(void);

// 如果正在进行波特率校准，等到校准结束再返回
void baudrate_calibrate_wait_finish(void);

// 语音协议模块收到同步ACK的时候，通过调用此接口告知已收到ACK。
void baudrate_calibrate_set_ack();

//设置波特率校准任务结束的标志
void set_baudrate_cali_close_flag();

//获取波特率校准任务是否结束的标志
uint8_t get_baudrate_cali_close_flag();
#endif

#endif /* UART_BAUDRATE_CALIBRATE */

