# CI130X Memory Layout

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Source: `projects/offline_asr_pro_sample/src/ci130x.lds`, `utils/common.lds`, `tools/common.lds`

---

## SRAM Memory Map

CI130X uses on-chip SRAM for code execution, data, heap, and stack. The linker script defines the complete layout.

### SRAM Address Ranges

| Region | Start Address | End Address | Size | Description |
|--------|--------------|-------------|------|-------------|
| Boot Parameter | `0x1FF50A00` | `0x1FF50BFF` | 512B | Boot parameter storage |
| Share SRAM | `0x1FF50C00` | `0x1FF50FFF` | 1KB | Shared memory between cores |
| **SRAM (Host)** | **`0x1FF51000`** | **`0x1FFCF000`** | **~500KB** | Host core SRAM (code, data, heap, stack) |
| SDK Pro SRAM Host End | `0x1FFCF000` | - | - | Host SRAM upper bound (SDK pro) |
| SRAM Host End | `0x1FFD1000` | - | - | Host SRAM upper bound (full) |
| Chip SRAM End | `0x1FFF8000` | - | - | Chip SRAM absolute end |

### Common.lds Defines

```lds
// utils/common.lds
SDK_PRO_SRAM_HOST_END_ADDR = 0x1FFCF000;
SRAM_HOST_END_ADDR         = 0x1FFD1000;
CHIP_SRAM_END_ADDR         = 0x1FFF8000;
CI1310_SRAM_HOST_END_ADDR  = 0x1FFAA000;
CI1310_SRAM_END_ADDR       = 0x1FFD0000;

// tools/common.lds (project-level override)
SRAM_HOST_END_ADDR         = 0x1ffb9000;
```

> **Note**: The project-level `tools/common.lds` overrides `SRAM_HOST_END_ADDR` to `0x1ffb9000`. The `ci130x.lds` includes this file and uses `SDK_PRO_SRAM_HOST_END_ADDR` as the actual SRAM end for the host core.

---

## SRAM Internal Layout

The SRAM region (`0x1FF51000` to `0x1FFCF000`) is organized as follows:

```
+SRAM-----------------+ 0x1FF51000  (SRAM_START_ADDR)
|                     |
|       CODE          |  .init, .text, .fini, .ro_data
|                     |
+---------------------+
|                     |
|       RW            |  .data, .bss, .no_init
|                     |
+---------------------+
|                     |
|       heap          |  FreeRTOS heap (sys_heap) + user heap
|                     |
+---------------------+
|       stack         |  3KB (STACK_SIZE = 0xC00)
+---------------------+  SRAM_END_ADDR
```

### Linker Script Symbols

| Symbol | Value | Description |
|--------|-------|-------------|
| `SRAM_START_ADDR` | `0x1FF51000` | SRAM start address |
| `SRAM_END_ADDR` | `SDK_PRO_SRAM_HOST_END_ADDR` (`0x1FFCF000`) | SRAM end address |
| `SRAM_SIZE` | `SRAM_END_ADDR - SRAM_START_ADDR` | Available SRAM size |
| `SYS_HEAP_SIZE` | `1024 * 60` (60KB) | FreeRTOS system heap size |
| `STACK_SIZE` | `0xC00` (3KB) | Stack size (included in RW) |
| `BOOT_PARAMETER_ADDR` | `0x1FF50A00` | Boot parameter area start |
| `BOOT_PARAMETER_SIZE` | `0x200` (512B) | Boot parameter area size |
| `SHARE_SRAM_ADDR` | `0x1FF50C00` | Shared SRAM area start |
| `SHARE_SRAM_SIZE` | `0x400` (1KB) | Shared SRAM area size |

---

## Memory Sections (from ci130x.lds)

### Special Sections

| Section | Memory Region | Description |
|---------|--------------|-------------|
| `.boot_parameter` (NOLOAD) | BOOT_PARAMETER | Boot parameter storage (not initialized) |
| `.share_memory` (NOLOAD) | SHARE_SRAM | Inter-core shared memory (not initialized) |
| `.no_init` (NOLOAD) | SRAM | Uninitialized data (survives reset) |

### Code and Data Sections

| Section | Description |
|---------|-------------|
| `.init` | Init code, vtable, load address |
| `.text` | Executable code |
| `.fini` | Finalization code |
| `.ro_data` | Read-only data (constants, strings) |
| `.data` | Initialized data |
| `.bss` | Zero-initialized data |
| `.preinit_array` | Pre-init function array |
| `.init_array` | Init function array |
| `.fini_array` | Finalization function array |
| `.ctors` | C++ constructors |
| `.dtors` | C++ destructors |

### Stack and Heap

| Section | Description |
|---------|-------------|
| `.stack` | Stack area (3KB), provides `_sp` symbol |
| `.sys_heap` | FreeRTOS system heap (60KB), provides `__FREERTOSHEAP_START` and `__FREERTOSHEAP_END` |
| `.heap_start` | User heap start, provides `heap_start` |
| `.heap_end` | User heap end at SRAM_END, provides `heap_end` and `__befor_os_int_sp` |

### Exported Symbols

```lds
// Stack
__stack_size     = STACK_SIZE;     // 0xC00 (3KB)
__int_stack_size = STACK_SIZE;     // 0xC00 (3KB)

// Heap
__FREERTOSHEAP_START  // Start of FreeRTOS heap
__FREERTOSHEAP_END    // End of FreeRTOS heap
heap_start            // Start of user heap
heap_end              // End of user heap (= SRAM_END_ADDR)
__befor_os_int_sp     // OS interrupt stack pointer

// Load configuration
LOAD_TYPE     = 0x0;   // 0=RAM mode, 1=TCM mode, 2=cache mode
ITCM_EN       = 0x0;   // ITCM disabled
STCM_EN       = 0x0;   // STCM disabled
ICACHE_EN     = 0x0;   // ICACHE disabled
SCACHE_EN     = 0x0;   // SCACHE disabled
```

---

## DMA Addressable Memory Regions

The DMA controller can access these memory regions (from `ci130x_dma.h`):

| Region | Address | Size | Description |
|--------|---------|------|-------------|
| SDRAM | `0x70000000` | 16MB | External SDRAM |
| CSRAM | `0x1FFF8000` | 32KB | Chip SRAM (cache) |
| SRAM0 | `0x1FFE8000` | 64KB | SRAM block 0 |
| SRAM1 | `0x20000000` | 64KB | SRAM block 1 |
| PCMRAM | `0x20020000` | 16KB | PCM RAM (audio) |
| FFTRAM | `0x200FF800` | 2KB | FFT RAM |

---

## ICACHE TCM Region

From `ci130x_cache.h`:

| Symbol | Address | Description |
|--------|---------|-------------|
| `ICACHE_TCM_S` | `0x1FFA8000` | ICACHE TCM start |
| `ICACHE_TCM_E` | `0x1FFAFFFF` | ICACHE TCM end |
| `ICACHE_TCM_A` | `0x1FBB0000` | ICACHE TCM actual address (alias) |

---

## Flash Memory Layout

The SPIFlash stores the firmware image, ASR models, voice prompts, and user data. The flash layout is managed by the firmware pack tool (`ci-tool-kit merge user-file`).

### Flash Partition Structure

| Partition | Start Address | Max Size | Description |
|-----------|--------------|----------|-------------|
| Bootloader | `0x00000000` | ~32KB | Bootloader (pre-flashed, not updated via OTA) |
| File Config | `0x00008000` | 4KB | Partition table (`FileConfig_Struct`) |
| User Code 1 | (from FileConfig) | ~448KB | Application firmware (host core) |
| User Code 2 | (from FileConfig) | varies | Secondary code (nuclear core / backup) |
| ASR CMD Model | (from FileConfig) | varies | ASR command word model |
| DNN Model | (from FileConfig) | varies | DNN model data |
| Voice Playing | (from FileConfig) | varies | Voice prompt audio files |
| User File | (from FileConfig) | varies | User data files (cmd_info, IR data, etc.) |
| Consumer Data | (from FileConfig) | varies | User application data |

### FileConfig Structure (from flash_update.h)

```c
#pragma pack(1)
typedef struct {
    uint32_t ManufacturerID;
    uint32_t ProductID[2];        // 64-bit (MAC Address)
    uint32_t HWName[16];          // Hardware name string
    uint32_t HWVersion;
    uint32_t SWName[16];          // Software name string
    uint32_t SWVersion;
    uint32_t BootLoaderVersion;
    uint8_t  Reserve[14];

    uint32_t UserCode1Version;
    uint32_t UserCode1StartAddr;
    uint32_t UserCode1Size;
    uint32_t UserCode1CRC;
    uint8_t  UserCode1CompltStatus;   // 0xF0=OK, 0xFC=Update, 0xC0=Old

    uint32_t UserCode2Version;
    uint32_t UserCode2StartAddr;
    uint32_t UserCode2Size;
    uint32_t UserCode2CRC;
    uint8_t  UserCode2CompltStatus;

    uint32_t ASRCMDModelVersion;
    uint32_t ASRCMDModelStartAddr;
    uint32_t ASRCMDModelSize;
    uint32_t ASRCMDModelCRC;
    uint8_t  ASRCMDModelCompltStatus;

    uint32_t DNNModelVersion;
    uint32_t DNNModelStartAddr;
    uint32_t DNNModelSize;
    uint32_t DNNModelCRC;
    uint8_t  DNNModelCompltStatus;

    uint32_t VoicePlayingVersion;
    uint32_t VoicePlayingStartAddr;
    uint32_t VoicePlayingSize;
    uint32_t VoicePlayingCRC;
    uint8_t  VoicePlayingCompltStatus;

    uint32_t UserFileVersion;
    uint32_t UserFileStartAddr;
    uint32_t UserFileSize;
    uint32_t UserFileCRC;
    uint8_t  UserFileCompltStatus;

    uint32_t ConsumerDataStartAddr;
    uint32_t ConsumerDataSize;

    uint16_t PartitionTableChecksum;
} FileConfig_Struct;
#pragma pack()
```

### User Code Area Status Values

| Value | Name | Description |
|-------|------|-------------|
| `0xF0` | `USER_CODE_AREA_STA_OK` | Code is valid and current |
| `0xFC` | `USER_CODE_AREA_STA_UPDATE` | Code is being updated |
| `0xC0` | `USER_CODE_AREA_STA_OLD` | Code is old/backup version |

### Flash Size by Chip Type

| Chip Type | Flash Size | Typical Use |
|-----------|-----------|-------------|
| CI1302 | 2MB | SSOP24, standard offline ASR |
| CI1306 | 4MB | QFN40, larger models + voice files |
| CI1312 | 2MB | SSOP16, low-cost |

---

## Heap Usage Verification

The SDK prints heap information at boot and in the main loop:

```c
// From main.c task_init():
extern char heap_start;
extern char heap_end;
mprintf("Heap size:%dKB\n", (((uint32_t)&heap_end) - ((uint32_t)&heap_start))/1024);

// Runtime monitoring:
mprintf("asr heap min free:%dKB\n", get_heap_bytes_remaining_size()/1024);
mprintf("system heap min free:%dKB\n", xPortGetMinimumEverFreeHeapSize()/1024);
mprintf("system heap free:%dKB\n", xPortGetFreeHeapSize()/1024);
```

---

## Adjusting Memory Layout

### Increasing Heap Size

```lds
/* In ci130x.lds */
SYS_HEAP_SIZE = (1024 * 80);  /* Increase from 60KB to 80KB */
```

### Increasing Stack Size

```lds
/* In ci130x.lds */
STACK_SIZE = 0x1000;  /* Increase from 3KB to 4KB */
```

### Enabling Cache (TCM mode)

```lds
/* In ci130x.lds */
LOAD_TYPE   = 0x2;     /* Switch to cache mode */
ICACHE_EN   = 0x1;     /* Enable ICACHE */
ICACHE_START_ADDR = 0x1FF51000;  /* Set cache range */
ICACHE_END_ADDR   = 0x1FFA8000;
ICACHE_ALIAS_ADDR = 0x1FBB0000;
```

> **Warning**: Enabling cache changes the memory map. Test thoroughly after changing cache settings.
