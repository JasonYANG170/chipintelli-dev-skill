# Recipe: OTA Firmware Update

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Headers: `components/ota/firmware_updater.h`, `components/ota/flash_update.h`

> Applies to: CI1301, CI1302, CI1303, and CI1306 unless the recipe states narrower support; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/ota.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

OTA (Over-The-Air) firmware update allows updating the CI130X firmware via UART without a programming tool. The bootloader receives new firmware data and writes it to flash.

### Update Components

| Component | Description |
|-----------|-------------|
| Bootloader | Pre-flashed, handles firmware reception and flash writing |
| FileConfig | Partition table in flash at `0x8000` with version/size/CRC for each partition |
| Updater | Application-level OTA handler, runs when update is requested |
| Update Tool | PC-side tool that sends firmware data via UART |

---

## Flash Partition Structure

The flash is divided into partitions managed by the `FileConfig_Struct`:

```c
typedef struct {
    uint32_t ManufacturerID;
    uint32_t ProductID[2];
    // ...
    uint32_t UserCode1Version;       // Application firmware version
    uint32_t UserCode1StartAddr;     // Application start address
    uint32_t UserCode1Size;
    uint32_t UserCode1CRC;
    uint8_t  UserCode1CompltStatus;  // 0xF0=OK, 0xFC=Updating, 0xC0=Old

    uint32_t ASRCMDModelVersion;     // ASR command model
    uint32_t ASRCMDModelStartAddr;
    uint32_t ASRCMDModelSize;
    uint32_t ASRCMDModelCRC;
    uint8_t  ASRCMDModelCompltStatus;

    uint32_t DNNModelVersion;        // DNN model
    uint32_t DNNModelStartAddr;
    uint32_t DNNModelSize;
    uint32_t DNNModelCRC;
    uint8_t  DNNModelCompltStatus;

    uint32_t VoicePlayingVersion;    // Voice prompts
    uint32_t VoicePlayingStartAddr;
    uint32_t VoicePlayingSize;
    uint32_t VoicePlayingCRC;
    uint8_t  VoicePlayingCompltStatus;

    uint32_t UserFileVersion;        // User data files
    uint32_t UserFileStartAddr;
    uint32_t UserFileSize;
    uint32_t UserFileCRC;
    uint8_t  UserFileCompltStatus;
    // ...
    uint16_t PartitionTableChecksum;
} FileConfig_Struct;
```

### Partition Status Values

| Value | Name | Description |
|-------|------|-------------|
| `0xF0` | `USER_CODE_AREA_STA_OK` | Partition is valid and current |
| `0xFC` | `USER_CODE_AREA_STA_UPDATE` | Partition is being updated |
| `0xC0` | `USER_CODE_AREA_STA_OLD` | Partition is old/backup |

---

## OTA Protocol

### Message Types

| Type | Value | Description |
|------|-------|-------------|
| `MSG_TYPE_CMD` | 0xA0 | Command |
| `MSG_TYPE_REQ` | 0xA1 | Request |
| `MSG_TYPE_ACK` | 0xA2 | Acknowledgment |
| `MSG_TYPE_NOTIFY` | 0xA3 | Notification |

### Commands

| Command | Value | Description |
|---------|-------|-------------|
| `MSG_CMD_UPDATE_REQ` | 0x03 | Update request |
| `MSG_CMD_GET_INFO` | 0x04 | Get device info |
| `MSG_CMD_UPDATE_CHECK_READY` | 0x05 | Check if ready for update |
| `MSG_CMD_UPDATE_BLOCK_INFO` | 0x06 | Block info |
| `MSG_CMD_UPDATE_ERA` | 0x07 | Erase |
| `MSG_CMD_UPDATE_WRITE` | 0x08 | Write data |
| `MSG_CMD_UPDATE_VERIFY` | 0x0A | Verify |
| `MSG_CMD_UPDATE_COMPLETE` | 0x0E | Update complete |
| `MSG_CMD_UPDATE_PROGRESS` | 0x11 | Progress notification |

### Message Structure

```c
typedef struct {
    uint16_t msg_head;    // Header
    uint16_t length;      // Payload length
    uint8_t  type;        // Message type
    uint8_t  cmd;         // Command
    uint8_t  number;      // Sequence number
    uint8_t  *data;       // Payload pointer
    uint16_t crc;         // CRC16
    uint8_t  msg_tail;    // Tail
} Data_t;
```

---

## OTA API

### Main Entry Point

```c
#include "firmware_updater.h"

// Enter OTA update mode
// baudrate: UART baud rate for update (e.g., UART_BaudRate115200)
int firmware_update_main(uint32_t baudrate);
```

### Flash Update Functions

```c
#include "flash_update.h"

// Initialize update buffer
int32_t flash_update_buf_init(void);

// Send functions
void send_req_update_req_packet(void);
void send_req_update_write_packet(uint32_t offset, uint32_t size);
void send_req_update_write_packet_ex(uint32_t index, uint32_t offset, uint32_t size);
void send_req_update_block_write_done_packet(void);

// ACK functions
void send_ack_get_info_packet(void);
void send_ack_update_check_ready_packet(void);
void send_ack_update_block_info_packet(void);
void send_ack_update_era_packet(void);
void send_ack_update_verify_packet(uint8_t verify);
void send_ack_update_complete_packet(void);
void send_ack_system_reset(void);
void send_notify_progress_packet(int index, int current, int total);

// State functions
int32_t get_update_state(void);
void set_update_complete_status(void);
int32_t check_req_ack(void);
int32_t have_a_new_message(void);
int32_t check_req_recv(void);

// CRC
uint16_t crc_func(uint16_t crc, uint8_t *buf, uint32_t len);

// UART receive
void receive_func(uint8_t receive_char);
```

---

## Triggering OTA Update

### Method 1: Voice Command Trigger

```c
// In user_msg_deal.c - deal_asr_msg_by_cmd_id():
case 100: // "进入升级模式" (Enter update mode)
{
    // Notify user
    prompt_play_by_cmd_string("<enter_update>", -1, NULL, true);
    vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for prompt to finish

    // Enter OTA mode
    firmware_update_main(UART_BaudRate115200);
    break;
}
```

### Method 2: UART Command Trigger

The OTA update can be triggered by sending the update request command via UART:

```c
// External MCU sends MSG_CMD_UPDATE_REQ (0x03) to the device
// The bootloader/updater receives and processes the request
```

### Method 3: Application Check on Boot

```c pseudocode
// There is no SDK-level check_update_flag() function.
// To implement boot-time OTA, store a flag in NVData (cinv) or a
// dedicated flash sector before resetting, then read it in task_init().
// Example pattern using cinv (see components/ci_nvdm/ci_nvdata_manage.h):

// 1. Before reset, write a flag:
//    cinv_item_write(NVDATA_ID_USER_START, sizeof(flag), &flag);

// 2. In main.c task_init(), read the flag:
//    uint16_t real_len;
//    uint8_t flag = 0;
//    cinv_item_read(NVDATA_ID_USER_START, sizeof(flag), &flag, &real_len);
//    if (flag == OTA_REQUESTED)
//    {
//        firmware_update_main(UART_BaudRate115200);
//    }
```

---

## Update Process Flow

```
1. Host sends MSG_CMD_UPDATE_REQ (0x03)
   Device responds with MSG_TYPE_ACK

2. Host sends MSG_CMD_GET_INFO (0x04)
   Device responds with FileConfig info (versions, sizes, CRC)

3. Host sends MSG_CMD_UPDATE_CHECK_READY (0x05)
   Device responds with ready status

4. Host sends MSG_CMD_UPDATE_BLOCK_INFO (0x06)
   Device responds with block info

5. Host sends MSG_CMD_UPDATE_ERA (0x07)
   Device erases flash sector
   Device responds with erase ACK

6. Host sends MSG_CMD_UPDATE_WRITE (0x08) with data
   Device writes data to flash
   Device responds with write ACK

7. Repeat 4-6 for each data block

8. Host sends MSG_CMD_UPDATE_VERIFY (0x0A)
   Device verifies CRC
   Device responds with verify result

9. Host sends MSG_CMD_UPDATE_COMPLETE (0x0E)
   Device updates FileConfig status
   Device sends MSG_CMD_SYS_RST (0xA1) - system reset
```

---

## Updating Individual Partitions

The OTA protocol supports updating individual partitions:

### Update Application Code Only

```
1. Send update request
2. Host sends UserCode1 data
3. Device erases UserCode1 area
4. Device writes new UserCode1
5. Device updates FileConfig.UserCode1CompltStatus = 0xFC (updating)
6. After verify: FileConfig.UserCode1CompltStatus = 0xF0 (OK)
7. Reset to load new firmware
```

### Update ASR Model Only

```
1. Send update request
2. Host sends ASRCMDModel data
3. Device erases ASR model area
4. Device writes new ASR model
5. Update FileConfig.ASRCMDModelCompltStatus
6. Reset to load new model
```

### Update Voice Prompts Only

```
1. Send update request
2. Host sends VoicePlaying data
3. Device erases voice area
4. Device writes new voice files
5. Update FileConfig.VoicePlayingCompltStatus
```

---

## Bootloader Configuration

### Flash Constants (from flash_update.h)

```c
#define FILECONFIG_SPIFLASH_START_ADDR  0x8000    // FileConfig at 32KB offset
#define FILECONFIG_SPIFLASH_SIZE        4096       // 4KB
#define USERCODE_MAX_SIZE               (1024*448) // 448KB max user code
#define USERCODE_PER_SIZE               4096        // 4KB erase unit
#define UNIQUE_ID_LENGTH               16
```

### Bootloader Version

```c
#define BOOT_LOADER_NEW_STR     "V20102"
#define BOOT_LOADER_NEW_MAJOR   0x02
#define BOOT_LOADER_NEW_MINOR   0x01
#define BOOT_LOADER_NEW_RELEASE 0x02
```

---

## source_file.prj for OTA

Ensure OTA source files are included:

```
source-file: components/ota/firmware_updater.c
```

Library:
```
library-file: $(LIBS_PATH)/libflash_encrypt.a
```

---

## PC-Side Update Tool

The SDK includes a PC tool for firmware updates:

```bash
# Using ci-tool-kit for OTA
$SDK_ROOT/tools/ci-tool-kit update -p COM3 -b 115200 -f firmware.bin
```

Or use the Chipintelli OTA tool with a GUI interface.

---

## Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| Update fails to start | Wrong UART or baud rate | Verify UART connection and baud rate |
| Update stuck at erase | Flash write protection | Call `spic_protect(QSPI0, DISABLE)` before erase |
| CRC verification fails | Data corruption during transfer | Check UART cable quality, lower baud rate |
| Device doesn't boot after update | FileConfig not updated | Ensure `set_update_complete_status()` is called |
| Partial update fails | Partition not properly erased | Erase full 4KB sectors |
| OTA times out | Large firmware over slow UART | Increase timeout or use higher baud rate |
| Lost old firmware | No backup partition | Consider dual-partition (A/B) update scheme |
| ASR stops working after update | Model not updated with firmware | Update both UserCode and ASR model partitions |

---

## Safety Considerations

1. **Power failure during update**: The `CompltStatus` field tracks update state. If status is `0xFC` (updating) and power is lost, the bootloader can detect the incomplete update and attempt recovery or fall back to the old partition.

2. **CRC verification**: Always verify CRC after writing. The `send_ack_update_verify_packet()` sends back the verification result.

3. **Flash erase before write**: Always erase flash before writing. Use `MSG_CMD_UPDATE_ERA` to erase sectors.

4. **Bootloader integrity**: The bootloader is pre-flashed and should never be updated via OTA. It resides in the first 32KB of flash.

5. **Partition table checksum**: The `PartitionTableChecksum` field protects the FileConfig from corruption. Verify this after any update.
