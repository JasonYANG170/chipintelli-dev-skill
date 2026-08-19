# CI23LC Example Project Index

> **SDK**: `CI23LC_SDK_BLE_V1.3.13/CI23LC_SDK_BLE_V1.3.13/`
> **Chips**: CI2312, CI23242

---

## Projects

### 1. offline_asr_sample

**Path**: `projects/offline_asr_sample/`

The primary project for CI23LC. Implements offline ASR + BLE voice control with WeChat mini-program support. Contains 7 firmware demo variants for different appliance types.

**Key files**:
- `src/main.c` -- Main entry, hardware init, BLE task creation
- `src/user_config.h` -- Chip type (23242/23162), BLE config, ASR config
- `src/system_msg_deal.c` -- System message handling (ASR + BLE messages)
- `src/ci_ssp_config.c` -- Voice signal processing config
- `project_file/source_file.prj` -- Source file list (includes BLE sources)
- `src/ci13lc.lds` -- Linker script

**Build**:
```bash
cd projects/offline_asr_sample/project_file
make -j4
```

**Firmware demos** (see below for details):
- `firmware_demo/firmware_风扇` (Fan)
- `firmware_demo/firmware_空调` (Air Conditioner)
- `firmware_demo/firmware_灯控` (Light/RGB Control)
- `firmware_demo/firmware_茶吧机` (Tea Bar Machine)
- `firmware_demo/firmware_取暖器` (Warmer/Heater)
- `firmware_demo/firmware_取暖桌` (Heating Table)
- `firmware_demo/firmware_水暖毯` (Water Heated Blanket)

### 2. cwsl_sample

**Path**: `projects/cwsl_sample/`

Command Word Self-Learning (CWSL) sample project. Allows users to train custom command words at runtime without re-flashing. Functionally similar to CI13LC CWSL but includes BLE support.

---

## Firmware Demo Variants

All demos live under `projects/offline_asr_sample/firmware_demo/`. Each demo contains:
- `voice/src/` -- Voice prompt WAV files (named by command ID)
- `合成分区bin文件.bat` -- Generate partition bin
- `打包升级.bat` -- Package and upgrade

### Demo Selection

Select the active demo in `app_ble/demo/cias_demo_config.h`:
```c
#define DEV_DRIVER_EN_ID  DEV_FAN_MAIN_ID  // Change this line
```

### firmware_风扇 (Fan) -- DEV_FAN_MAIN_ID (0x06)

**Device**: `DEV_NUMBER_ID = 9` (9-speed fan)

**Command words** (sample, from voice file names):
- `[1]` Turn on/off
- `[2]` Turn off
- `[3]`~`[8]` Speed levels 7-12
- `[4]`~`[6]` Speed levels 8-10
- `[34]` Decrease brightness / `[32]` Increase brightness
- `[35]` Timer cancelled
- `[39]` Wind speed decreased / `[38]` Wind speed increased
- `[40]` Turn on fan light / `[41]` Turn off fan light
- `[42]` Night light mode / `[43]` Bright mode
- `[44]` Close night light / `[45]` Close bright mode
- `[50]`~`[55]` Speed levels 1-6
- `[56]` Max wind / `[57]` Medium wind / `[58]` Min wind
- `[75]`~`[91]` Timer 1-8 hours
- `[65535]` Parameter info (do not delete)

**BLE callbacks**: `fan_init()`, `fan_callback()`, `fan_query()`, `fan_report()`

**Features**: Power, 12-speed control, night/bright light, timer 1-8h, volume control, ASR on/off

### firmware_空调 (Air Conditioner) -- DEV_AIRCONDITION_MAIN_ID (0x02)

**Device**: `DEV_NUMBER_ID = 1`

**BLE callbacks**: `aircondition_init()`, `aircondition_callback()`, `aircondition_query()`, `aircondition_report()`

### firmware_灯控 (Light/RGB Control) -- DEV_LIGHT_CONTROL_MAIN_ID (0x03)

**Device**: `DEV_NUMBER_ID = DEV_LIGHT_CONTROL_RGB_SUB_ID (0x07)`

**BLE callbacks**: `rgb_init()`, `rgb_callback()`, `rgb_query()`, `rgb_report()`

**Note**: Also includes `cias_rgb_driver.c` for RGB LED hardware control. When `CIAS_BLE_ADV_GROUP_MODE_ENABEL` is set, `CONFIG_TYPE` becomes 4 (broadcast device).

### firmware_茶吧机 (Tea Bar Machine) -- DEV_TEA_BAR_MAIN_ID (0x05)

**Device**: `DEV_NUMBER_ID = 1`

**BLE callbacks**: `tbm_init()`, `tbm_callback()`, `tbm_query()`, `tbm_report()`

### firmware_取暖器 (Warmer/Heater) -- DEV_WARMER_MAIN_ID (0x08)

**Device**: `DEV_NUMBER_ID = 1`

**BLE callbacks**: `warmer_init()`, `warmer_callback()`, `warmer_query()`, `warmer_report()`

### firmware_取暖桌 (Heating Table) -- DEV_HEATTABLE_MAIN_ID (0x07)

**Device**: `DEV_NUMBER_ID = 1`

**BLE callbacks**: `heattable_init()`, `heattable_callback()`, `heattable_query()`, `heattable_report()`

### firmware_水暖毯 (Water Heated Blanket) -- DEV_WATERHEATED_MAIN_ID (0x09)

**Device**: `DEV_NUMBER_ID = 1`

**BLE callbacks**: `waterheated_init()`, `waterheated_callback()`, `waterheated_query()`, `waterheated_report()`

---

## SDK Components

| Component | Path | Description |
|-----------|------|-------------|
| `ci_ble` | `components/ci_ble/` | BLE stack: `ble_main.c`, `ble_communicate.c` |
| `asr` | `components/asr/` | ASR engine (libasr.a) |
| `alg` | `components/alg/` | Audio algorithms (AEC, denoise, beamforming) |
| `player` | `components/player/` | Audio player (MP3, prompt, ADPCM) |
| `cmd_info` | `components/cmd_info/` | Command word info reader |
| `msg_com` | `components/msg_com/` | UART protocol, I2C protocol |
| `led` | `components/led/` | LED/RGB light control |
| `flash_control` | `components/flash_control/` | Flash data management |
| `ci_nvdm` | `components/ci_nvdm/` | NV data management (pairing keys, etc.) |
| `freertos` | `components/freertos/` | FreeRTOS kernel + port |
| `codec_manager` | `components/codec_manager/` | Audio codec management |

## BLE Application Layer

| File | Path | Description |
|------|------|-------------|
| `ble_adv_msg_deal.c/h` | `app_ble/` | BLE advertising + 2.4G remote handling |
| `cias_ble_msg_deal.c/h` | `app_ble/demo/` | BLE message dispatch, protocol parsing |
| `cias_demo_config.h` | `app_ble/demo/` | Demo device selection |
| `cias_fan_msg_deal.c/h` | `app_ble/demo/` | Fan appliance logic |
| `cias_aircondition_msg_deal.c/h` | `app_ble/demo/` | Air conditioner logic |
| `cias_rgb_msg_deal.c/h` | `app_ble/demo/` | RGB light logic |
| `cias_rgb_driver.c/h` | `app_ble/demo/` | RGB LED hardware driver |
| `cias_tbm_msg_deal.c/h` | `app_ble/demo/` | Tea bar machine logic |
| `cias_warmer_msg_deal.c/h` | `app_ble/demo/` | Warmer logic |
| `cias_heattable_msg_deal.c/h` | `app_ble/demo/` | Heating table logic |
| `cias_waterheated_msg_deal.c/h` | `app_ble/demo/` | Water heated blanket logic |

## Driver

| Module | Path |
|--------|------|
| `ci13lc_chip_driver` | `driver/ci13lc_chip_driver/` (same as CI13LC) |
| `boards` | `driver/boards/` (CI-G24XGS02J-V10.h for CI23242) |
| `third_device_driver` | `driver/third_device_driver/` |
