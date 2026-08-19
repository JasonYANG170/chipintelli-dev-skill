# Recipe: Creating a New CI112X Project

> **Chip**: CI1122 (1st generation host MCU, RISC-V Nuclei N201)
> **Docs**: `docs/软件开发/SDK/CI112X芯片SDK/`
> **SDK**: Download `CI112X_SDK` from [启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment)

CI112X is the 1st generation host MCU variant. The SDK is offline-only with Denoise support. This recipe provides a brief guide for project setup. Many concepts are shared with CI110X (see `chips/ci110x/recipes/new_project.md`).

---

## Step 1: Download the SDK

Download `CI112X_SDK` from the [voice AI platform](https://aiplatform.chipintelli.com/attachment). This is the only SDK variant available for CI112X.

## Step 2: Copy Template Project

```bash
cd CI112X_SDK
cp -r projects/sample_project projects/my_device
```

## Step 3: Configure Chip

In your project's configuration file (refer to `start/SDK软件结构/` for exact location):

```c
#define CI_CHIP_TYPE  1122
```

## Step 4: Configure Peripherals

CI112X driver modules are in `driver/ci112x_driver/`. Note: CI112X has **fewer** driver modules than CI110X:

```c
// NOTE: The CI112X SDK is not included locally. These header names are
// based on the SDK documentation (docs/软件开发/SDK/CI112X芯片SDK/).
// Download the SDK from the voice AI platform and verify all function
// signatures against the actual headers in driver/ci112x_driver/.
//
// Available drivers:
#include "ci112x_gpio.h"
#include "ci112x_uart.h"
#include "ci112x_timer.h"
#include "ci112x_iic.h"
#include "ci112x_pwm.h"
#include "ci112x_adc.h"
#include "ci112x_codec.h"
#include "ci112x_iis.h"
#include "ci112x_iwdg.h"

// NOT available on CI112X (unlike CI110X):
// - No SDIO driver
// - No SPI driver
// - No SPIFlash driver
// - No cache control
```

If you need SPI Flash access, check if it's handled internally by the codec/flash_control component, or use the CI110X SDK instead.

## Step 5: Configure ASR

1. Generate ASR model on the [voice AI platform](https://aiplatform.chipintelli.com)
2. Place model files in the project's firmware directory
3. Configure wakeup and command words
4. Refer to `start/命令词和固件制作指南/` for firmware packaging

## Step 6: Configure Memory

Refer to `start/内存结构/` for CI112X memory layout:
- CI1122 has more flash than CI1102 (host MCU role)
- Adjust heap size if needed for your application

## Step 7: Build

```bash
# Set up RISC-V toolchain
export PATH="$SDK_ROOT/tools/build-tools/bin:$GCC_ROOT/bin:$PATH"

# Build
cd projects/my_device
make -j4
```

## Step 8: Flash and Debug

Refer to the Quick Start guide for flashing and debugging instructions.

## Key Characteristics

1. **Offline only**: No cloud/WiFi support in the SDK
2. **Denoise only**: Only Denoise algorithm available (no AEC, DOA, CWSL)
3. **Host MCU role**: Designed for controlling external peripherals
4. **USB support**: CI1122 includes USB (not in CI110X)
5. **More flash**: Larger flash than CI1102 for host applications
6. **No WiFi interaction**: No SDIO or WiFi module communication protocol

## Reference Documentation

| Document | Path |
|----------|------|
| Quick Start | `docs/软件开发/SDK/CI112X芯片SDK/start/CI112X_SDK_Quick_Start/` |
| SDK Structure | `docs/软件开发/SDK/CI112X芯片SDK/start/SDK软件结构/` |
| Memory Layout | `docs/软件开发/SDK/CI112X芯片SDK/start/内存结构/` |
| UART Protocol | `docs/软件开发/SDK/CI112X芯片SDK/start/CI112X串口协议/` |
| Firmware Guide | `docs/软件开发/SDK/CI112X芯片SDK/start/命令词和固件制作指南/` |
| Algorithm Overview | `docs/软件开发/SDK/CI112X芯片SDK/start/算法概述/` |
| ASR Usage | `docs/软件开发/SDK/CI112X芯片SDK/components/语音识别使用说明/` |
| Denoise Usage | `docs/软件开发/SDK/CI112X芯片SDK/components/语音降噪使用说明/` |

## Migration

If your project needs more features, consider migrating to:
- **CI110X**: Has SDIO, SPI, SPIFlash, DOA, CWSL, cloud support
- **CI130X**: 2nd gen, dual-core, faster N300 (see `chips/ci130x/`)
- **CI13LC**: 3rd gen, low-cost, CWSL, NN encoder (see `chips/ci13lc/`)
