# Recipe: OTA Firmware Update (CI13XX)

> SDK: `CI13XX_SDK_ASR_ALG_V2.7.12`
> Chips: CI1306, CI1311, CI1312, CI1316, CI1324, CI1332, CI2312

> Evidence: `chips/ci13xx/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13xx/recipes/ota.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

CI13XX OTA firmware update allows updating firmware, ASR models, voice prompts, and user data via UART. The CI13XX SDK adds `ota_aes.h` for AES encryption support and `ota_partition_verify.h` for partition verification. The OTA protocol is compatible with CI130X/CI13LC.

---

## SDK

`CI13XX_SDK_ASR_ALG_V2.7.12` - component: `ota`

---

## Source Anchors

- `CI13XX_SDK_ASR_ALG_V2.7.12/components/ota/firmware_updater.h` - OTA main entry point
- `CI13XX_SDK_ASR_ALG_V2.7.12/components/ota/flash_update.h` - flash update protocol, packet structures
- `CI13XX_SDK_ASR_ALG_V2.7.12/components/ota/ota_aes.h` - AES encryption for OTA
- `CI13XX_SDK_ASR_ALG_V2.7.12/components/ota/ota_partition_verify.h` - partition verification
- `CI13XX_SDK_ASR_ALG_V2.7.12/projects/offline_asr_alg_pro_sample/app/app_ota/ota_config.h` - OTA configuration (in LLM_AIOT SDK)

---

## API Usage

## Main Entry Point (firmware_updater.h)

```c
// Enter OTA update mode
// baudrate: UART baud rate for update communication
int firmware_update_main(uint32_t baudrate);
```

## AES Encryption (ota_aes.h)

```c
// Get OTA AES info address
uint32_t ota_aes_info_addr_func(void);

// Check OTA AES message
void ota_aes_msg_check(void);
```

## Partition Verification (ota_partition_verify.h)

Includes:
- `ci130x_spiflash.h` - flash driver
- `flash_update.h` - OTA protocol
- `flash_manage_outside_port.h` - flash management
- `ci_flash_data_info.h` - partition table

## Flash Update Protocol (flash_update.h)

### Flash Port

```c
#define FLASH_SPI_PORT  QSPI0
```

### Partition Status

```c
#define USER_CODE_AREA_STA_UPDATE  0xFC  // Being updated
#define USER_CODE_AREA_STA_OK      0xF0  // Valid
#define USER_CODE_AREA_STA_OLD     0xC0  // Old/backup
```

### Flash Layout

```c
#define FILECONFIG_SPIFLASH_SIZE  4096         // 4KB partition table
#define USERCODE_MAX_SIZE          (1024*448)   // 448KB max user code
#define USERCODE_PER_SIZE          4096          // 4KB erase unit
#define UNIQUE_ID_LENGTH           16
```

### Message Types and Commands

```c
// Types
#define MSG_TYPE_CMD     0xA0
#define MSG_TYPE_REQ     0xA1
#define MSG_TYPE_ACK     0xA2
#define MSG_TYPE_NOTIFY  0xA3

// Commands
#define MSG_CMD_UPDATE_REQ             0x03
#define MSG_CMD_GET_INFO               0x04
#define MSG_CMD_UPDATE_CHECK_READY     0x05
#define MSG_CMD_UPDATE_BLOCK_INFO      0x06
#define MSG_CMD_UPDATE_ERA             0x07
#define MSG_CMD_UPDATE_WRITE           0x08
#define MSG_CMD_UPDATE_BLOCK_WRITE_DONE 0x09
#define MSG_CMD_UPDATE_VERIFY          0x0A
#define MSG_CMD_TRY_FAST_BD            0x0b
#define MSG_CMD_TRY_FAST_BD_TEST       0x0c
#define MSG_CMD_UPDATE_READ            0x0d
#define MSG_CMD_UPDATE_COMPLETE        0x0e
#define MSG_CMD_UPDATE_EXTERNAL_DEV    0x10
#define MSG_CMD_UPDATE_PROGRESS        0x11
#define MSG_CMD_SYS_RST                0xA1
```

### Message Structure

```c
typedef struct {
    uint16_t msg_head;
    uint16_t length;
    uint8_t  type;
    uint8_t  cmd;
    uint8_t  number;
    uint8_t  *data;
    uint16_t crc;
    uint8_t  msg_tail;
} Data_t;
```

### Partition Table (FileConfig_Struct)

Same structure as CI13LC. See `flash_update.h` for full definition.

### Functions

```c
// Initialize update buffer
int32_t flash_update_buf_init(void);

// UART send/receive
void send_func(void);
void receive_func(uint8_t receive_char);

// Packet processing
void Resolution_func(void);

// CRC
uint16_t crc_func(uint16_t crc, uint8_t *buf, uint32_t len);

// Request packets
void send_req_update_req_packet(void);
void send_req_update_write_packet(uint32_t offset, uint32_t size);
void send_req_update_write_packet_ex(uint32_t index, uint32_t offset, uint32_t size);
void send_req_update_block_write_done_packet(void);

// ACK packets
void send_ack_get_info_packet(void);
void send_ack_update_check_ready_packet(void);
void send_ack_update_block_info_packet(void);
void send_ack_update_era_packet(void);
void send_ack_update_verify_packet(uint8_t verify);
void send_ack_try_fast_bd_packet(void);
void send_ack_try_fast_bd_test_packet(uint32_t size);
void send_ack_update_read_packet(uint32_t StartAddr, uint32_t Size);
void send_ack_update_complet_packet(void);
void send_ack_update_extern_dev_packet(void);
void send_ack_system_reset(void);
void send_notify_progress_packet(int index, int current, int total);

// State
int32_t get_update_state(void);
void set_update_complete_status(void);
int32_t check_req_ack(void);
int32_t have_a_new_message(void);
int32_t check_req_recv(void);
```

## OTA Configuration (user_config.h)

```c
// Enable OTA
#define CI_OTA_ENABLE    0   // Set to 1 to enable

// OTA timeout settings
#define OTA_TIMOUT       3    // MCU interaction timeout (seconds)
#define OTA_RETRY_TIME   10   // MCU retry count

// Chip type for OTA
#if (CI_CHIP_TYPE == 1306)
#define OTA_CHIP_TYPE    0x06
#else
#define OTA_CHIP_TYPE    0x02
#endif
```

---

## Usage Example

### Enable OTA in user_config.h

```c
// In user_config.h:
#define CI_OTA_ENABLE    1
#define OTA_TIMOUT       3
#define OTA_RETRY_TIME   10
```

### Trigger OTA

```c
#include "firmware_updater.h"

void enter_ota_mode(void)
{
    // Play "entering update" prompt if available
    // ...

    // Wait for prompt
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Enter OTA update mode
    firmware_update_main(UART_BaudRate115200);
}
```

### OTA with AES Check

```c
#include "ota_aes.h"

void ota_with_aes_check(void)
{
    // Check AES message before entering OTA
    ota_aes_msg_check();

    // Get OTA info address
    uint32_t aes_info = ota_aes_info_addr_func();

    // Enter OTA
    firmware_update_main(UART_BaudRate115200);
}
```

### Boot-Time OTA Check

```c pseudocode
// Use ci_flash_data_info cias_ota_flag_t structure:
// typedef struct {
//     unsigned int cias_ota_chip_type;     // Chip type
//     unsigned char cias_ota_uart_port;   // OTA UART port (0/1/2)
//     UART_BaudRate cias_ota_baud;       // OTA baud rate
// } cias_ota_flag_t;

// Store OTA request in flash before reset:
// cias_ota_flag_t ota_flag = {
//     .cias_ota_chip_type = OTA_CHIP_TYPE,
//     .cias_ota_uart_port = 0,
//     .cias_ota_baud = UART_BaudRate115200,
// };
// Write to flash at a known address...

// In task_init(), check for OTA flag:
// Read the flag from flash
// If flag indicates OTA requested:
//     firmware_update_main(ota_flag.cias_ota_baud);
```

### Update Process Flow

```
1. Host sends MSG_CMD_UPDATE_REQ (0x03) -> Device ACKs
2. Host sends MSG_CMD_GET_INFO (0x04) -> Device sends FileConfig
3. Host sends MSG_CMD_UPDATE_CHECK_READY (0x05) -> Device ACKs ready
4. Host sends MSG_CMD_UPDATE_BLOCK_INFO (0x06) -> Device ACKs block info
5. Host sends MSG_CMD_UPDATE_ERA (0x07) -> Device erases, ACKs
6. Host sends MSG_CMD_UPDATE_WRITE (0x08) + data -> Device writes, ACKs
7. Repeat 4-6 for each block
8. Host sends MSG_CMD_UPDATE_VERIFY (0x0A) -> Device verifies CRC, ACKs
9. Host sends MSG_CMD_UPDATE_COMPLETE (0x0E) -> Device updates status, resets
```

---

## Notes/Tips

- Set `CI_OTA_ENABLE=1` in `user_config.h` to enable OTA support. This changes `FILECONFIG_SPIFLASH_START_ADDR` from `0x2000` to `0x6000`.
- The `ota_aes.h` provides AES encryption support for secure OTA updates. Call `ota_aes_msg_check()` before entering OTA mode if encryption is used.
- `ota_partition_verify.h` includes dependencies for partition verification: flash driver, OTA protocol, flash management, and partition table.
- The CI13XX LLM_AIOT SDK (`CI13XX_SDK_LLM_AIOT_2.1.2`) has project-level OTA config files at `projects/<sample>/app/app_ota/ota_config.h`.
- When `CI_OTA_ENABLE=1`, the firmware must be packed with the V4 packaging tool (set `firmware_version=FW_V4` in `firmware/config.ini`).
- The OTA protocol supports updating individual partitions (UserCode, ASR model, DNN model, voice, user files) independently.
- Flash erase must use 4KB sector boundaries (`USERCODE_PER_SIZE`).
- After OTA, the system resets automatically via `send_ack_system_reset()`.
- Source files: ensure `components/ota/firmware_updater.c` is included in `source_file.prj`.
