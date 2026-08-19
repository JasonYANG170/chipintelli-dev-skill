# CI110X Example Project Index

> **Chips**: CI1102, CI1103 (1st generation, RISC-V Nuclei N201)
> **Docs**: `docs/软件开发/SDK/CI110X芯片SDK/`

CI110X is the 1st generation Chipintelli voice MCU family. The SDK is available in 3 variants. The local `docs/` directory contains extracted documentation for the SDK structure, components, and drivers.

---

## SDK Variants

CI110X provides 3 SDK variants for different application scenarios:

| SDK | Target | Features |
|-----|--------|----------|
| `CI110X_SDK_ASR_Offline` | Pure offline ASR | AEC, Denoise. Simple applications: voice switches, plugs |
| `CI110X_SDK_ALG_Application` | Algorithm-heavy | AEC, Denoise, DOA, Voice Enhancement, Dereverb, CWSL. Treadmills, range hoods |
| `CI110X_SDK_Combine_Cloud` | Online + offline | All algorithms + cloud connectivity. Smart speakers |

Download from: [启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment)

---

## SDK Structure

Based on the documentation at `docs/软件开发/SDK/CI110X芯片SDK/`:

```
CI110X芯片SDK/
  CI110X芯片SDK/
    index.md               # SDK overview
  components/               # SDK components
    CI110X IIC 协议/        # I2C protocol
    DOA使用说明/            # Direction of Arrival
    FatFS/                  # FAT file system
    flash_control/         # Flash management
    FreeRTOS/              # RTOS
    ir/                    # IR remote control
    key/                   # Key input
    led/                   # LED control
    LOG日志/               # Logging
    LWIP/                  # LwIP TCP/IP stack
    nvdata/                # Non-volatile data
    sensor/                # Sensor drivers
    单总线/                # Single-wire protocol
    调试组件/              # Debug components
    回声消除使用说明/       # AEC usage
    离线命令词自学习使用说明/ # CWSL usage
    命令词信息表/          # Command word info
    提示音播放器/          # Prompt player
    系统监控/              # System monitor
    音频播放器/            # Audio player
  driver/
    ci110x_driver/         # CI110X chip driver
      adc/                # ADC
      cache/               # Cache control
      codec/              # Audio codec
      gpio/                # GPIO
      iic/                 # I2C
      iis/                 # IIS (audio interface)
      iwdg/                # Independent watchdog
      pwm/                 # PWM
      sdio/                # SDIO
      spi/                 # SPI
      spiflash/            # SPI Flash
      timer/               # Timer
      uart/                # UART
      录音和放音设备/       # Recording and playback devices
    third_device_driver/   # Third-party device drivers
  start/                   # Getting started guides
    CI110X_SDK_Quick_Start/
    CI110X_SDK内存分配(icf)调整指南/
    CI110X串口协议/
    CI110X语义ID文档说明/
    FLASH加密功能使用说明/
    JLINK调试指南/
    SDK软件结构/
    低功耗使用说明/
    命令词和固件制作指南/
    内存结构/
    算法概述/
    语音模块与WIFI模块数据交互协议/
```

## Driver Modules

| Module | Path | Description |
|--------|------|-------------|
| ADC | `ci110x_driver/adc/` | Analog-to-digital converter |
| Cache | `ci110x_driver/cache/` | Instruction/data cache control |
| Codec | `ci110x_driver/codec/` | Audio codec (mic input, audio output) |
| GPIO | `ci110x_driver/gpio/` | General-purpose I/O |
| IIC | `ci110x_driver/iic/` | I2C master/slave |
| IIS | `ci110x_driver/iis/` | I2S audio interface |
| IWDG | `ci110x_driver/iwdg/` | Independent watchdog timer |
| PWM | `ci110x_driver/pwm/` | Pulse-width modulation |
| SDIO | `ci110x_driver/sdio/` | SDIO interface (for WiFi module communication) |
| SPI | `ci110x_driver/spi/` | SPI master/slave |
| SPIFlash | `ci110x_driver/spiflash/` | SPI Flash driver |
| Timer | `ci110x_driver/timer/` | Hardware timers |
| UART | `ci110x_driver/uart/` | UART communication |

## Key Documentation

| Doc | Path | Description |
|-----|------|-------------|
| Quick Start | `start/CI110X_SDK_Quick_Start/` | Getting started guide |
| Memory Allocation | `start/CI110X_SDK内存分配(icf)调整指南/` | ICF memory layout adjustment |
| UART Protocol | `start/CI110X串口协议/` | Serial communication protocol |
| Semantic IDs | `start/CI110X语义ID文档说明/` | Semantic ID documentation |
| Flash Encryption | `start/FLASH加密功能使用说明/` | Flash encryption feature |
| JLINK Debug | `start/JLINK调试指南/` | JTAG/JLINK debugging guide |
| SDK Structure | `start/SDK软件结构/` | Software architecture overview |
| Low Power | `start/低功耗使用说明/` | Low-power mode usage |
| Firmware Guide | `start/命令词和固件制作指南/` | Command word and firmware creation |
| Memory Layout | `start/内存结构/` | Memory structure overview |
| Algorithm Overview | `start/算法概述/` | Algorithm descriptions |
| WiFi Interaction | `start/语音模块与WIFI模块数据交互协议/` | Voice-WiFi module protocol |

## Differences from Later Generations

CI110X is the 1st generation with notable differences from CI130X/CI13LC:

1. **Core**: Nuclei N201 (vs N300 in later gens) -- slower, fewer features
2. **No dual-core**: Single core (CI130X introduced dual-core host+nuclear)
3. **No CWSL in offline SDK**: Only available in ALG_Application SDK
4. **WiFi via external module**: Uses SDIO to communicate with external WiFi (not integrated)
5. **Separate SPI driver**: Has dedicated SPI module (later gens merged into chip driver)
6. **Different build system**: Uses ICF (IAR) or Makefile, not the unified Lua system
7. **Limited flash**: CI1102 = 2MB, CI1103 = 4MB (vs 4MB standard in later gens)

## Recommended Approach

For new projects on CI110X:
1. Read `start/CI110X_SDK_Quick_Start/` first
2. Refer to `chips/ci130x/` for patterns (many concepts carry over)
3. Download the appropriate SDK variant from the AI platform
4. Use `start/SDK软件结构/` for architecture understanding
5. Refer to `start/内存结构/` for memory layout
