/**
 * @file flash_rw_process.c
 * @brief 用于统一管理软件读写flash，避免同硬件读写flash冲突
 * @version 2.0
 * @date 2018-07-10
 * 
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 * 
 */
#include <stdbool.h>
#include "flash_rw_process.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "status_share.h"


// #include "asr_api.h"

#include "ci_spiflash.h"
#include "ci_dtrflash.h"
#include "ci_scu.h"

static bool before_asr_run = true;
SemaphoreHandle_t flash_ctl_xSemaphore = NULL;

/**
 * @brief 设置asr将要启动，之后的flash操作需要通知到dnn
 * 
 */
void set_asr_run_flag(void)
{
    before_asr_run = false;
}

/**
 * @brief 设置asr停止，之后的flash操作不需要通知到dnn
 * 
 */
void set_asr_stop_flag(void)
{
    before_asr_run = true;
}


/**
 * @brief 初始化flash管理任务
 * @note this init before task run,maybe other task used these resource
 * 
 */
int32_t flash_ctl_init(void)
{
    // flash_ctl_xSemaphore = xSemaphoreCreateMutex();
    flash_ctl_xSemaphore = xSemaphoreCreateBinary();
    if(NULL == flash_ctl_xSemaphore)
    {
        ci_logerr(LOG_FLASH_CTL,"flash_ctl_xSemaphore creat fail\r\n");   
        return RETURN_ERR;
    }
    xSemaphoreGive(flash_ctl_xSemaphore);

    return RETURN_OK;
}

static void wait_flash_sem(void)
{
    if(pdFAIL == xSemaphoreTake(flash_ctl_xSemaphore,pdMS_TO_TICKS(3000)))
    {
        CI_ASSERT(0,"\n");
    }
}


static void give_flash_sem(void)
{
    //ciss_set(CI_SS_FLASH_HOST_STATE,CI_SS_FLASH_IDLE);
    xSemaphoreGive(flash_ctl_xSemaphore);
}


/**
 * @brief 请求控制flash
 * 
 * @retval RETURN_OK 请求成功
 * @retval RETURN_ERR 请求失败
 */
int32_t requset_flash_ctl(void)
{
    wait_flash_sem();
    return 0;
}


/**
 * @brief 解除控制flash
 * 
 * @retval RETURN_OK 解除成功
 * @retval RETURN_ERR 解除失败
 */
int32_t release_flash_ctl(void)
{
    give_flash_sem();
    return RETURN_OK;
}


uint32_t xip_read_flash(uint8_t* buf,uint32_t addr,uint32_t size)
{
    uint32_t tmp;
    uint32_t end_addr = addr + size;
    //mprintf("addr:0x%x, size = %d\n",addr, size);
    uint32_t pre_num = addr & 3;
    if(pre_num)
    {
        tmp = *(uint32_t*)(addr & 0xFFFFFFFC);
        uint32_t real_size = 4 - pre_num;
        if(size < real_size)
        {
            real_size = size;
        }
        memcpy(buf,((uint8_t*)&tmp) + pre_num, real_size);
        addr += real_size;
        buf += real_size; 
    }

    int n = (end_addr - (uint32_t)addr)/4;

    for(int i = 0; i < n; i++)
    {
        *(uint32_t*)buf = *(uint32_t*)addr;
        buf += 4;
        addr += 4;
    }

    if(end_addr & 3)
    {
        tmp = *(uint32_t*)addr;
        memcpy(buf,(uint8_t*)&tmp,end_addr & 3);
    }

    return size;
}

/**
 * @brief 请求读flash
 * 
 * @param buf 数据buff
 * @param addr 数据地址
 * @param size 数据大小
 * @retval RETURN_OK 读取成功
 * @retval RETURN_ERR 读取失败
 */
int32_t post_read_flash(char *buf, uint32_t addr, uint32_t size)
{
    // wait_flash_sem();
    if(addr >= FLASH_CPU_READ_BASE_ADDR)
    {
        addr -= FLASH_CPU_READ_BASE_ADDR;
    }
    
    // wait_flash_sem();
    #if 0 //C101已修复bug，可以直接使用memcpy
    xip_read_flash(buf,FLASH_CPU_READ_BASE_ADDR+addr,size);
    #else
    memcpy(buf,(uint8_t*)(FLASH_CPU_READ_BASE_ADDR+addr), size);
    #endif
    // give_flash_sem();

    return RETURN_OK;
}


/**
 * @brief 请求写flash
 * 
 * @param buf 数据buff
 * @param addr 数据地址
 * @param size 数据大小
 * @retval RETURN_OK 读取成功
 * @retval RETURN_ERR 读取失败
 */
int32_t post_write_flash(char *buf, uint32_t addr, uint32_t size)
{
    static int w_t = 0;
	int ret = RETURN_ERR;
    w_t++;
    wait_flash_sem();
    // if(w_t % 30 == 0)
    {
        mprintf("w_t=%d A=%x S=%d ", w_t, addr, size);
        init_timer0();
        timer0_start_count();
    }
    
    // vTaskSuspendAll();
    flash_config_to_normal();
    ret = flash_write(QSPI0,addr,(uint32_t)buf, size);
    flash_config_to_xip();
    // if(w_t % 30 == 0)
    {
        timer0_end_count_only_print_time_us();
    }
    // xTaskResumeAll();
    give_flash_sem();

    return ret;
}


/**
 * @brief 请求擦除flash
 * 
 * @param addr 数据地址
 * @param size 数据大小
 * @retval RETURN_OK 擦除成功
 * @retval RETURN_ERR 擦除失败
 */
int32_t post_erase_flash(uint32_t addr, uint32_t size)
{
    static int e_t = 0;
	int ret = RETURN_ERR;
    e_t++;
    
    wait_flash_sem();
    mprintf("e_t=%d A=%x S=%d ", e_t, addr, size);
    init_timer0();
    timer0_start_count();
    // vTaskSuspendAll();
    flash_config_to_normal();
    ret = flash_erase(QSPI0,addr,size);
    flash_config_to_xip();
    // xTaskResumeAll();
    timer0_end_count_only_print_time_us();
    give_flash_sem();

    return ret;
}


_NOINLINE_ int32_t post_read_flash_unique_id(uint8_t * dst_addr) 
{
    wait_flash_sem();
    ciss_set(CI_SS_FLASH_BNPU_STATE,CI_SS_FLASH_READ_UNIQUE_ID);
    //写操作流程
    // vTaskSuspendAll();
    flash_config_to_normal();
    int32_t ret = spic_read_unique_id(QSPI0,(uint8_t*)dst_addr);
    flash_config_to_xip();
    // xTaskResumeAll();
    give_flash_sem();
    return ret;
}

static int xip_mod = 0;

void flash_init_to_xip(void)
{
    scu_run_in_flash();
    flash_init(QSPI0);
    spic_xipconfig(QSPI0);
    xip_mod++;
    // vTaskDelay(pdMS_TO_TICKS(10));
}


void flash_config_to_normal(void)
{
    // scu_run_not_in_flash();
    // flash_init(QSPI0);
    CI_ASSERT(xip_mod == 1, "\n");
    xip_mod--;
    spic_prefetch_en(QSPI0,false);
}


void flash_config_to_xip(void)
{
    CI_ASSERT(xip_mod == 0, "\n");
    xip_mod++;
    scu_run_in_flash();
    // flash_init(QSPI0);
    spic_xipconfig(QSPI0);
}

