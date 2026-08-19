/**
 * @file main.c
 * @brief 示例程序
 * @version 1.0.0
 * @date 2021-03-19
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdio.h> 
#include <string.h>
#include <malloc.h>
#include "FreeRTOS.h" 
#include "task.h"
#include "semphr.h"
#include "sdk_default_config.h"
#include "ci_core_eclic.h"
#include "ci_core_misc.h"
#include "ci_spiflash.h"
#include "ci_gpio.h"
#include "ci_flash_data_info.h"
#include "board.h"
#include "ci_uart.h"
#include "flash_rw_process.h"
#include "system_msg_deal.h"
#include "ci_dpmu.h"
#include "romlib_api.h"
#include "ci_log.h"
#include "status_share.h"
#include "asr_api.h"
#include "alg_preprocess.h"
#include "ci_iwdg.h"
#include "../../system/port_api.h"
#include "flash_rw_process.h"
#include "ci_debug_config.h"
#include "codec_manager.h"
#include "simple_mp3_player.h"
#include <string.h>
#if USE_CWSL 
#include "cwsl_manage.h"
#endif
#if USE_BLE_MOUDLE
#include "ble_param_config.h"
#include "ble_main.h"
#include "ble_communicate.h"
#include "cias_ble_msg_deal.h"
#endif

/**
 * @brief 硬件初始化
 *          这个函数主要用于系统上电后初始化硬件寄存器到初始值，配置中断向量表初始化芯片io配置时钟
 *          配置完成后，系统时钟配置完毕，相关获取clk的函数可以正常调用
 */
static void hardware_default_init(void)
{
    /* 配置外设复位，硬件外设初始化 */
    #if ((!USE_V5) && (!USE_V7))
	extern void SystemInit(void);
    SystemInit();
    #endif

	/* 设置中断优先级分组 */
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO0);
    
	/* 开启全局中断 */
	eclic_global_interrupt_enable();
	enable_mcycle_minstret();

    /* 初始化maskrom lib */
    maskrom_lib_init();

	init_platform();

    // TODO
    // #if !(USE_INNER_LDO3)
    // dpmu_ldo3_en(false);
    // dpmu_config_update_en(DPMU_UPDATE_EN_NUM_LDO3);
    // #endif

    extern void flash_init_to_xip(void);
    flash_init_to_xip();

    void debug_mode_init_code_in_flash();
    debug_mode_init_code_in_flash();

    write_csr(0x7CA,0x01);      // 开启ICACHE

    //ICACHE_DISABLE_INT
    scu_unlock_system_config();
    *(volatile uint32_t *)(HAL_SCU_BASE + 0X2D4) = 1;
    scu_lock_system_config();
}


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
void check_chip_name()
{
    char *a = TOSTRING(BD_SURPPORT_CHIP_NAME);
    char *b = TOSTRING(CI_CHIP_TYPE);
    if (!strstr(a, b))
    {
        ci_logerr(LOG_SYS_INFO, "CI_CHIP_TYPE error, please define it with a name of the chip included in BD_SURPPORT_CHIP_NAME.\n");
    }
}

/**
 * @brief 用于平台初始化相关代码
 *
 * @note 在这里初始化硬件需要注意：
 *          由于部分驱动代码中使用os相关接口，在os运行前调用这些接口会导致中断被屏蔽
 *          其中涉及的驱动包括：QSPIFLASH、DMA、I2C、SPI
 *          所以这些外设的初始化需要放置在vTaskVariablesInit进行。
 *          如一定需要（非常不建议）在os运行前初始化这些驱动，请仔细确认保证：
 *              1.CONFIG_DIRVER_BUF_USED_FREEHEAP_EN  宏配置为0
 *              2.DRIVER_OS_API                     宏配置为0
 */
_XIF_ static int platform_init(void)
{   
    #if SIMULATE_UART_ENABLE   //初始化模拟串口
    simulate_for_uart();
    #endif
    #if CONFIG_CI_LOG_UART
    ci_log_init();      //初始化日志模块
    #if COMMAND_LINE_CONSOLE_EN         
    vUARTCommandConsoleStart( 256, 4);
    #endif
    #endif

    #if (CONFIG_CI_LOG_UART == UART_PROTOCOL_NUMBER && MSG_COM_USE_UART_EN)
	CI_ASSERT(0,"Log uart and protocol uart confict!\n");
    #endif
    
    #if CONFIG_SYSTEMVIEW_EN   
    /* 初始化SysView RTT，仅用于调试 */
	SEGGER_SYSVIEW_Conf();
	/* 使用串口方式输出sysview信息 */
	vSYSVIEWUARTInit();
	ci_logdebug(CI_LOG_DEBUG, "Segger Sysview Control Block Detection Address is 0x%x\n",&_SEGGER_RTT);
    #endif

    check_chip_name();

    // 默认用硬件打开的iwdg
    // iwdg_init_t init;
    // init.irq = iwdg_irqen_enable;
    // init.res = iwdg_resen_enable;
    // init.count = ((get_src_clk()/0x10)*3);/* IWDG时钟从src_clk经过16分频得到, 当前配置为2秒*/
    // scu_set_device_gate(IWDG, ENABLE);
    // dpmu_iwdg_reset_system_config();
    // iwdg_init(IWDG,init);
    // iwdg_open(IWDG);

    // iwdg_close(HAL_IWDG_BASE);

    iwdg_config_reset(HAL_IWDG_BASE);

	return 0;
}


/**
 * @brief sdk上电信息打印
 *
 */
_XIF_ static void welcome(void)
{
    ci_loginfo(LOG_USER,"\r\n");
    ci_loginfo(LOG_USER,"\r\n");
    ci_loginfo(LOG_USER,"\033[1;32mWelcome to CI23LC_SDK.\033[0;39m\r\n");
    ci_loginfo(LOG_USER,"ci23lc_sdk_%s_%d.%d.%d Built-in\r\n",
               SDK_TYPE,
               SDK_VERSION,SDK_SUBVERSION,SDK_REVISION);
    ci_loginfo(LOG_USER,"Project: %s\r\n", PROJECT_NAME);
    ci_loginfo(LOG_USER,"%s\r\n", COMPILE_TIME);
    extern uint32_t get_chip_type_from_chip(void);
    uint32_t t = get_chip_type_from_chip();
    if (t == 13080)
    {
        //ci_loginfo(LOG_USER,"chip type in chip:1308X\n");
    }
    else
    {
        //ci_loginfo(LOG_USER,"chip type in chip:%d\n", (int)t);
    }
    ci_loginfo(LOG_USER,"chip type in software:%d\n", (int)CI_CHIP_TYPE);
    
    extern char heap_start;
    extern char heap_end;
    ci_loginfo(LOG_USER,"Heap size:%dKB\n", (((uint32_t)&heap_end) - ((uint32_t)&heap_start))/1024);
    ci_loginfo(LOG_USER,"Freq factor %d\n", (int)(get_freq_factor()*1000));
    ci_loginfo(LOG_USER,"Freq %d\n", (int)(get_ipcore_clk()));

    // 实际主频检查
    if (abs(((int)get_ipcore_clk()) - ((int)MAIN_FREQUENCY)) > 10000000)
    {
        mprintf("PLL config err!\n");
        while(1);
    }
}

TaskHandle_t ble_task_handle = NULL;
_XIF_ static void task_init(void *p_arg)
{
    ciss_init();//bnpu的ciss_init需要在mailboxboot_sync之后调用
    ciss_set(CI_SS_DECODER_MIN_ACTIVE,DECODER_MIN_ACTIVE);
    float beam = DECODER_BEAM;
    ciss_set(CI_SS_DECODER_BEAM,*(uint32_t*)&beam);
    
    cm_init();

    /* 注册录音codec */
    audio_in_codec_registe();
    /* 注册语音前段信号处理模块 */
    extern ci_ssp_config_t ci_ssp;
    extern audio_capture_t audio_capture;
    set_ssp_registe(&audio_capture, (ci_ssp_st*)&ci_ssp, sizeof(ci_ssp)/sizeof(ci_ssp_st));

    asr_process_init();
    #if AUDIO_PLAYER_ENABLE
    /* 播放器任务 */
    smp_init();//audio_play_init();
    #endif

    /* 用户任务 */
    sys_msg_task_initial();
    xTaskCreate(UserTaskManageProcess,"UserTaskManageProcess",480,NULL,4,NULL);
	#if USE_BLE_MOUDLE
    #if APP_GET_CMD_INFO_ENABEL
	extern void get_command_info_sheet(void);
    get_command_info_sheet();
    #endif
    extern BleInitCfg_t gBleInitCfg;
    gBleInitCfg.ble_send_data_callback = ble_send_payload;   //蓝牙发送数据函数
    #if USE_CI_APPLET_ENABEL   //使用启英小程序
    gBleInitCfg.ble_recv_data_callback = ci_ble_recv_data_handle;   
    gBleInitCfg.ble_connected_callback = ci_ble_connected_handle;   
    gBleInitCfg.ble_disconnected_callback = ci_ble_disconnect_handle;
    gBleInitCfg.ble_adv_data_init = ci_ble_adv_data_init;
    #else //客户使用自己小程序自己实现接受收处理函数
    
    #endif
    extern void ble_main_task(void);
    xTaskCreate(ble_main_task,"ble_main_task",480,NULL,4,&ble_task_handle);
    extern void ci_ble_recv_task(void);
    xTaskCreate(ci_ble_recv_task,  "ci_ble_recv_task",480,NULL,4,NULL); 
    #endif
    #if (!COMMAND_LINE_CONSOLE_EN)
    while(1) 
    {
        UBaseType_t ArraySize = 10;
        TaskStatus_t *StatusArray;
        //ArraySize = uxTaskGetNumberOfTasks();
        StatusArray = pvPortMalloc(ArraySize*sizeof(TaskStatus_t));
        if (StatusArray && ArraySize)
        {
            uint32_t ulTotalRunTime;
            volatile UBaseType_t ArraySize2 = uxTaskGetSystemState(StatusArray, ArraySize, &ulTotalRunTime);
            mprintf("TaskName\t\tPriority\tTaskNumber\tMinStk\t%d\n", ArraySize2);
            for (int i = 0;i < ArraySize2;i++)
            {
                mprintf("% -16s\t%d\t\t%d\t\t%d\r\n",
                    StatusArray[i].pcTaskName,
                    (int)StatusArray[i].uxCurrentPriority,
                    (int)StatusArray[i].xTaskNumber,
                    (int)StatusArray[i].usStackHighWaterMark
                );
            }
            mprintf("\n");
            extern int get_heap_bytes_remaining_size(void);
            mprintf("asr heap min free:%dKB\n", get_heap_bytes_remaining_size()/1024);
            mprintf("system heap min free:%dKB\n", xPortGetMinimumEverFreeHeapSize()/1024);
            mprintf("system heap free:%dKB\n", xPortGetFreeHeapSize()/1024);
        }
        vPortFree(StatusArray);
        vTaskDelay(pdMS_TO_TICKS(15000));
    }
    #else
    vTaskDelete(NULL);
    #endif
}
 

/**
 * @brief
 *  
 */
int main(void)
{
    hardware_default_init();

    /*平台相关初始化*/
    platform_init();

    /* 版本信息 */
    welcome();
    
    /* 创建启动任务 */
    xTaskCreate(task_init,"init task",300,NULL,4,NULL);
 
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();

    while(1){}
}


