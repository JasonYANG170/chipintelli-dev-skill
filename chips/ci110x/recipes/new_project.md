# Recipe: Creating a New CI110X Project

> **Chips**: CI1102, CI1103 (1st generation, RISC-V Nuclei N201)
> **Docs**: `docs/软件开发/SDK/CI110X芯片SDK/`
> **SDK**: Download from [启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment)
> Applies to: CI1102 and CI1103 first-generation offline voice MCU projects.
> Excludes: CI112X host MCU projects, CI130X/CI13LC second/third-generation projects, and CI230X ARM/LN882H combo projects.
> Evidence: `chips/ci110x/resources/example_list.md`, `chips/ci110x/resources/pitfalls.md`, and CI110X SDK project templates.
> Validation: example-derived.

CI110X is the 1st generation Chipintelli voice MCU. The SDK uses riscv-nuclei-elf-gcc with Make or IAR. This recipe provides a brief guide for setting up a new project. Many concepts are shared with CI130X (see `chips/ci130x/recipes/new_project.md` for detailed patterns).

---

## Step 1: Select SDK Variant

Choose based on your application:

| SDK | When to Use |
|-----|-------------|
| `CI110X_SDK_ASR_Offline` | Simple offline voice control (switches, plugs) |
| `CI110X_SDK_ALG_Application` | Noisy environments, DOA, advanced algorithms |
| `CI110X_SDK_Combine_Cloud` | Smart speakers, online + offline combo |

## Step 2: Copy Template Project

Copy the closest sample project from the SDK:

```bash
cd CI110X_SDK_ASR_Offline  # or your chosen SDK
cp -r projects/sample_project projects/my_device
```

## Step 3: Configure Chip Type

In your project's `user_config.h` (or equivalent config file):

```c
#define CI_CHIP_TYPE  1102  // or 1103
```

Refer to `start/SDK软件结构/` for the exact configuration file location in your SDK variant.

## Step 4: Configure Peripherals

CI110X driver modules are in `driver/ci110x_driver/`:

```c
// NOTE: The CI110X SDK is not included locally. These header names are
// based on the SDK documentation (docs/软件开发/SDK/CI110X芯片SDK/).
// Download the SDK from the voice AI platform and verify all function
// signatures against the actual headers in driver/ci110x_driver/.
#include "ci110x_gpio.h"
#include "ci110x_uart.h"
#include "ci110x_timer.h"
// etc.
```

Configure:
- **UART**: Log output and protocol UART (must be different ports)
- **GPIO**: Input/output pins for your application
- **I2C**: If using external sensors
- **SPIFlash**: For voice model and prompt storage

## Step 5: Configure ASR

1. Generate ASR model on [voice AI platform](https://aiplatform.chipintelli.com)
2. Place model files in the project's firmware directory
3. Configure wakeup word, command words, and voice prompts
4. Set confidence thresholds and exit-wakeup timeout

Refer to `start/命令词和固件制作指南/` for firmware packaging.

## Step 6: Configure Memory

Refer to `start/内存结构/` and `start/CI110X_SDK内存分配(icf)调整指南/`:

- CI1102: 2MB flash, limited RAM
- CI1103: 4MB flash, more RAM
- Adjust heap size if using many tasks or large buffers
- Use the memory adjustment guide to tune ICF/ld settings

## Step 7: Build

```bash
# Set up RISC-V toolchain
export PATH="$SDK_ROOT/tools/build-tools/bin:$GCC_ROOT/bin:$PATH"

# Build
cd projects/my_device
make -j4
```

For IAR builds, open the `.eww` workspace file and build.

## Step 8: Flash and Debug

Refer to `start/JLINK调试指南/` for JTAG debugging setup.

Flash using:
- `ci-tool-kit.exe` or equivalent flashing tool from the SDK
- JLink/OpenOCD for debugging

## Step 9: Verify

1. Check UART log for SDK welcome message
2. Verify ASR model loads correctly
3. Test wakeup word recognition
4. Test command word recognition and prompt playback

## Key Differences from CI130X+

1. **Nuclei N201 core**: Slower than N300, fewer pipeline stages
2. **Single core**: No dual-core host+nuclear architecture
3. **Separate driver files**: Each peripheral has its own directory (not unified in `ci13lc_chip_driver`)
4. **No unified Lua build**: May use traditional Makefile or IAR
5. **External WiFi**: Use SDIO driver (`ci110x_driver/sdio/`) to communicate with external WiFi module
6. **Flash encryption**: Available via `start/FLASH加密功能使用说明/`

## Reference Documentation

| Document | Path |
|----------|------|
| Quick Start | `docs/软件开发/SDK/CI110X芯片SDK/start/CI110X_SDK_Quick_Start/` |
| SDK Structure | `docs/软件开发/SDK/CI110X芯片SDK/start/SDK软件结构/` |
| Memory Layout | `docs/软件开发/SDK/CI110X芯片SDK/start/内存结构/` |
| UART Protocol | `docs/软件开发/SDK/CI110X芯片SDK/start/CI110X串口协议/` |
| Firmware Guide | `docs/软件开发/SDK/CI110X芯片SDK/start/命令词和固件制作指南/` |
| Algorithm Overview | `docs/软件开发/SDK/CI110X芯片SDK/start/算法概述/` |
| WiFi Interaction | `docs/软件开发/SDK/CI110X芯片SDK/start/语音模块与WIFI模块数据交互协议/` |

## Migration to Later Generations

If your project outgrows CI110X capabilities, consider migrating to:
- **CI130X**: 2nd gen, dual-core, faster N300, more RAM (see `chips/ci130x/`)
- **CI13LC**: 3rd gen, low-cost, CWSL, NN encoder (see `chips/ci13lc/`)
- **CI23LC**: 3rd gen + BLE (see `chips/ci23lc/`)

The ASR API and message handling patterns are similar across generations, simplifying migration.
