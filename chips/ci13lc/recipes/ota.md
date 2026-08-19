# Recipe: OTA Firmware Update (CI13LC)

> SDK: `CI13LC_SDK_V2.0.15`
> Chips: CI1311, CI1312, CI1316, CI1324, CI1332

---

## Overview

OTA (Over-The-Air) firmware update on CI13LC allows updating firmware, ASR models, voice prompts, and user data via UART. The bootloader receives new firmware data using a packet protocol and writes it to flash. The CI13LC OTA uses the same protocol structure as CI130X.

---

## SDK

`CI13LC_SDK_V2.0.15` - component: `ota`

---

## Source Anchors

- `components/ota/firmware_updater.h` - OTA main entry point
- `components/ota/flash_update.h` - flash update protocol, packet structures, partition table

---

## API Usage

## Main Entry Point (firmware_updater.h)

```c
// Enter OTA update mode
// baudrate: UART baud rate for update communication
int firmware_update_main(uint32_t baudrate);
```

## Flash Update Protocol (flash_update.h)

### Flash Port

```c
#define FLASH_SPI_PORT  QSPI0  // SPI flash controller used for OTA
```

### Partition Status Values

```c
#define USER_CODE_AREA_STA_UPDATE  0xFC  // Partition is being updated
#define USER_CODE_AREA_STA_OK      0xF0  // Partition is valid and current
#define USER_CODE_AREA_STA_OLD     0xC0  // Partition is old/backup
```

### Bootloader Version

```c
#define BOOT_LOADER_NEW_STR     "V20102"
#define BOOT_LOADER_NEW_MAJOR   0x02
#define BOOT_LOADER_NEW_MINOR   0x01
#define BOOT_LOADER_NEW_RELEASE 0x02
```

### Flash Layout Constants

```c
#define FILECONFIG_SPIFLASH_SIZE  4096         // 4KB partition table
#define USERCODE_MAX_SIZE          (1024*448)   // 448KB max user code
#define USERCODE_PER_SIZE          4096         // 4KB erase unit
#define UNIQUE_ID_LENGTH           16
```

### Message Types

```c
#define MSG_TYPE_CMD     0xA0  // Command
#define MSG_TYPE_REQ     0xA1  // Request
#define MSG_TYPE_ACK     0xA2  // Acknowledgment
#define MSG_TYPE_NOTIFY  0xA3  // Notification
```

### Commands

```c
#define MSG_CMD_UPDATE_REQ             0x03  // Update request
#define MSG_CMD_GET_INFO               0x04  // Get device info
#define MSG_CMD_UPDATE_CHECK_READY     0x05  // Check ready
#define MSG_CMD_UPDATE_BLOCK_INFO      0x06  // Block info
#define MSG_CMD_UPDATE_ERA             0x07  // Erase
#define MSG_CMD_UPDATE_WRITE           0x08  // Write data
#define MSG_CMD_UPDATE_BLOCK_WRITE_DONE 0x09  // Block write done
#define MSG_CMD_UPDATE_VERIFY          0x0A  // Verify
#define MSG_CMD_TRY_FAST_BD            0x0b  // Fast block transfer
#define MSG_CMD_UPDATE_READ            0x0d  // Read
#define MSG_CMD_UPDATE_COMPLETE        0x0e  // Update complete
#define MSG_CMD_UPDATE_EXTERNAL_DEV    0x10  // External device
#define MSG_CMD_UPDATE_PROGRESS        0x11  // Progress notification
#define MSG_CMD_SYS_RST                0xA1  // System reset
```

### Message Structure

```c
typedef struct {
    uint16_t msg_head;    // Header
    uint16_t length;      // Payload length
    uint8_t  type;        // Message type (MSG_TYPE_*)
    uint8_t  cmd;         // Command (MSG_CMD_*)
    uint8_t  number;      // Sequence number
    uint8_t  *data;       // Payload pointer
    uint16_t crc;         // CRC16
    uint8_t  msg_tail;    // Tail byte
} Data_t;
```

### Partition Table (FileConfig_Struct)

```c
#pragma pack(1)
typedef struct {
    uint32_t ManufacturerID;
    uint32_t ProductID[2];          // MAC Address
    uint32_t HWName[16];
    uint32_t HWVersion;
    uint32_t SWName[16];
    uint32_t SWVersion;
    uint32_t BootLoaderVersion;
    uint8_t  Reserve[14];
    uint32_t UserCode1Version, UserCode1StartAddr, UserCode1Size, UserCode1CRC;
    uint8_t  UserCode1CompltStatus;
    uint32_t UserCode2Version, UserCode2StartAddr, UserCode2Size, UserCode2CRC;
    uint8_t  UserCode2CompltStatus;
    uint32_t ASRCMDModelVersion, ASRCMDModelStartAddr, ASRCMDModelSize, ASRCMDModelCRC;
    uint8_t  ASRCMDModelCompltStatus;
    uint32_t DNNModelVersion, DNNModelStartAddr, DNNModelSize, DNNModelCRC;
    uint8_t  DNNModelCompltStatus;
    uint32_t VoicePlayingVersion, VoicePlayingStartAddr, VoicePlayingSize, VoicePlayingCRC;
    uint8_t  VoicePlayingCompltStatus;
    uint32_t UserFileVersion, UserFileStartAddr, UserFileSize, UserFileCRC;
    uint8_t  UserFileCompltStatus;
    uint32_t ConsumerDataStartAddr, ConsumerDataSize;
    uint16_t PartitionTableChecksum;
} FileConfig_Struct;
#pragma pack()
```

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

// Request packets (host -> device)
void send_req_update_req_packet(void);
void send_req_update_write_packet(uint32_t offset, uint32_t size);
void send_req_update_write_packet_ex(uint32_t index, uint32_t offset, uint32_t size);
void send_req_update_block_write_done_packet(void);

// ACK packets (device -> host)
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

// State functions
int32_t get_update_state(void);
void set_update_complete_status(void);
int32_t check_req_ack(void);
int32_t have_a_new_message(void);
int32_t check_req_recv(void);
```

---

## Usage Example

### Trigger OTA via Application Code

```c
#include "firmware_updater.h"

// Enter OTA update mode (typically triggered by voice command or UART protocol)
void enter_ota_mode(uint32_t baudrate)
{
    firmware_update_main(baudrate);
}
```

### Trigger OTA via Voice Command

```c
// In user message handler:
void handle_ota_command(void)
{
    // Play "entering update mode" prompt
    // ... play prompt ...

    // Wait for prompt to finish
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Enter OTA mode
    firmware_update_main(UART_BaudRate115200);
}
```

### Boot-Time OTA Check

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

### Update Process Flow

```
1. Host sends MSG_CMD_UPDATE_REQ (0x03)
   Device responds with ACK

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

## Notes/Tips

- **Flash erase before write**: Always erase 4KB sectors before writing. The OTA protocol handles this via `MSG_CMD_UPDATE_ERA`.
- **Partition status**: After update, `CompltStatus` is set to `0xFC` (updating) during transfer, then `0xF0` (OK) after successful verify.
- **CRC verification**: Always verify CRC after writing. The `send_ack_update_verify_packet()` sends back the verification result.
- **Bootloader**: Pre-flashed and should never be updated via OTA. It handles the initial firmware reception.
- **Individual partitions**: The OTA protocol supports updating individual partitions (UserCode, ASR model, DNN model, voice prompts) independently.
- **Baud rate**: Use `UART_BaudRate115200` for reliable updates. Higher baud rates may cause CRC errors on long cables.
- **PC-side tool**: Use the Chipintelli OTA tool (GUI or CLI) to send firmware data via UART.
- **source_file.prj**: Ensure `components/ota/firmware_updater.c` is included in the build.
- **Power failure**: The `CompltStatus` field tracks update state. If status is `0xFC` and power is lost, the bootloader can detect the incomplete update and attempt recovery.
