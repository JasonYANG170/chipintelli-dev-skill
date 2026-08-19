/*============================================================================*/
/* @file exe_app_ota.c
 * @author onmicro
 * @date 2020/02
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "exe_hal.h"
#include "exe_api.h"
#include "ci_log.h"
#if defined(__nds32__)
#include "cpu_mm.h"
#define EXE_FW_PART_ADDR_1ST  0x10000
#define EXE_FW_PART_ADDR_2ND  0x20000
#else
#define __ONCHIP_CODE__
#define EXE_FW_PART_ADDR_1ST  0x00000
#define EXE_FW_PART_ADDR_2ND  0x20000
#endif

extern uint32_t exe_packet_crc24(uint8_t *p_data, int len, uint32_t crc_init);
extern uint8_t vendor_ota_data_value[];

/* 全局变量 true-OTA功能 false-HID功能 */
bool exe_gbool_ota_start;

///烧录的包索引，每包数据16字节。
static uint16_t _ota_pkt_index;
///上位机指定的CRC32.
static uint32_t _ota_crc_pkt;
///下位机计算的CRC32.
static uint32_t _ota_crc_cal;

static __ONCHIP_CODE__ void flash_erase_block(uint32_t addr)
{
  hal_flash_op(addr, 0, 0, 0xd8);
}

static __ONCHIP_CODE__ void flash_write_page(uint32_t addr, uint32_t len, void *buf)
{
  hal_flash_op(addr, len, buf, 0x02);
}

static __ONCHIP_CODE__ void flash_read_page(uint32_t addr, uint32_t len, void *buf)
{
  hal_flash_op(addr, len, buf, 0x03);
}

/* The XIP program is destroied by this routine in RAM. */
static __ONCHIP_CODE__ void _ota_burn_epilogue(void)
{
  uint16_t pkt_idx;
  uint32_t flash_tbuf[16/4];
  /* Erase the 1st partition. */
  flash_erase_block(EXE_FW_PART_ADDR_1ST);
  /* Duplicate the 2nd partition to the 1st partitioin. */
  for (pkt_idx=0; pkt_idx < _ota_pkt_index; pkt_idx++)
  {
    flash_read_page(EXE_FW_PART_ADDR_2ND+pkt_idx*16, 16, flash_tbuf);
    flash_write_page(EXE_FW_PART_ADDR_1ST+pkt_idx*16, 16, flash_tbuf);
  }

  /* Invalidate the 2nd partition. */
  flash_erase_block(EXE_FW_PART_ADDR_2ND);

  hal_pm_reset();
  while (1);
}

/**
 * @brief ATT Write Request for OTA.
 *        cmd2; data16+crc2
 *        0xff00: nothing
 *        0xff01: start, reset _ota_pkt_index
 *        0xff02: done
 *        0x0000: data4 crc4 data8
 *         Index: data16
 *
 * @param [in] p_xpkt_req - ATT写请求，含xpkt_len。
 * @return unused, remove later.
 */
bool ota_att_handler(exe_data_pkt_t *p_xpkt_req)
{
  uint32_t flash_addr;
  uint8_t  flash_tbuf[16];
  uint16_t cmd_pkt_idx = little_endian_read_16(&p_xpkt_req->pdu.att.payload.write_req.Value[0], 0);

#if !defined(NDEBUG)
  unsigned ii;
  if (cmd_pkt_idx <= 0x0001) {
    mprintf("%04x: ", cmd_pkt_idx);
    for (ii=2; ii<p_xpkt_req->l2cap.Length-3; ii++) mprintf("%02x", p_xpkt_req->pdu.att.payload.write_req.Value[ii]);
    mprintf("\n");
  }
  if ((cmd_pkt_idx & 0xFF00) == 0xFF00) mprintf("%04X\n", cmd_pkt_idx); else if (cmd_pkt_idx % 16 == 0) mprintf(".");
#endif
  if (cmd_pkt_idx != 0xff00)
  {
    if (cmd_pkt_idx == 0xff01)
    {
      /* 提高数据带宽。*/
      exe_l2cap_update_conn_para(6, 6, 0, 300);
      exe_gap_disable_latency();
      _ota_pkt_index = 0;
      flash_erase_block(EXE_FW_PART_ADDR_2ND);
      return false;
    } /* end if (cmd_pkt_idx == 0xff01) */

    if (cmd_pkt_idx == 0xff02)
    {
      if ((_ota_crc_pkt != 0) &&
#if defined(__nds32__)
          (_ota_crc_pkt != 0x09000040)
#else
          1
#endif
          ) {
        if (_ota_crc_pkt != _ota_crc_cal)
        {
#if !defined(NDEBUG)
          mprintf("crc check: %08lx in firmware, but %08lx calculated.\n", _ota_crc_pkt, _ota_crc_cal);
#endif
          hal_pm_reset();
          while(1);
        }
      }

#if !defined(__nds32__)
      /* Mark the 2nd partition is ready, and bootstrap will check it. */
      p_xpkt_req->pdu.att.payload.write_req.Value[0] = 0x4b;
      flash_write_page(EXE_FW_PART_ADDR_2ND + 0x8, 1, &p_xpkt_req->pdu.att.payload.write_req.Value[0]);
#else
      /* TBD */
#endif
      _ota_burn_epilogue();
      return false;
    } /* end if (cmd_pkt_idx == 0xff02) */

    memcpy(vendor_ota_data_value, &p_xpkt_req->pdu.att.payload.write_req.Value[0], 20);

    if (cmd_pkt_idx == 0)
    {
      /* 第一包(index=0)的vector1其实是上位机计算的所有数据的crc32，下位机校验时该字段需填充0. */
      _ota_crc_pkt = little_endian_read_32(&p_xpkt_req->pdu.att.payload.write_req.Value[0], 19-13);
      little_endian_store_32(&p_xpkt_req->pdu.att.payload.write_req.Value[0], 19-13, 0);
#if HAL_BLE_HW_CRC
      _ota_crc_cal = 0;
#else
      _ota_crc_cal = exe_packet_crc24(&p_xpkt_req->pdu.att.payload.write_req.Value[2], 16, 0);
#endif
      /* 恢复vector1的内容. */
      memcpy(&p_xpkt_req->pdu.att.payload.write_req.Value[6], &_ota_crc_pkt, 4);
    }
    else
    {
#if HAL_BLE_HW_CRC
      _ota_crc_cal = 0;
#else
      _ota_crc_cal = exe_packet_crc24(&p_xpkt_req->pdu.att.payload.write_req.Value[2], 16, _ota_crc_cal);
#endif
    } /* end if (u5_val16 == 0) */

    flash_addr = EXE_FW_PART_ADDR_2ND + cmd_pkt_idx * 16;
    if (cmd_pkt_idx == 0)
    { p_xpkt_req->pdu.att.payload.write_req.Value[0] = 0xff;
    }
    /* Program. */
    flash_write_page(flash_addr, 16, &p_xpkt_req->pdu.att.payload.write_req.Value[2]);

    /* Verify. */
    flash_read_page(flash_addr, 16, &flash_tbuf[0]);
    if (0 != memcmp(&p_xpkt_req->pdu.att.payload.write_req.Value[2], flash_tbuf, 16))
    {
#if !defined(NDEBUG)
/*       m("failed data index=%04X: ", cmd_pkt_idx);
      for (ii=0; ii<16; ii++) printf("%02x", flash_tbuf[ii]);
      mprintf("\n"); */
#endif
      /* Verification fail. */
      flash_erase_block(EXE_FW_PART_ADDR_2ND);
      hal_pm_reset();
      while(1);
    }

    /* Verification pass. */
    if (_ota_pkt_index < cmd_pkt_idx)
    { _ota_pkt_index = cmd_pkt_idx; }
  } /* end if (cmd_pkt_idx != 0xff00) */

  return false;
}

