# Recipe: SPI Flash Read/Write/Erase

> SDK: `CI130X_SDK_Offline_V2.1.14`
> Chips: CI1302, CI1303, CI1306, CI1312

> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/spiflash.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

The CI130X connects to external SPI flash via the QSPI0 controller. The SDK provides high-level `flash_read`/`flash_write`/`flash_erase` functions as well as low-level `spic_*` functions for direct flash command access.

---

## SDK

`CI130X_SDK_Offline_V2.1.14` - driver: `ci130x_chip_driver`

---

## Source Anchors

- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_spiflash.h`

---

## API Usage

### Controller and Types

```c
typedef enum {
    QSPI0 = HAL_DTRFLASH_BASE,
} spic_base_t;
```

### Flash Commands (spic_cmd_code_t)

Key command codes from the enum:

```c
SPIC_CMD_CODE_WRITE_ENABLE              = 0x06,  // Write enable
SPIC_CMD_CODE_PAGEPROGRAM               = 0x02,  // Page program (256 bytes max)
SPIC_CMD_CODE_SECTORERASE4K             = 0x20,  // 4KB sector erase
SPIC_CMD_CODE_BLOCKERASE32K             = 0x52,  // 32KB block erase
SPIC_CMD_CODE_BLOCKERASE64K             = 0xd8,  // 64KB block erase
SPIC_CMD_CODE_CHIPERASE                 = 0xc7,  // Chip erase
SPIC_CMD_CODE_READDATA                  = 0x03,  // Read data
SPIC_CMD_CODE_FASTREAD                  = 0x0b,  // Fast read
SPIC_CMD_CODE_READJEDECID               = 0x9F,  // Read JEDEC ID
SPIC_CMD_CODE_READ_UNIQUE_ID            = 0x4b,  // Read unique ID
```

### High-Level Functions

```c
// Initialize flash controller
int32_t flash_init(spic_base_t spic);

// Read data from flash into SRAM buffer
// buf: SRAM destination address, addr: flash address, size: bytes
int32_t flash_read(spic_base_t spic, uint32_t buf, uint32_t addr, uint32_t size);

// Write data from SRAM buffer to flash (page program)
// addr: flash address, buf: SRAM source address, size: bytes
int32_t flash_write(spic_base_t spic, uint32_t addr, uint32_t buf, uint32_t size);

// Erase flash region (auto-selects 4K/32K/64K based on alignment)
int32_t flash_erase(spic_base_t spic, uint32_t addr, uint32_t size);

// Set flash clock divider
int32_t flash_clk_div_init(spic_base_t spic);

// Configure XIP (Execute-In-Place) mode
int32_t spic_xipconfig(spic_base_t spic);
```

### ID and Security Functions

```c
int32_t spic_read_unique_id(spic_base_t spic, uint8_t* unique);    // 16-byte unique ID
int32_t spic_read_jedec_id(spic_base_t spic, uint8_t* jedec);      // JEDEC ID

int32_t spic_erase_security_reg(spic_base_t spic, spic_security_reg_t reg);
int32_t spic_write_security_reg(spic_base_t spic, spic_security_reg_t reg,
                                 uint32_t buf, uint32_t addr, uint32_t size);
int32_t spic_read_security_reg(spic_base_t spic, spic_security_reg_t reg,
                                uint32_t buf, uint32_t addr, uint32_t size);
int32_t spic_security_reg_lock(spic_base_t spic, spic_security_reg_t reg);
```

### Low-Level Functions

```c
// Direct erase with specific command code
int32_t spic_erase(spic_base_t spic, spic_cmd_code_t code, uint32_t addr);

// Enable/disable flash write protection
int32_t spic_protect(spic_base_t spic, FunctionalState cmd);

// Enable quad mode
int32_t spic_quad_mode(spic_base_t spic);

// Reset flash device
int32_t spic_reset(spic_base_t spic);
```

### DNN Mode Functions

```c
// Configure DNN mode address range
int32_t dnn_mode_config(spic_base_t spic, uint32_t start_addr, uint32_t size);

// Enter/exit DNN mode (direct flash access for neural network)
int32_t flash_dnn_mode(spic_base_t spic, FunctionalState cmd);

// Check if currently in DNN mode
int32_t flash_is_dnn_mode(spic_base_t spic);
uint32_t flash_check_mode(spic_base_t spic);
```

---

## Usage Example

### Basic Flash Read/Write/Erase

```c
#include "ci130x_spiflash.h"
#include "ci130x_scu.h"

#define USER_FLASH_ADDR  0x300000  // 3MB offset (must not overlap firmware)

void flash_rw_test(void)
{
    // Initialize flash
    flash_init(QSPI0);

    // Read JEDEC ID
    uint8_t jedec[3];
    spic_read_jedec_id(QSPI0, jedec);
    // jedec[0] = manufacturer ID, jedec[1] = memory type, jedec[2] = capacity

    // Read unique ID (16 bytes)
    uint8_t unique_id[16];
    spic_read_unique_id(QSPI0, unique_id);

    // Erase a 4KB sector before writing
    flash_erase(QSPI0, USER_FLASH_ADDR, 4096);

    // Write data (note: buf is SRAM address cast to uint32_t)
    uint8_t write_buf[256] = "Hello Flash";
    flash_write(QSPI0, USER_FLASH_ADDR, (uint32_t)write_buf, sizeof(write_buf));

    // Read back
    uint8_t read_buf[256] = {0};
    flash_read(QSPI0, (uint32_t)read_buf, USER_FLASH_ADDR, sizeof(read_buf));
}
```

### Store User Data in Flash

```c
// Write a struct to flash at a user-defined address
typedef struct {
    uint32_t magic;
    uint32_t config_value;
    uint32_t crc;
} user_config_t;

void save_config_to_flash(user_config_t *cfg)
{
    // Erase 4KB sector (minimum erase unit)
    flash_erase(QSPI0, USER_FLASH_ADDR, 4096);

    // Write config struct
    flash_write(QSPI0, USER_FLASH_ADDR, (uint32_t)cfg, sizeof(user_config_t));
}

void load_config_from_flash(user_config_t *cfg)
{
    flash_read(QSPI0, (uint32_t)cfg, USER_FLASH_ADDR, sizeof(user_config_t));
}
```

### Read Security Register

```c
void read_security_reg_demo(void)
{
    uint8_t buf[256] = {0};
    spic_read_security_reg(QSPI0, SPIC_SECURITY_REG1,
                           (uint32_t)buf, 0, sizeof(buf));
}
```

---

## Notes/Tips

- **Always erase before writing**: Flash must be erased (set to 0xFF) before programming. Erase minimum unit is 4KB (sector).
- **Page program limit**: `flash_write` uses page program (0x02) internally; a single page program command writes at most 256 bytes. The SDK `flash_write` handles multi-page writes automatically.
- **Address conflicts**: Do not write to addresses used by firmware, ASR models, or voice data. Use addresses above the firmware partition (check `ci_flash_data_info.h` for partition layout).
- **DNN mode**: When ASR is running, the flash may be in DNN mode. Use `flash_is_dnn_mode()` to check before accessing flash directly. The `flash_control` module manages flash access arbitration.
- **Write protection**: Call `spic_protect(QSPI0, DISABLE)` before erasing if the flash has hardware protection enabled. Re-enable after: `spic_protect(QSPI0, ENABLE)`.
- **XIP**: The code executes directly from flash (XIP mode). Flash operations that change the controller mode may disrupt code execution; avoid flash operations in critical interrupt contexts.
