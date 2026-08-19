/*============================================================================*/
/* @file exe_int_crypto.h
 * @brief EXE crypto internal header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_INT_CRYPTO_H__
#define __EXE_INT_CRYPTO_H__

#include <stdint.h>

/*
 * BLE Link Layer Security.
 * little endian.
 */

void exe_ll_sec_s1(uint8_t *p_tk, uint8_t *p_mrand, uint8_t *p_srand, uint8_t *p_stk);
void exe_ll_sec_c1(uint8_t *p_key, uint8_t *p_rand, uint8_t *p_p1, uint8_t *p_p2, uint8_t *p_cfm);

void exe_ll_sec_encryption(const uint8_t *p_key, const uint8_t *p_plain, uint8_t *p_cipher);

void exe_ll_sec_ccm_encryption_init(uint8_t *p_key, uint8_t *p_skdm, uint8_t *p_skds, uint8_t *p_ivm, uint8_t *p_ivs);
void exe_ll_sec_ccm_encryption(uint8_t *p_pkt, uint8_t directionBit);
int  exe_ll_sec_ccm_decryption(uint8_t *p_pkt, uint8_t directionBit);

/*
 * Basic crypto algorithm.
 * big endian.
 */

void aes_ccm_ble_cbc_mic(uint8_t *p_nonce, uint8_t *p_payload, uint16_t payload_len, uint8_t hdr0, uint8_t *p_mic);
void aes_ccm_ble_ctr_pdu(uint8_t *p_nonce, uint8_t *p_payload, uint16_t payload_len, uint8_t hdr0, uint8_t *p_mic, uint8_t direction);

int  aes_encrypt_wrapper(uint8_t *p_key, uint8_t *p_plain, uint8_t *p_cipher);
void aes128_setkey(uint8_t *p_key);
void aes128_encrypt(uint32_t *p_wdata);

#endif /* #ifndef __EXE_INT_CRYPTO_H__ */

