# CI13LC Memory Layout

## SRAM Layout (CI1316x/CI1324x/CI1332x)

The CI13LC series shares the same Nuclei N300 core as CI130X, with similar memory mapping.

### SRAM Regions

| Region | Start Address | Size | Description |
|---|---|---|---|
| Boot Parameter | 0x1FF50A00 | 0x200 | Boot parameters |
| Share SRAM | 0x1FF50C00 | 0x400 | Inter-core shared SRAM |
| SRAM (host) | 0x1FF51000 | Variable | Host core code + data |
| SRAM (nuclear) | - | - | Nuclear core code + data |
| Stack | Top of SRAM | 0xC00 | Main stack (included in RW) |

### Heap Configuration

`SYS_HEAP_SIZE` in the linker script (`.lds`) controls the FreeRTOS heap:

```
SYS_HEAP_SIZE = (1024*60);  // 60KB default
```

Adjust based on task count and buffer sizes. Use `xPortGetFreeHeapSize()` to verify.

### Flash Layout

| Partition | Offset | Size | Content |
|---|---|---|---|
| Bootloader | 0x000000 | ~16KB | Boot code (ROM) |
| User Code | ~0x004000 | Variable | Application firmware |
| ASR Model | Variable | Variable | ASR + DNN models |
| Voice Prompts | Variable | Variable | MP3/prompt audio |
| User Data | Variable | Variable | NVDM data, CWSL data |

Flash size depends on chip:
- CI1312: 2MB
- CI13242: 4MB
- CI13322: 4MB

### Linker Script Structure

The CI13LC linker script (`ci13lc.lds`) follows the same pattern as CI130X:
- `INCLUDE common.lds` for shared definitions
- `MEMORY` block defines SRAM regions
- `STACK_SIZE = 0xC00` (3KB, included in RW)
- `SYS_HEAP_SIZE` adjustable
- `OUTPUT_ARCH("riscv")` + `ENTRY(_start)`

### Chip-Specific SRAM End Addresses

| Chip | SDK_PRO_SRAM_HOST_END_ADDR | SRAM_HOST_END_ADDR |
|---|---|---|
| CI1310 | 0x1FFAA000 | 0x1FFD0000 |
| Standard | 0x1FFCF000 | 0x1FFD1000 |
| Full | - | 0x1FFF8000 |
