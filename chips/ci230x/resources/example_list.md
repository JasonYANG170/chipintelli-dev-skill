# CI230X Example Project Index

> **SDK**: `CI230X_wifi_combo_sdk_release_v1.1.1/`
> **Chips**: CI2305, CI2306 (LN882H + CI13xx dual-chip)

---

## Main Project

### ci230x-wifi-sdk-combo

**Path**: `project/ci230x-wifi-sdk-combo/`

The primary (and only) project in the CI230X SDK. Implements Wi-Fi + BLE + voice combo functionality with cloud connectivity. Uses CMake build system.

**Key structure**:
```
project/ci230x-wifi-sdk-combo/
  CMakeLists.txt          # Project build configuration
  Config.h.in             # Config template (processed by CMake)
  gcc/                    # GCC toolchain and linker
    ln882h.ld             # Linker script
    gcc-toolchain-setup.cmake
    gcc-compiler-flags.cmake
  cfg/                    # Configuration
    flash_partition_cfg.json    # Flash partition layout
    flash_partition_table.h     # Partition defines (generated)
    proj_config.h               # Project config
    project_prof_config.h       # Profile config
    ln882h.sct                  # Keil scatter file
  bsp/                    # Board support
    serial_hw.c                 # UART hardware init
    drv_adc_measure.c           # ADC voltage measurement
  startup/
    startup_ln882h_gcc.c        # ARM startup code
  app/                    # Application code (see below)
```

---

## Application Modules (`app/`)

### cias_aiot_wifi

**Path**: `app/cias_aiot_wifi/`

Main AIoT application with cloud connectivity. Contains sub-modules:

| Sub-module | Path | Description |
|------------|------|-------------|
| `cias_common` | `cias_common/` | Common utilities and headers |
| `cias_adapter` | `cias_adapter/` | Hardware adapter layer (`cias_ln_hardware.c`) |
| `cias_auth` | `cias_auth/` | Device authentication |
| `cias_system` | `cias_system/` | System management |
| `cias_kernel` | `cias_kernel/freertos/` | FreeRTOS kernel wrappers (queues, etc.) |
| `cias_msg_handle` | `cias_msg_handle/` | Message handling, slave (CI13xx) communication |
| `cias_media` | `cias_media/` | Media playback (MP3, TTS) |
| `cias_protocol` | `cias_protocol/cias_olcp/` | OLCP protocol (online command protocol) |
| `cias_net` | `cias_net/cias_http_interface/` | HTTP interface |
| `cias_net` | `cias_net/cias_ap_interface/` | AP mode networking (BLE provisioning) |
| `cias_wifi_port` | `cias_wifi_port/` | WiFi port layer |
| `cias_ble_port` | `cias_ble_port/` | BLE configuration port |
| `cias_ota` | `cias_ota/ota_audio/` | Audio chip OTA agent |
| `cias_ota` | `cias_ota/ota_wifi/` | WiFi chip OTA |
| `cias_algorithm` | `cias_algorithm/` | Audio compression (Speex) |
| `cias_cloud` | `cias_cloud/` | Cloud platform integrations (see below) |

### Cloud Platform Integrations

| Cloud | Path | CMake Option | Description |
|-------|------|-------------|-------------|
| Tencent TVS | `cias_cloud/cloud_tvs/` | `CIAS_IOT_TVS_ENABLE=1` | Tencent Voice Service (default) |
| Hisense | `cias_cloud/cloud_iot_hisense/` | `CIAS_IOT_CLOUD_HISENSE_ENABLE=1` | Hisense IoT |
| Tencent QCloud | `cias_cloud/cloud_iot_tencent/` | `CIAS_IOT_TENCENT_ENABLE=1` | Tencent IoT Cloud |
| Alibaba | `cias_cloud/cloud_ali/` | `CIAS_IOT_CLOUD_ALI_ENABLE=1` | Alibaba IoT (currently disabled) |
| Tuya | `cias_cloud/cloud_tuya/` | `CIAS_IOT_TUYA_ENABLE=1` | Tuya IoT platform |
| Chipintelli | `cias_cloud/` | `CIAS_IOT_CLOUD_CI_ENABLE=1` | Chipintelli cloud |
| Huawei | `cias_cloud/` | `CIAS_IOT_CLOUD_HUAWEI=1` | Huawei cloud |
| Xiaomi | `cias_cloud/` | `CIAS_IOT_CLOUD_XIAOMI_ENABLE=1` | Xiaomi cloud |

### BLE Application Variants

| App | Path | Cloud Platform | Description |
|-----|------|---------------|-------------|
| `ble_usr_ln` | `app/ble_usr_ln/` | Default (TVS/CI) | Standard BLE configuration |
| `ble_usr_hisense` | `app/ble_usr_hisense/` | Hisense | Hisense BLE pairing |
| `ble_usr_tuya` | `app/ble_usr_tuya/` | Tuya | Tuya BLE pairing |

### User Application Variants

| App | Path | Description |
|-----|------|-------------|
| `usr_ln` | `app/usr_ln/` | Default user application |
| `usr_hisense` | `app/usr_hisense/` | Hisense user app (includes ultra sleep) |
| `usr_tuya` | `app/usr_tuya/` | Tuya user app |

### ATE (Auto Test Equipment)

**Path**: `app/ate/`

Production-line testing code (`ln_ty_ate.c`). Enabled when building for manufacturing test.

---

## SDK Components (`components/`)

| Component | Path | Description |
|-----------|------|-------------|
| `ble` | `components/ble/` | BLE stack (full stack library) |
| `fota` | `components/fota/` | Firmware OTA agent |
| `fs` | `components/fs/` | File systems: KV, NVDS, partition manager |
| `kernel` | `components/kernel/` | FreeRTOS kernel |
| `libc` | `components/libc/` | C library stubs |
| `net` | `components/net/` | Networking: LwIP, DHCPD, HTTP, mbedTLS, ping, iperf |
| `serial` | `components/serial/` | Serial communication |
| `utils` | `components/utils/` | Utilities (CRC, hex dump, etc.) |
| `wifi` | `components/wifi/` | WiFi driver library |
| `ln_at` | `components/ln_at/` | AT command framework |
| `ln_at_cmd` | `components/ln_at_cmd/` | AT command implementations |

---

## MCU Driver (`mcu/`)

| Module | Path | Description |
|--------|------|-------------|
| `CMSIS_5.3.0` | `mcu/CMSIS_5.3.0/` | ARM CMSIS headers |
| `driver_ln882h` | `mcu/driver_ln882h/` | LN882H HAL driver |
| `hal` | `mcu/driver_ln882h/hal/` | HAL layer (GPIO, UART, I2C, SPI, etc.) |
| `ln882h` | `mcu/ln882h/` | Chip-specific headers and startup |

---

## Tools (`tools/`)

| Tool | Path | Description |
|------|------|-------------|
| JFlash | `tools/JFlash/` | JFlash tool with `ln882h.jflash` config for flashing |
| Python scripts | `tools/python_scripts/` | `after_build_gcc.py` (post-build image generation) |

---

## Build Configurations

The project supports multiple build configurations via CMake options:

### Default (Tencent TVS)
```cmake
set(CIAS_AIOT_ENABLE              1)
set(CIAS_BLE_CONFIG_ENABLE        1)
set(CIAS_IOT_TVS_ENABLE           1)  # Tencent TVS
set(CIAS_IOT_CLOUD_HISENSE_ENABLE 0)
set(CIAS_IOT_TUYA_ENABLE          0)
set(CIAS_AIOT_AUDIO_OTA_ENABLE    0)
set(CIAS_AIOT_WIFI_OTA_ENABLE     0)
```

### Hisense
```cmake
set(CIAS_IOT_CLOUD_HISENSE_ENABLE 1)
set(CIAS_IOT_TVS_ENABLE           0)
# BLE_USR_APP_PATH changes to app/ble_usr_hisense
# USR_APP_PATH changes to app/usr_hisense
```

### Tuya
```cmake
set(CIAS_IOT_TUYA_ENABLE          1)
set(CIAS_IOT_TVS_ENABLE           0)
set(CIAS_TUYA_IR_CTRL_ENABLE      1)  # Tuya IR remote
# Requires product key configuration
# BLE_USR_APP_PATH changes to app/ble_usr_tuya
# USR_APP_PATH changes to app/usr_tuya
```

### Debug/Release
```cmake
set(CMAKE_BUILD_TYPE  Release CACHE STRING "build for release" FORCE)
# or
set(CMAKE_BUILD_TYPE  Debug   CACHE STRING "build for debug"   FORCE)
```

---

## Build Commands

```bash
# Set toolchain
export CROSS_TOOLCHAIN_ROOT=/path/to/arm-none-eabi-gcc

# Configure (release)
cd CI230X_wifi_combo_sdk_release_v1.1.1
cmake -B build-ci230x-wifi-sdk-combo-release

# Build
cmake --build build-ci230x-wifi-sdk-combo-release

# Output: build-ci230x-wifi-sdk-combo-release/bin/ci230x-wifi-sdk-combo.elf
```

**Flash**: Use `tools/JFlash/JFlash.exe` with `ln882h.jflash` configuration.
