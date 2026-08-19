# Recipe: Creating a New CI230X Wi-Fi Combo Project with CMake

> **SDK**: `CI230X_wifi_combo_sdk_release_v1.1.1`
> **Chips**: CI2305, CI2306 (LN882H + CI13xx)

The CI230X SDK uses CMake (not Make). There is one main project (`ci230x-wifi-sdk-combo`) that supports multiple cloud platforms via CMake options. This recipe explains how to create a new project or customize the existing one.

---

## Understanding the Build System

### Build Architecture

```
CI230X_wifi_combo_sdk_release_v1.1.1/
  CMakeLists.txt                    # Root CMake: toolchain, project selection
  project/
    ci230x-wifi-sdk-combo/
      CMakeLists.txt                # Project CMake: source collection, cloud options
      ProjModuleCollect.cmake       # Module collection helper
      gcc/
        gcc-toolchain-setup.cmake   # ARM GCC toolchain setup
        gcc-compiler-flags.cmake    # Compiler flags
        gcc-custom-build-stage.cmake # Post-build stages
        ln882h.ld                   # Linker script
      cfg/
        flash_partition_cfg.json    # Flash partition config
        flash_partition_table.h     # Partition defines
        proj_config.h               # Project configuration
      bsp/                          # Board support
      startup/                      # ARM startup code
      app/                          # Application code
  components/                        # SDK components
  mcu/                               # MCU drivers (LN882H HAL)
```

### Toolchain Setup

The LN882H uses ARM GCC (`arm-none-eabi-gcc`). Set the environment variable:

```bash
# Linux/Mac
export CROSS_TOOLCHAIN_ROOT=/opt/gcc-arm-none-eabi

# Windows (Git Bash)
export CROSS_TOOLCHAIN_ROOT="C:/Program Files/gcc-arm-none-eabi"
```

The toolchain setup (`gcc-toolchain-setup.cmake`) reads this and configures:
- `CMAKE_C_COMPILER`: `arm-none-eabi-gcc`
- `CMAKE_CXX_COMPILER`: `arm-none-eabi-g++`
- `CMAKE_ASM_COMPILER`: `arm-none-eabi-gcc`
- `CMAKE_OBJCOPY`: `arm-none-eabi-objcopy`
- `LN_MKIMAGE`: `tools/python_scripts/after_build_gcc.py`

---

## Option 1: Customize the Existing Project

For most use cases, customize `project/ci230x-wifi-sdk-combo/` directly.

### Step 1: Select Cloud Platform

Edit `project/ci230x-wifi-sdk-combo/CMakeLists.txt`:

```cmake
set(CIAS_AIOT_ENABLE              1)
set(CIAS_BLE_CONFIG_ENABLE        1)

# Select ONE cloud platform:
set(CIAS_IOT_TVS_ENABLE           1)   # Tencent TVS (default)
set(CIAS_IOT_CLOUD_HISENSE_ENABLE 0)
set(CIAS_IOT_TUYA_ENABLE          0)
set(CIAS_IOT_CLOUD_ALI_ENABLE     0)
set(CIAS_IOT_TENCENT_ENABLE       0)
set(CIAS_IOT_CLOUD_HUAWEI         0)
set(CIAS_IOT_CLOUD_XIAOMI_ENABLE  0)
set(CIAS_IOT_CLOUD_CI_ENABLE      0)

# OTA options:
set(CIAS_AIOT_AUDIO_OTA_ENABLE    0)   # Audio chip OTA
set(CIAS_AIOT_WIFI_OTA_ENABLE     0)   # WiFi chip OTA
```

### Step 2: Configure User Application

The user app path is set automatically based on cloud selection:

```cmake
# Default
set(USR_APP_PATH     app/usr_ln)
set(BLE_USR_APP_PATH app/ble_usr_ln)

# Hisense (auto-selected when CIAS_IOT_CLOUD_HISENSE_ENABLE=1)
set(USR_APP_PATH     app/usr_hisense)
set(BLE_USR_APP_PATH app/ble_usr_hisense)

# Tuya (auto-selected when CIAS_IOT_TUYA_ENABLE=1)
set(USR_APP_PATH     app/usr_tuya)
set(BLE_USR_APP_PATH app/ble_usr_tuya)
```

### Step 3: Add Custom Source Files

Add your source files to the project CMakeLists.txt:

```cmake
# Add custom source files
file(GLOB_RECURSE MY_CUSTOM_SRC app/my_custom/*.c)

set(PROJ_ALL_SRC
    bsp/serial_hw.c
    bsp/drv_adc_measure.c
    app/ate/ln_ty_ate.c
    startup/startup_${CHIP_SERIAL}_gcc.c
    ${USR_APP_SRC}
    ${BLE_USR_APP_SRC}
    ${BLE_USR_APP_CALL_BACK_SRC}
    ${CIAS_BLE_CONFIG_SRC}
    ${MODULE_SRC}
    ${CIAS_AIOT_SRC}
    ${MY_CUSTOM_SRC}           # <-- Add your sources here
    # ...
)
```

### Step 4: Add Include Directories

```cmake
target_include_directories(${pro_executable_target}
    PRIVATE
    app
    bsp
    cfg
    app/my_custom              # <-- Add your include path
    # ... existing paths ...
)
```

### Step 5: Configure Flash Partitions (if needed)

Edit `cfg/flash_partition_cfg.json` (see `resources/memory_layout.md` for details):

```json
{
    "user_define": [
        {"partition_type": "APP",  "start_addr": "0x00007000", "size_KB": 1100},
        {"partition_type": "OTA",  "start_addr": "0x0011A000", "size_KB": 672},
        ...
    ]
}
```

Update the linker script `gcc/ln882h.ld` to match:
```
FLASH (rx) : ORIGIN = 0x10007100, LENGTH = 1100K
```

### Step 6: Build

```bash
# Configure
cd CI230X_wifi_combo_sdk_release_v1.1.1
cmake -B build-ci230x-wifi-sdk-combo-release

# Build
cmake --build build-ci230x-wifi-sdk-combo-release

# Output
# build-ci230x-wifi-sdk-combo-release/bin/ci230x-wifi-sdk-combo.elf
# build-ci230x-wifi-sdk-combo-release/bin/ci230x-wifi-sdk-combo.bin (after post-build)
```

### Step 7: Flash

Use JFlash with the LN882H configuration:
```bash
tools/JFlash/JFlash.exe -openprj tools/JFlash/ln882h.jflash
# Or use JFlash GUI with the .jflash project file
```

---

## Option 2: Create a New Project

### Step 1: Copy the project directory

```bash
cd CI230X_wifi_combo_sdk_release_v1.1.1
cp -r project/ci230x-wifi-sdk-combo project/my-wifi-project
```

### Step 2: Register in root CMakeLists.txt

Edit the root `CMakeLists.txt`:

```cmake
if(NOT DEFINED USER_PROJECT)
    set(USER_PROJECT  my-wifi-project)  # <-- Change this
    message(STATUS "<SET> USER_PROJECT = ${USER_PROJECT}")
endif()
```

### Step 3: Customize the project CMakeLists.txt

Edit `project/my-wifi-project/CMakeLists.txt`:

```cmake
include(ProjModuleCollect.cmake)

# Set your app paths
set(BLE_USR_APP_PATH app/ble_usr_ln)
set(USR_APP_PATH app/usr_ln)

# Enable/disable features
set(CIAS_AIOT_ENABLE           1)
set(CIAS_IOT_TVS_ENABLE        1)
set(CIAS_BLE_CONFIG_ENABLE     1)

# Collect source files
file(GLOB_RECURSE USR_APP_SRC      ${USR_APP_PATH}/*.c)
file(GLOB_RECURSE BLE_USR_APP_SRC  ${BLE_USR_APP_PATH}/*.c)

set(PROJ_ALL_SRC
    bsp/serial_hw.c
    bsp/drv_adc_measure.c
    startup/startup_${CHIP_SERIAL}_gcc.c
    ${USR_APP_SRC}
    ${BLE_USR_APP_SRC}
    ${MODULE_SRC}
)

# Output target
set(TARGET_ELF_NAME  ${USER_PROJECT})
set(pro_executable_target  ${TARGET_ELF_NAME}.elf)
add_executable(${pro_executable_target}  ${PROJ_ALL_SRC})

target_link_libraries(${pro_executable_target}
    PUBLIC
    ${CHIP_SERIAL}_ble_full_stack
    ln::dhcpd
    lwip
    ${CHIP_SERIAL}_wifi
    -lc -lm -lnosys
    PRIVATE
    -T${LINKER_SCRIPT}
    ${EXTRA_LINK_FLAGS}
)

target_link_directories(${pro_executable_target}
    PRIVATE
    ${LN_SDK_ROOT}/lib/gcclib
)
```

### Step 4: Build and flash (same as Option 1)

---

## Configuration Files

### proj_config.h (`cfg/proj_config.h`)

Key settings:
```c
#define XTAL_CLOCK       40000000    // 40 MHz crystal
#define HAL_ASSERT_EN    ENABLE      // HAL assertions (ENABLE or 0)
```

### Config.h.in

The CMake `configure_file` generates `Config.h` from `Config.h.in`. This file contains compile-time feature flags based on CMake options:

```cmake
configure_file(
    "${PROJECT_SOURCE_DIR}/Config.h.in"
    "${PROJECT_SOURCE_DIR}/Config.h"
)
```

Use `#cmakedefine` in `Config.h.in` to pass CMake options to C code.

---

## Debug vs Release

```cmake
# Release (default)
set(CMAKE_BUILD_TYPE  Release CACHE STRING "build for release" FORCE)

# Debug
set(CMAKE_BUILD_TYPE  Debug   CACHE STRING "build for debug"   FORCE)
```

Debug build includes `-g` debug symbols. Release build enables optimizations.

---

## Common Build Issues

1. **`CROSS_TOOLCHAIN_ROOT must be set`**: Set the environment variable to your ARM GCC installation.

2. **`flash partition overlap`**: Check `flash_partition_cfg.json` for overlapping partitions.

3. **`unknown user config: product_XXX`**: Tuya build requires a valid product key file.

4. **Linker errors with cloud libraries**: Only enable ONE cloud platform at a time.

5. **`TIMER3` conflict**: Don't use TIMER3 in application code -- it's reserved for WiFi.
