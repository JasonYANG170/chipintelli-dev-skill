# CI230X Memory Layout

> **SDK**: `CI230X_wifi_combo_sdk_release_v1.1.1`
> **Chip**: LN882H (ARM Cortex-M4)
> **Sources**: `project/ci230x-wifi-sdk-combo/cfg/flash_partition_cfg.json`, `cfg/flash_partition_table.h`, `gcc/ln882h.ld`

---

## Flash Partition Layout (4MB SPI Flash)

The flash is divided into 7 partitions. System partitions (BOOT, PART_TAB) are fixed and cannot be modified. User partitions can be adjusted but must remain 4K-aligned with no overlaps.

### Partition Table

| Partition | Type | Start Address | Size | End Address | Description |
|-----------|------|--------------|------|-------------|-------------|
| BOOT | System | `0x00000000` | 24 KB | `0x00006000` | Bootloader (fixed, do not modify) |
| PART_TAB | System | `0x00006000` | 4 KB | `0x00007000` | Partition table (fixed, do not modify) |
| APP | User | `0x00007000` | 1100 KB | `0x0011A000` | Application firmware |
| OTA | User | `0x0011A000` | 672 KB | `0x001C2000` | OTA download/restore area |
| NVDS | User | `0x001C2000` | 12 KB | `0x001C5000` | Non-volatile data storage |
| KV | User | `0x001C5000` | 16 KB | `0x001C9000` | Key-value store |
| USER | User | `0x001C9000` | 220 KB | `0x00200000` | User data (voice models, configs) |
| **Total** | | | **2044 KB** | `0x00200000` | (4MB flash, ~2MB unused) |

### Partition Defines (`flash_partition_table.h`)

```c
#define BOOT_SPACE_OFFSET      (0x00000000)
#define BOOT_SPACE_SIZE        (1024*24)

#define PART_TAB_SPACE_OFFSET  (0x00006000)
#define PART_TAB_SPACE_SIZE    (1024*4)

#define APP_SPACE_OFFSET       (0x00007000)
#define APP_SPACE_SIZE         (1024*1100)

#define OTA_SPACE_OFFSET       (0x0011A000)
#define OTA_SPACE_SIZE         (1024*672)

#define NVDS_SPACE_OFFSET      (0x001C2000)
#define NVDS_SPACE_SIZE        (1024*12)

#define KV_SPACE_OFFSET        (0x001C5000)
#define KV_SPACE_SIZE          (1024*16)

#define USER_SPACE_OFFSET      (0x001C9000)
#define USER_SPACE_SIZE        (1024*220)

#define IMAGE_HEADER_SIZE      (0x100)  // 256 bytes
```

### JSON Configuration (`flash_partition_cfg.json`)

```json
{
    "vendor_define": [
        {"partition_type": "BOOT",     "start_addr": "0x00000000", "size_KB": 24},
        {"partition_type": "PART_TAB", "start_addr": "0x00006000", "size_KB": 4}
    ],
    "user_define": [
        {"partition_type": "APP",  "start_addr": "0x00007000", "size_KB": 1100},
        {"partition_type": "OTA",  "start_addr": "0x0011A000", "size_KB": 672},
        {"partition_type": "NVDS", "start_addr": "0x001C2000", "size_KB": 12},
        {"partition_type": "KV",   "start_addr": "0x001C5000", "size_KB": 16},
        {"partition_type": "USER", "start_addr": "0x001C9000", "size_KB": 220}
    ]
}
```

**Modification rules**:
- System partitions (`vendor_define`): **Never modify**
- User partitions (`user_define`): Can modify `start_addr` and `size`, must be 4K-aligned, must not overlap

---

## RAM Memory Layout (LN882H)

The LN882H has 3 RAM regions defined in the linker script (`gcc/ln882h.ld`):

| Region | Start | Size | End | Usage |
|--------|-------|------|-----|-------|
| RAM0 | `0x20000000` | 295 KB | `0x20049C00` | Main RAM: code, data, BSS, stack, heap, WiFi/BLE buffers |
| RETENTION | `0x20049C00` | 1 KB | `0x2004A000` | Retention RAM (survives deep sleep) |
| CACHE_MEM | `0x2004A000` | 32 KB | `0x20052000` | Cache memory (do not use for application data) |

### Linker Script Memory Regions

```
MEMORY
{
  FLASH     (rx)  : ORIGIN = 0x10007100, LENGTH = 1100K
  RAM0      (rwx) : ORIGIN = 0x20000000, LENGTH = 295K
  RETENTION (rwx) : ORIGIN = 0x20049C00, LENGTH = 1K
  CACHE_MEM (rwx) : ORIGIN = 0x2004A000, LENGTH = 32K
}
```

**Note**: FLASH `ORIGIN = 0x10007100` (not `0x00007000`). The `0x10000000` prefix indicates XIP (eXecute In Place) address space. The `0x100` offset is the image header (`IMAGE_HEADER_SIZE`). The actual flash offset is `0x00007100`.

### RAM0 Section Layout

```
0x20000000  +-----------------------+
            | .flash_copysection    |  Code copied from flash to RAM
            |   - .vectors          |  ARM interrupt vector table
            |   - hal_qspi .text    |  QSPI code (must run from RAM)
            |   - hal_cache .text   |  Cache code (must run from RAM)
            |   - hal_dma .text     |  DMA code (must run from RAM)
            |   - hal_rtc .text     |  RTC code (must run from RAM)
            |   - flash .text       |  Flash code (must run from RAM)
            |   - port .text        |  FreeRTOS port code
            |   - .data             |  Initialized data
            |   - .init_array       |  C++ constructors
            |   - .fini_array       |  C++ destructors
            +-----------------------+
            | .stack_dummy          |  Stack (grows downward)
            |   __stack_start__     |
            |   __c_stack_top__     |
            +-----------------------+
            | .bss_ram0             |  Uninitialized data
            |   - memp .bss         |  LwIP memory pools
            |   - wlan_mem_local    |  WiFi local memory
            |   - wlan_mem_pkt      |  WiFi packet buffers
            |   - wlan_mem_dscr     |  WiFi descriptors
            |   - .bss              |  Application BSS
            |   - COMMON            |  Common symbols
            |   __bss_ram0_end__    |
            +-----------------------+
            | heap0                 |  System heap (FreeRTOS)
            |   heap0_start         |  = __bss_ram0_end__
            |   ...                 |
            |   heap0_end           |  = 0x20049C00 (RAM0 end)
            +-----------------------+
0x20049C00  +-----------------------+
            | RETENTION (1KB)       |  Data preserved across deep sleep
            |   __retention_start__ |
            |   retention_data      |
            |   __retention_end__   |
            +-----------------------+
0x2004A000  +-----------------------+
            | CACHE_MEM (32KB)      |  Hardware cache (do not use)
            +-----------------------+
0x20052000
```

### Key Symbols

```c
// Stack
__StackTop    = __c_stack_top__;    // Top of stack
__StackLimit  = __stack_start__;    // Bottom of stack

// Heap
heap0_start = __bss_ram0_end__;     // Heap starts after BSS
heap0_end   = ORIGIN(RAM0) + LENGTH(RAM0);  // Heap ends at RAM0 boundary
heap0_len   = heap0_end - heap0_start;

// Assertions
ASSERT(heap0_start < heap0_end, "region RAM0 overflowed with .bss");
```

### Flash Section Layout

```
0x00007000  +-----------------------+
            | Image Header (256B)   |  IMAGE_HEADER_SIZE = 0x100
0x00007100  +-----------------------+
            | .flash_text           |  Main code section
            |   - .text             |  All application code
            |   - .rodata           |  Read-only data
            |   - ln_at_cmd_tbl     |  AT command table
            +-----------------------+
            |                       |  Remaining flash for OTA, NVDS, KV, USER
0x0011A000  +-----------------------+
            | OTA partition (672KB) |
            +-----------------------+
            | ...                   |
0x00200000  +-----------------------+
```

---

## CI13xx Voice Chip Flash (Separate)

The CI13xx voice chip has its own flash, separate from LN882H. Its layout follows CI13LC conventions:
- ASR model files
- DNN model files
- Voice prompt files
- User code

The CI13xx flash is accessed via SDIO from the LN882H for audio OTA updates.

---

## Modifying Partitions

### To increase APP size (reduce OTA):

1. Edit `flash_partition_cfg.json`:
   ```json
   {"partition_type": "APP", "start_addr": "0x00007000", "size_KB": 1400},
   {"partition_type": "OTA", "start_addr": "0x001C7000", "size_KB": 372},
   ```

2. Regenerate `flash_partition_table.h` (the build system does this from the JSON)

3. Verify no overlap errors:
   ```c
   #if (OTA_SPACE_OFFSET < (APP_SPACE_OFFSET + APP_SPACE_SIZE))
     #error "flash partition overlap!!!"
   #endif
   ```

4. Update the linker script `LENGTH` to match the new APP size:
   ```
   FLASH (rx) : ORIGIN = 0x10007100, LENGTH = 1400K
   ```

**Rules**:
- All offsets and sizes must be 4K-aligned (multiples of 0x1000)
- No overlaps between partitions
- BOOT and PART_TAB are fixed (vendor-defined)
- Total must not exceed flash size (4MB = 0x400000)
