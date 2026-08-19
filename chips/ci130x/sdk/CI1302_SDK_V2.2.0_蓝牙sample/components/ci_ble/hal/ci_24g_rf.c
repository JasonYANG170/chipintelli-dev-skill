/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-04-18 09:58:11
 * @LastEditTime: 2023-05-10 11:18:39
 * @Description:    
 */
#include "ci_ble_rf.h"
#include "ci130x_gpio.h"
#include "ci_log.h"
#include "FreeRTOS.h"
#include "ci130x_timer.h"
#include "user_config.h"
#include "exe_api.h"
#include "rf_msg_deal.h"

extern rf_cb_funcs_t rf_cb_funcs;
void HS6220_Init()
{
    unsigned char temp[5];
	HS6220_SPI_Init();
    vTaskDelay(pdMS_TO_TICKS(10));
	HS6220_write_byte(HS6220_BANK0_FEATURE, SOFT_RST); // soft_reset
#if (HS6220_SPI_NWIRE == SPI_4_WIRE) 
	// 默认是3线SPI的，如果要使用4线SPI，则上电之后要设置一下
	HS6220_write_byte(HS6220_BANK0_DYNPD, 0x08);
#else
	HS6220_write_byte(HS6220_BANK0_DYNPD, 0x00);
#endif
	HS6220_CE_Low();	
	HS6220_write_byte(HS6220_BANK0_CONFIG, 0x8b); // power up
	Delay1ms(3); // wait 3 ms
	HS6220_write_byte(HS6220_BANK0_PMU_CTL, 0xac); // HS6220_PWRDWN = 00
	Delay1ms(2);
	HS6220_Bank_Switch(HS6220_Bank1);
	HS6220_write_byte(HS6220_BANK1_TEST_PKDET, 0x20); // pll_vdiv2_sel = 01, A2 don't need config this bit
	temp[0] = 0x01;
	HS6220_wr_buffer(HS6220_BANK1_FAGC_CTRL_1, temp, 1);
	HS6220_Bank_Switch(HS6220_Bank0);
	HS6220_CE_High();
	Delay1ms(1);
	HS6220_CE_Low(); // 一定要注意，校准的时候CE是低的
	while((HS6220_read_byte(HS6220_BANK0_RF_SETUP) & 0x20) == 0x00);   //wait cal done
	HS6220_write_byte(HS6220_BANK0_RF_SETUP, 0x40);  // cal_en = 0
	HS6220_Bank_Switch(HS6220_Bank1);	
	temp[2] = 0x75;  //bp_dac =1 bp_rc = 1
	temp[1] = 0x98;  // bp_vco_amp = 1 bp_vco_ldo=1
	temp[0] = 0x20;
	HS6220_wr_buffer(HS6220_BANK1_CAL_CTL, temp, 3);
	HS6220_Bank_Switch(HS6220_Bank0);	
	temp[0] = 0x46;
	temp[1] = 0x0b;
	temp[2] = 0xaf;
	temp[3] = 0x43;
	temp[4] = 0x98;	
	HS6220_wr_buffer(HS6220_BANK0_RX_ADDR_P0, temp, 5); // set address
	HS6220_wr_buffer(HS6220_BANK0_TX_ADDR,temp,5);
	HS6220_Clear_All_Irq();
	HS6220_Flush_Tx();			
}

void HS6220_CLEAR_STATUS()
{
    HS6220_Flush_Tx();
    HS6220_Flush_Rx();
    HS6220_Clear_All_Irq();
}

void change_rx_tx_mode(bool recv_mode)
{
    HS6220_Init();
    HS6220_write_byte(HS6220_BANK0_FEATURE,0x10);//HS6220_write_byte
    HS6220_write_byte(HS6220_BANK0_EN_AA,0x00);
    HS6220_write_byte(HS6220_BANK0_CONFIG,0xfa);
    HS6220_write_byte(HS6220_BANK0_RX_PW_P0,rx_lenth);
    HS6220_write_byte(HS6220_BANK0_RF_CH,5);
    HS6220_write_byte(HS6220_BANK0_EN_RXADDR,0x03); 
    HS6220_Change_Pwr(Pwr_8db);
    if (recv_mode)
    {
        mprintf("hs6220 recv mode.\n");
        HS6220_ModeSwitch(HS6220_PRX_Mode);//工作模式切换
        HS6220_CLEAR_STATUS();
        HS6220_CE_High(); 
    }
    else
    {
        mprintf("hs6220 send mode.\n");
        HS6220_ModeSwitch(HS6220_PTX_Mode);//工作模式切换
        HS6220_CLEAR_STATUS();
    }
}
/**
 * @brief 2.4G模式运行入口函数
 */
void ci_24g_exec_loop(void* paragram)
{
    int send_count = 0;
    int recv_count = 0;
    int len = 0;
    int status = 0;
    bool recv_mode = false;        //初始为接收模式
    bool switch_mode = true;
    for (size_t i = 0; i < 10; i++)
    {
        rf_send_data[i] = i;
    }
    
    while (1)
    {
        if (rf_send_msg)
        {
            mprintf("rf_send_msg");
            rf_send_msg = false;
            switch_mode = true;
            recv_mode = false;
        }
        
        if(switch_mode)    //模式标记，运行收发模式切换函数
        {
            switch_mode = false;
            change_rx_tx_mode(recv_mode);  
            recv_count = 0;
            send_count = 0;
        }
        
        if (recv_mode)
        {
            vTaskDelay(pdMS_TO_TICKS(20));
            status = HS6220_read_byte(HS6220_BANK0_STATUS);
            if(HS6220_STATUS_RX_DR & status)
            {
                memset(rf_recv_data, 0 , RF_LEN_MAX);
                rf_recv_len = HS6220_ReceivePack(rf_recv_data,RF_LEN_MAX);

                HS6220_CE_Low();
                HS6220_CLEAR_STATUS();
                HS6220_CE_High();
                rf_cb_funcs.rf_recv(rf_recv_data, rf_recv_len);
            }
        }
        else
        {
            mprintf("send data \r\n");
            HS6220_SendPack(HS6220_W_TX_PAYLOAD, rf_send_data, RF_LEN_MAX);
            HS6220_CE_High();
            Delay1us(40);//vTaskDelay(pdMS_TO_TICKS(1));
            HS6220_CE_Low();
            Delay1us(4000);//vTaskDelay(pdMS_TO_TICKS(4));

            HS6220_CLEAR_STATUS();
            vTaskDelay(pdMS_TO_TICKS(200));
            send_count++;
            if (send_count == 500) //发送5帧数据,切换为接收模式
            {
                send_count = 0;
                switch_mode = true;
                recv_mode = true;
                memset(rf_send_data, 0, RF_LEN_MAX);
            } 
        }
    }
}
