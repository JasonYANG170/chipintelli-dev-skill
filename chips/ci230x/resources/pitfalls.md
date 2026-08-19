# CI230X Pitfalls

> **SDK**: `CI230X_wifi_combo_sdk_release_v1.1.1`
> **Chips**: CI2305, CI2306 (LN882H + CI13xx dual-chip)

CI230X is a dual-chip solution with unique pitfalls not found in single-chip families. The CI13xx voice chip portion follows CI13LC pitfalls (see `chips/ci13lc/resources/pitfalls.md`). This document covers LN882H and dual-chip specific issues.

---

## Dual-Chip Communication

### 1. SDIO Communication Failure Between CI13xx and LN882H

**Problem**: The CI13xx (voice) and LN882H (Wi-Fi) communicate via SDIO. If SDIO pins are misconfigured, clock is wrong, or the CI13xx firmware doesn't match the SDIO protocol version, communication fails silently.

**Symptoms**: Wi-Fi doesn't connect, voice commands don't reach the cloud, no ASR results forwarded to LN882H.

**Fix**:
- Verify SDIO pin connections on the hardware design
- Check SDIO clock configuration in both chips
- Ensure CI13xx firmware supports the SDIO communication protocol expected by LN882H
- The `hal_sdio_device` and `hal_sdio_room` HAL modules handle the LN882H side
- Use UART log on both chips to verify SDIO handshake

### 2. CI13xx Firmware Version Mismatch

**Problem**: The CI13xx voice chip runs its own firmware (separate from LN882H). If the CI13xx firmware version doesn't match the LN882H SDK version, the SDIO message format may differ.

**Fix**: Always use matched firmware versions. The CI230X SDK release includes both the LN882H SDK and the matching CI13xx voice firmware. Do not mix versions.

### 3. Boot Sequence Dependency

**Problem**: The LN882H must wait for the CI13xx to be ready before attempting SDIO communication. If LN882H boots faster and sends SDIO commands before CI13xx is initialized, the commands are lost.

**Fix**: The SDK handles this with a boot synchronization mechanism. Do not remove or bypass the boot sync delay. If customizing boot, ensure SDIO init happens after CI13xx is confirmed ready.

---

## Build System (CMake)

### 4. Wrong Toolchain Selection

**Problem**: CI230X uses ARM GCC (`arm-none-eabi-gcc`), NOT RISC-V GCC (`riscv-nuclei-elf-gcc`). Using the wrong toolchain produces cryptic linker errors.

**Fix**: Set `CROSS_TOOLCHAIN_ROOT` environment variable to the ARM GCC installation:
```bash
export CROSS_TOOLCHAIN_ROOT=/path/to/arm-none-eabi-gcc
```

The CMake toolchain setup (`gcc/gcc-toolchain-setup.cmake`) reads this variable:
```cmake
set(CROSS_COMPILE_PREFIX ${COMPILER_HOME}/bin/arm-none-eabi-)
```

If `CROSS_TOOLCHAIN_ROOT` is not set, CMake fails with:
```
CMake Error: CROSS_TOOLCHAIN_ROOT must be set!!!
```

### 5. CMake Build Directory Must Be Separate

**Problem**: Running `cmake .` in the source directory pollutes the source tree and causes rebuild issues.

**Fix**: Always use a separate build directory:
```bash
cmake -B build-ci230x-wifi-sdk-combo-release
cmake --build build-ci230x-wifi-sdk-combo-release
```

### 6. CMake Option Conflicts

**Problem**: Multiple cloud platform options (`CIAS_IOT_CLOUD_HISENSE_ENABLE`, `CIAS_IOT_TUYA_ENABLE`, `CIAS_IOT_TVS_ENABLE`, etc.) are mutually exclusive. Enabling more than one causes symbol conflicts and build errors.

**Fix**: Only enable ONE cloud platform at a time in `project/ci230x-wifi-sdk-combo/CMakeLists.txt`:
```cmake
set(CIAS_IOT_TVS_ENABLE         1)   # Tencent TVS
set(CIAS_IOT_CLOUD_HISENSE_ENABLE 0)  # Hisense (off)
set(CIAS_IOT_TUYA_ENABLE        0)   # Tuya (off)
```

The default config has TVS enabled and others disabled. Each cloud platform uses different BLE config paths and user app paths.

### 7. Tuya Build Requires Product Key

**Problem**: When `CIAS_IOT_TUYA_ENABLE=1`, the build requires a Tuya product key and version. Without it, CMake fails:
```
ERROR: unknown user config: product_XXX_1.0.0
```

**Fix**: Set the product key in CMakeLists.txt:
```cmake
set(APP_NAME     "cloud_tuya")
set(PRODECT_KEY  "your_product_key_here")
set(PRODUCT_VER  "1.0.0")
```

---

## Flash Partition

### 8. Partition Table Mismatch

**Problem**: `flash_partition_cfg.json` and `flash_partition_table.h` must match. The JSON defines partitions for the build tool; the header defines them for the firmware. Changing one without the other causes OTA failures or boot corruption.

**Fix**: When modifying partitions, update BOTH:
- `project/ci230x-wifi-sdk-combo/cfg/flash_partition_cfg.json` (build tool)
- `project/ci230x-wifi-sdk-combo/cfg/flash_partition_table.h` (firmware)

The header has compile-time overlap checks:
```c
#if (APP_SPACE_OFFSET < (PART_TAB_SPACE_OFFSET + PART_TAB_SPACE_SIZE))
  #error "flash partition overlap!!!"
#endif
```

### 9. OTA Partition Size Insufficient

**Problem**: The OTA partition (672 KB) must be large enough to hold the new firmware image. If the application grows beyond 672 KB compressed, OTA fails.

**Fix**: Either:
- Reduce application size (disable unused cloud platforms)
- Adjust partition sizes in `flash_partition_cfg.json` (ensure 4K alignment and no overlaps)
- The APP partition (1100 KB) and OTA partition (672 KB) must both fit in flash

### 10. Flash Erase Alignment

**Problem**: `hal_flash_erase()` requires 4K-aligned offset and length. Passing unaligned values causes undefined behavior.

**Fix**: Always align erase operations to 4K boundaries:
```c
hal_flash_erase(aligned_offset, aligned_length);  // Both must be multiples of 4096
```

---

## Memory Layout

### 11. Linker Script FLASH Origin Includes Image Header

**Problem**: The linker script sets `FLASH ORIGIN = 0x10007100`, not `0x00007000` (the APP partition offset). The 0x100 bytes is the image header (`IMAGE_HEADER_SIZE`).

**Fix**: This is by design. The image header (256 bytes) precedes the actual firmware. When flashing, the tool writes the header + firmware starting at `0x00007000`. Do not change the linker FLASH origin.

### 12. RAM0 Shared with WiFi/BLE

**Problem**: RAM0 (295 KB at 0x20000000) is shared between application code, WiFi library, and BLE stack. The WiFi library uses significant RAM for packet buffers (`wlan_mem_local`, `wlan_mem_pkt`, `wlan_mem_dscr`).

**Fix**: 
- Check heap with `xPortGetFreeHeapSize()` after WiFi init
- The `.bss_ram0` section in the linker script includes WiFi/BLE buffers
- Application heap starts after BSS: `heap0_start = __bss_ram0_end__`
- If heap is insufficient, reduce WiFi buffer count or disable unused features

### 13. TIMER3 Reserved for WiFi

**Problem**: `TIMER3_BASE` is used internally by the WiFi library. Using it in application code causes WiFi malfunction.

**Fix**: Only use `TIMER0_BASE`, `TIMER1_BASE`, `TIMER2_BASE` in application code.

### 14. Cache Memory Region

**Problem**: `CACHE_MEM` (32 KB at 0x2004A000) is used for cache. Using this region for application data causes cache corruption and crashes.

**Fix**: Do not place any application data in the CACHE_MEM region. The linker script handles this automatically.

---

## BLE Configuration

### 15. BLE App Path Must Match Cloud Platform

**Problem**: Different cloud platforms use different BLE app implementations. If `BLE_USR_APP_PATH` doesn't match the selected cloud, BLE pairing fails.

**Fix**: The CMakeLists.txt sets the correct path automatically:
- Default (TVS/CI): `app/ble_usr_ln`
- Hisense: `app/ble_usr_hisense`
- Tuya: `app/ble_usr_tuya`

Do not override `BLE_USR_APP_PATH` manually unless you know the implications.

### 16. BLE Config Enable Requires Correct Port File

**Problem**: `CIAS_BLE_CONFIG_ENABLE=1` includes `cias_ble_config.c`. For Hisense, it must use `hisense_ble_config.c` instead.

**Fix**: The CMakeLists.txt handles this:
```cmake
if(CIAS_IOT_CLOUD_HISENSE_ENABLE)
    file(GLOB_RECURSE CIAS_BLE_CONFIG_PORT_SRC app/cias_aiot_wifi/cias_ble_port/hisense_ble_config.c)
else()
    file(GLOB_RECURSE CIAS_BLE_CONFIG_PORT_SRC app/cias_aiot_wifi/cias_ble_port/cias_ble_config.c)
endif()
```

---

## OTA

### 17. OTA State Machine Must Complete

**Problem**: The OTA process has multiple states (DOWNLOAD_ING -> DOWNLOAD_OK -> RESTORE_ING -> RESTORE_OK -> REPORT_OK). Interrupting at any stage leaves the device in an inconsistent state.

**Fix**: Follow the OTA state machine carefully:
```c
typedef enum {
    UPG_STATE_DOWNLOAD_ING  = 0,
    UPG_STATE_DOWNLOAD_OK   = 1,
    UPG_STATE_RESTORE_ING   = 2,
    UPG_STATE_RESTORE_OK    = 3,
    UPG_STATE_RESTORE_FILED = 4,
    UPG_STATE_REPORT_OK     = 5,
} upg_state_t;
```

If download succeeds but restore fails (`UPG_STATE_RESTORE_FILED`), the device must report failure and retry.

### 18. Audio OTA vs WiFi OTA

**Problem**: The SDK supports both audio chip OTA (`CIAS_AIOT_AUDIO_OTA_ENABLE`) and WiFi chip OTA (`CIAS_AIOT_WIFI_OTA_ENABLE`). Enabling the wrong one wastes flash and may cause conflicts.

**Fix**: 
- `CIAS_AIOT_AUDIO_OTA_ENABLE`: Updates the CI13xx voice chip firmware via LN882H
- `CIAS_AIOT_WIFI_OTA_ENABLE`: Updates the LN882H firmware itself
- Enable only the one you need

### 19. Image Type Must Match for Diff OTA

**Problem**: The OTA image header supports multiple image types (ORIGINAL, ORIGINAL_XZ, DIFF, DIFF_XZ). Diff OTA requires the exact previous version. If the running version doesn't match `ver_diff_depend`, the diff apply fails.

**Fix**: Use full (ORIGINAL or ORIGINAL_XZ) images for general updates. Only use DIFF images when you can guarantee the base version matches.
