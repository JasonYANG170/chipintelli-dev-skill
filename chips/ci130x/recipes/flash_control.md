# Recipe: Flash Data Management (flash_control)

> SDK: `CI130X_SDK_Offline_V2.1.14`
> Chips: CI1302, CI1303, CI1306, CI1312

> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/flash_control.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

The `flash_control` component manages the flash partition table and model data. It provides `ci_flash_data_info` for initializing the partition table, locating ASR/DNN/voice model addresses, and switching between model groups (e.g., wakeup vs. command models). It also handles inter-core flash access arbitration between the host and DSP cores.

---

## SDK

`CI130X_SDK_Offline_V2.1.14` - component: `flash_control`

---

## Source Anchors

- `ci130x_sdk/components/flash_control/flash_control_inc/ci_flash_data_info.h` - partition table, model address queries
- `ci130x_sdk/components/flash_control/flash_control_inc/flash_control_inner_port.h` - inter-core flash access port
- `ci130x_sdk/components/flash_control/flash_control_inc/flash_rw_process.h` - flash read/write arbitration (commented out in V2.1.14)
- `ci130x_sdk/components/flash_control/flash_control_outside/flash_control_outside_port.h` - outside-facing port

---

## API Usage

### Partition Table Structure

```c
#pragma pack(1)
typedef struct {
    uint32_t manu_facturer_id;
    uint32_t product_id[2];             // MAC Address
    uint32_t hard_ware_name[16];
    uint32_t hard_ware_version;
    uint32_t soft_ware_name[16];
    uint32_t soft_ware_version;
    uint32_t bootloader_version;
    char     ChipName[9];
    uint8_t  FirmwareFormatVer;
    uint8_t  reserve[4];
    // Partition entries (version, offset, size, crc, status per partition):
    uint32_t user_code1_version, user_code1_offset, user_code1_size, user_code1_crc;
    uint8_t  user_code1_status;
    uint32_t user_code2_version, user_code2_offset, user_code2_size, user_code2_crc;
    uint8_t  user_code2_status;
    uint32_t asr_cmd_model_version, asr_cmd_model_offset, asr_cmd_model_size, asr_cmd_model_crc;
    uint8_t  asr_cmd_model_status;
    uint32_t dnn_model_version, dnn_model_offset, dnn_model_size, dnn_model_crc;
    uint8_t  dnn_model_status;
    uint32_t voice_version, voice_offset, voice_size, voice_crc;
    uint8_t  voice_status;
    uint32_t user_file_version, user_file_offset, user_file_size, user_file_crc;
    uint8_t  user_file_status;
    uint32_t nv_data_offset, nv_data_size;
    uint16_t patitiontablechecksum;
} partition_table_t;
#pragma pack()
```

### File and Group Structures

```c
typedef struct {
    uint16_t file_id;
    uint32_t file_addr;
    uint32_t file_size;
} file_header_t;

typedef struct {
    uint16_t file_number;
    file_header_t file_header[1];  // Variable-length array
} file_table_t;

typedef struct {
    uint16_t group_id;
    uint32_t group_addr;
} group_header_t;

typedef struct {
    uint16_t group_number;
    group_header_t group_header[1];  // Variable-length array
} group_table_t;
```

### Key Constants

```c
#define FILECONFIG_SPIFLASH_START_ADDR  (0x2000)  // Partition table base address
#define COMMAND_INFO_FILE_ID            60000
#define VOICE_PRINT_DNN_ID              60001
```

### Initialization and Model Queries

```c
// Initialize flash data info with default model group
// default_model_group_id: 0=command model, 1=wakeup model (typical)
uint32_t ci_flash_data_info_init(uint8_t default_model_group_id);

// Get current model addresses (DNN and ASR)
uint32_t get_current_model_addr(uint32_t *p_dnn_addr, uint32_t *p_dnn_size,
                                 uint32_t *p_asr_addr, uint32_t *p_asr_size);

// Get DNN model address by file ID
uint32_t get_dnn_addr_by_id(uint16_t dnn_file_id, uint32_t *p_dnn_addr, uint32_t *p_dnn_size);

// Get ASR model address by ID
uint32_t get_asr_addr_by_id(int asr_id, uint32_t *p_asr_addr, uint32_t *p_asr_size);

// Get voice prompt address by voice IDs
uint32_t get_voice_addr_by_id(uint16_t *voice_id_buffer, uint32_t *voice_addr_buffer, uint32_t voice_num);

// Get user file address by file ID
uint32_t get_userfile_addr(uint16_t file_id, uint32_t *p_file_addr, uint32_t *p_file_size);
```

### Group and File Access

```c
// Get group address by group ID within a partition
uint32_t get_group_addr(uint32_t partition_addr, uint16_t group_id);

// Get file address within a group
uint32_t get_file_addr(uint32_t group_addr, uint16_t file_id, uint32_t *p_file_addr, uint32_t *p_file_size);

// Get file table from a group (allocates memory; must release after use)
file_table_t * get_file_table(uint32_t group_addr);

// Release file table memory
void release_file_table(file_table_t * p_file_table);
```

### Partition Table and Version

```c
// Get pointer to the partition table
partition_table_t * get_partition_table(void);

// Get firmware version
int32_t get_fw_version(product_version_t *product_version);

// Check if ci_flash_data_info has been initialized
void is_ci_flash_data_info_inited(bool* state);
void set_ci_flash_data_info_init_flag(void);
```

### Cached Flash Reader

```c
// Initialize a cached flash reader at a given start address
uint32_t cached_flash_reader_init(uint32_t start_addr_in_flash);

// Read from cached flash reader
uint32_t cached_flash_reader_read(int32_t read_offset, uint8_t *read_buffer, uint32_t read_length);

// Destroy cached flash reader
uint32_t cached_flash_reader_destroy(void);
```

### Flash Control Port (Inter-Core)

```c
// Initialize the flash control inter-core communication port
void flash_control_inner_port_init(void);
```

---

## Usage Example

### Standard Initialization (in main.c task_init)

```c
#include "ci_flash_data_info.h"
#include "flash_control_inner_port.h"

// In task_init():
void task_init(void *p_arg)
{
    // ... dual-core init, codec init ...

    // Initialize flash control inter-core port
    flash_control_inner_port_init();

    // Initialize flash data info with default model group
    // DEFAULT_MODEL_GROUP_ID is defined in user_config.h
    // 0 = command model, 1 = wakeup model (when USE_SEPARATE_WAKEUP_EN=1)
    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);

    // ... rest of initialization ...
}
```

### Query Current Model Addresses

```c
void print_model_info(void)
{
    uint32_t dnn_addr, dnn_size, asr_addr, asr_size;
    get_current_model_addr(&dnn_addr, &dnn_size, &asr_addr, &asr_size);
    mprintf("DNN: addr=0x%x, size=%d\n", dnn_addr, dnn_size);
    mprintf("ASR: addr=0x%x, size=%d\n", asr_addr, asr_size);
}
```

### Access User File Data

```c
void read_user_file(uint16_t file_id)
{
    uint32_t file_addr, file_size;
    if (get_userfile_addr(file_id, &file_addr, &file_size) == 0)
    {
        // Read file data from flash
        uint8_t *buf = pvPortMalloc(file_size);
        if (buf)
        {
            flash_read(QSPI0, (uint32_t)buf, file_addr, file_size);
            // Process data...
            vPortFree(buf);
        }
    }
}
```

### Get Firmware Version

```c
void print_fw_version(void)
{
    product_version_t ver;
    if (get_fw_version(&ver) == 0)
    {
        mprintf("HW: 0x%x, SW: 0x%x\n", ver.hard_ware_version, ver.soft_ware_version);
    }
}
```

### Model Switching (Wakeup -> Command)

Model switching is handled through the system message queue, not by direct calls to `ci_flash_data_info`:

```c
#include "system_msg_deal.h"

// Switch from wakeup model to command model (after wakeup word recognized)
void switch_to_command_model(void)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
}

// Switch back to wakeup model (on exit wakeup timeout)
void switch_to_wakeup_model(void)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
}
```

---

## Notes/Tips

- `ci_flash_data_info_init()` must be called before any ASR/player initialization, as it sets up the model address mapping.
- The partition table is stored at `FILECONFIG_SPIFLASH_START_ADDR` (0x2000 for non-OTA, 0x6000 when `CI_OTA_ENABLE=1`).
- `get_file_table()` allocates memory from the system heap; always call `release_file_table()` to avoid leaks.
- Model group switching is serialized through the system message queue (`send_msg_to_sys_task`) to avoid race conditions with the ASR engine.
- The `flash_control_inner_port` manages inter-core communication so that flash access by the host core does not conflict with DNN model access on the DSP core.
- When `USE_SEPARATE_WAKEUP_EN=1`, group 0 contains command words and group 1 contains the wakeup word. `DEFAULT_MODEL_GROUP_ID` controls which model loads at boot.
- The partition table checksum (`patitiontablechecksum`) protects against corruption; verify it after any OTA update.
