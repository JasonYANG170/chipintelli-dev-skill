# CI112X Example Project Index

> **Chip**: CI1122 (1st generation host MCU, RISC-V Nuclei N201)
> **Docs**: `docs/软件开发/SDK/CI112X芯片SDK/`

CI112X is the 1st generation host MCU variant of the Chipintelli voice MCU family. It is designed as a host controller with more flash and USB support. The SDK is offline-only.

---

## SDK Variant

CI112X has only one SDK variant:

| SDK | Target | Features |
|-----|--------|----------|
| `CI112X_SDK` | Pure offline ASR | Denoise only |

Download from: [启英泰伦语音AI平台](https://aiplatform.chipintelli.com/attachment)

---

## SDK Structure

Based on the documentation at `docs/软件开发/SDK/CI112X芯片SDK/`:

```
CI112X芯片SDK/
  CI112X芯片SDK/
    index.md               # SDK overview
  components/               # SDK components
    CI112X IIC 协议/        # I2C protocol
    flash_control/         # Flash management
    FreeRTOS/              # RTOS
    ir/                    # IR remote control
    key/                   # Key input
    led/                   # LED control
    LOG日志/               # Logging
    nvdata/                # Non-volatile data
    sensor/                # Sensor drivers
    单总线/                # Single-wire protocol
    命令词信息表/          # Command word info
    提示音播放器/          # Prompt player
    系统监控/              # System monitor
    音频播放器/            # Audio player
    语音降噪使用说明/       # Denoise usage
    语音识别使用说明/       # ASR usage
  driver/
    ci112x_driver/         # CI112X chip driver
      adc/                # ADC
      codec/               # Audio codec
      gpio/                # GPIO
      iic/                 # I2C
      iis/                 # IIS (audio interface)
      iwdg/                # Independent watchdog
      pwm/                 # PWM
      timer/               # Timer
      uart/                # UART
      录音和放音设备/       # Recording and playback devices
    third_device_driver/   # Third-party device drivers
  start/                   # Getting started guides
    CI112X_SDK_Quick_Start/
    CI112X串口协议/
    CI112X语义ID文档说明/
    FLASH加密功能使用说明/
    SDK软件结构/
    命令词和固件制作指南/
    内存结构/
    算法概述/
```

## Driver Modules

| Module | Path | Description |
|--------|------|-------------|
| ADC | `ci112x_driver/adc/` | Analog-to-digital converter |
| Codec | `ci112x_driver/codec/` | Audio codec (mic input, audio output) |
| GPIO | `ci112x_driver/gpio/` | General-purpose I/O |
| IIC | `ci112x_driver/iic/` | I2C master/slave |
| IIS | `ci112x_driver/iis/` | I2S audio interface |
| IWDG | `ci112x_driver/iwdg/` | Independent watchdog timer |
| PWM | `ci112x_driver/pwm/` | Pulse-width modulation |
| Timer | `ci112x_driver/timer/` | Hardware timers |
| UART | `ci112x_driver/uart/` | UART communication |

## Differences from CI110X

CI112X is similar to CI110X but with key differences:

1. **Host MCU role**: Designed as a host controller (more flash, USB support)
2. **No SDIO**: CI112X driver does not include SDIO (unlike CI110X)
3. **No SPI**: CI112X driver does not include separate SPI module
4. **No SPIFlash**: CI112X driver does not include SPI Flash driver
5. **No Cache**: CI112X driver does not include cache control
6. **No DOA**: Documentation does not include DOA usage guide
7. **No CWSL**: Documentation does not include command word self-learning
8. **No WiFi interaction**: No WiFi module communication protocol doc
9. **No LwIP**: No TCP/IP stack component
10. **No FatFS**: No FAT file system component
11. **Simpler component set**: Fewer components overall

## Key Documentation

| Doc | Path | Description |
|-----|------|-------------|
| Quick Start | `start/CI112X_SDK_Quick_Start/` | Getting started guide |
| UART Protocol | `start/CI112X串口协议/` | Serial communication protocol |
| Semantic IDs | `start/CI112X语义ID文档说明/` | Semantic ID documentation |
| Flash Encryption | `start/FLASH加密功能使用说明/` | Flash encryption feature |
| SDK Structure | `start/SDK软件结构/` | Software architecture overview |
| Firmware Guide | `start/命令词和固件制作指南/` | Command word and firmware creation |
| Memory Layout | `start/内存结构/` | Memory structure overview |
| Algorithm Overview | `start/算法概述/` | Algorithm descriptions |

## Use Cases

CI112X is suited for:
- Standalone offline voice control devices
- Host MCU applications (controlling other peripherals)
- Simple voice switch / plug / appliance control
- Applications requiring USB connectivity
- Products needing more flash than CI1102 provides

For more advanced features (DOA, CWSL, cloud, WiFi), use CI110X or later generations.
