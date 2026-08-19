/*============================================================================*/
/* @file exe_hal_cfg.c
 * @brief EXE APIs of configuration data in flash/OTP storage.
 * @author onmicro
 * @date 2020/02
 */

#include <stdint.h>
#include "exe_hal.h"


/* 全局指针指向配置区。*/
#if defined(__C51__)
hal_storage_cfg_t *g_p_cfg;
#else
hal_storage_cfg_t *g_p_cfg = (hal_storage_cfg_t *)HAL_STORAGE_CFG_BASE;
#endif

/**
 * @brief 从flash/OTP获得本设备蓝牙地址。
 *
 * @param [out] p_mac - 从flash/OTP获得BDA，填入这个用户buffer.
 * @return None.
 */
void hal_cfg_set_bda(uint8_t *p_mac)
{
  /* Omit the erased flash. */
  if (g_p_cfg->bda[0] == 0xff) return;
  /* Omit the virgin OTP. */
  if ((g_p_cfg->bda[0] | g_p_cfg->bda[1]) == 0x00) {
#if defined(HM1001_M0)
      return;
#endif
#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
    /* Generate static random address into the virgin OTP. */
    p_mac[0] = 0x33;
    little_endian_store_32(p_mac, 1, hal_rng_get_word());
    /* 等初次上电稳定，再编程OTP，否则易损坏代码区。*/
    //vTaskDelay(pdMS_TO_TICKS(500));
    hal_flash_op(&g_p_cfg->bda[0] - (uint8_t *)0, 6, p_mac, PORT_NVM_OP_PROGRAM);
#endif
    return;
  }
  memcpy(p_mac, &g_p_cfg->bda[0], 6);
}

/**
 * @brief 从flash/OTP配置区取得最近的设备名。
 *
 * @param [out] p_devname - 指向用户指定的设备名buffer.
 *                          为节省内存其位于exe_gbuf_adv_ind里，长度不大于EXE_ADV_DEV_NAME_LEN.
 */
void hal_cfg_set_devname(uint8_t *p_devname)
{
  int idx;
  uint8_t dev_name_len = 0;
  for (idx=0; idx<CFG_DEV_NAME_LEN_MAX; idx++)
  {
    if ((g_p_cfg->dev_name[CFG_DEV_NAME_LEN_MAX-1-idx] != 0x00) &&
        (g_p_cfg->dev_name[CFG_DEV_NAME_LEN_MAX-1-idx] != 0xff))
    {
      dev_name_len++;
	} else {
	  if (dev_name_len) break;
	}
  }
  if (dev_name_len != 0)
  {
    uint8_t dev_idx = CFG_DEV_NAME_LEN_MAX-1-dev_name_len+1;
    memset(p_devname, ' ', EXE_ADV_DEV_NAME_LEN);
    memcpy(p_devname, &g_p_cfg->dev_name[dev_idx], dev_name_len);
  }
}

void hal_cfg_set_rx_gain(void)
{}

/**
 * @brief 从flash/OTP配置频偏。
 *
 * @return None.
 */
void hal_cfg_set_freq_offset(uint8_t freq_off_cw)
{
  switch (g_p_cfg->freq_off) {
  case 0xfe:
  case 0x00:
    hal_rf_set_freq_off(0);
    break;

  case 0xff:
    /* 设置成用户传入的参数。*/
    hal_rf_set_freq_off(freq_off_cw);
    break;

  default:
    /* 设置成CFG的配置。*/
    hal_rf_set_freq_off(g_p_cfg->freq_off);
    break;
  }
}

