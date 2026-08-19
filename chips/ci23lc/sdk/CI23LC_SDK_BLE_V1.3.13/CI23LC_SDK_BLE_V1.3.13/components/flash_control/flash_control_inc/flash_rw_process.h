/**
 * @file flash_rw_process.h
 * @brief 用于统一管理软件读写flash，避免同硬件读写flash冲突
 * @version 2.0
 * @date 2018-07-10
 * 
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 * 
 */

#ifndef _FLASH_RW_PEOCESS_H
#define _FLASH_RW_PEOCESS_H

#include "ci_log.h"
#include "ci_system.h"
#include "FreeRTOS.h"

/**
 * @addtogroup flash_control
 * @{
 */

void set_asr_run_flag(void);
void set_asr_stop_flag(void);
int32_t flash_ctl_init(void);

int32_t requset_flash_ctl(void);
int32_t release_flash_ctl(void);
int32_t post_write_flash(char *buf, uint32_t addr, uint32_t size);
int32_t post_read_flash(char *buf, uint32_t addr, uint32_t size);
int32_t post_erase_flash(uint32_t addr, uint32_t size);
int32_t post_read_flash_unique_id(uint8_t * dst_addr);


void flash_init_to_xip(void);
void flash_config_to_normal(void);
void flash_config_to_xip(void);

/** @} */   

#endif /* _FLASH_RW_PEOCESS_H */

