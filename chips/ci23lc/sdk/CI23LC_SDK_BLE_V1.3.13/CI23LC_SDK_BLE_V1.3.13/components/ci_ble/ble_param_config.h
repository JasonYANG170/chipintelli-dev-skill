/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-04-25 11:06:33
 * @LastEditTime: 2023-05-12 18:10:37
 * @Description: 
 */
#ifndef __BLE_PARAM_CONFIG_H__
#define __BLE_PARAM_CONFIG_H__

#include "user_config.h"
#include "sdk_default_config.h"

#define BLE_PATCH_CRC                      0x84EE       //蓝牙模块固件crc校验值-不可修改
#define BLE_FIRMWARE_ID                    7000         //蓝牙固件ID，放在user_file文件目录-不可修改
#define BLE_ADV_NAME_MAX_LEN               29           //蓝牙名称最大字节数，最大只支持29字节-不可修改

#define CIAS_BLE_SCAN_ENABLE               0            //开启蓝牙蓝模块搜索2.4G遥控器广播
#if CIAS_BLE_SCAN_ENABLE
#define CIAS_BLE_SCAN_REMOTE_CONTROL_LEN   0x16         //ble广播扫描遥控器数据的长度,公版0x16
#endif
#define CIAS_BLE_HEART_TIMER_ENABLE        1            //查询蓝牙状态心跳

#if BLE_PAIRING_ENBLE
#define BLE_PAIRING_MODE                    BLE_PAIRING_SECURE_CONNECT_JUSTWORK
#define PAIRING_DATA_LEN                    170
#define FLASH_PAIRING_DATA_ADDR             0x1FE000    //配对成功后存储的秘钥的FLASH地址
#define NVDATA_ID_PAIRING_DATA              0x70000002  //配对成功后存储的秘钥的NV ID
#endif

#define TIMEOUT_ONE_PACKET_INTERVAL         1000
#define BLE_MSG_DATA_MAX_SIZE               240          //蓝牙连接状态收发消息的最大长度
#define BLE_RCV_MSG_QUEUE_SIZE              5            //蓝牙接收数据队列大小
#define RF_RX_TX_MAX_LEN                    20           //设备端回复手机的蓝牙消息长度
#define BLE_ADV_LEN                         42           //蓝牙广播包长度
#define BLE_SEND_PAYLOAD_DELAY_20           20           //设备发送字节数小于等于BLE_MSG_DATA_MAX_SIZE/4的蓝牙数据到手机后的延时时间(ms)
#define BLE_SEND_PAYLOAD_DELAY_50           50           //设备发送字节数小于等于BLE_MSG_DATA_MAX_SIZE/2的蓝牙数据到手机后的延时时间(ms)
#define BLE_SEND_PAYLOAD_DELAY_100          100          //设备发送字节数大于BLE_MSG_DATA_MAX_SIZE/2的蓝牙数据到手机后的延时时间(ms)


//io相关配置-不可修改
#define BLE_DEFAULT_BAUDRATE                115200         //mcu和蓝牙模块通信串口波特率
#define BLE_PROTOCOL_NUMBER                 HAL_UART1_BASE //mcu和蓝牙模块通信串口
#define BLE_PROTOCOL_IRQ_NUMBER             UART1_IRQn     //mcu和蓝牙模块通信串口中断号


#define BLE_RESET_PIN                       PA6            // 复位引脚选择
#define BLE_RESET_GPIO_PORT                 PA             // 复位引脚GPIO口选择 
#define BLE_RESET_GPIO_PIN                  pin_6          // 复位引脚GPIO pin 号选择 
#define BLE_RESET_PIN_REUSE                 FIRST_FUNCTION // 复位引脚复用功能选择

#define DEV_MTU_TYPE                        0x02           //3.5代蓝牙设备，可以收发长包

#endif   //__BLE_PARAM_CONFIG_H__