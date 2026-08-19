# Glossary

## Chipintelli Terminology

| Term | Abbreviation | Description |
|---|---|---|
| ASR | Automatic Speech Recognition | 语音识别，将语音转换为文本/命令 |
| KWS | Keyword Spotting | 关键词检测，用于唤醒词检测 |
| VAD | Voice Activity Detection | 语音活动检测，判断是否有人说话 |
| AEC | Acoustic Echo Cancellation | 回声消除，消除播放器声音对麦克风的干扰 |
| BF | Beamforming | 波束成形，多麦降噪和方向增强 |
| DOA | Direction of Arrival | 声源定位，判断声音来源方向 |
| CWSL | Command Word Self-Learning | 命令词自学习，用户可现场录制自定义命令词 |
| NL | Natural Language | 自然说，支持自然语言理解的离线语音识别 |
| VPR | Voiceprint Recognition | 声纹识别，通过声音特征识别说话人 |
| TTS | Text-To-Speech | 文本转语音 |
| LLM | Large Language Model | 大语言模型 |
| AIoT | AI + IoT | 人工智能物联网 |
| DPMU | Dynamic Power Management Unit | 动态电源管理单元 |
| SCU | System Control Unit | 系统控制单元，管理时钟/复位/电源 |
| DSU | Dual-core Sync Unit | 双核同步单元 |
| DTR | Dual Transfer Rate | 双倍数据传输速率（Flash） |
| LDO | Low Dropout Regulator | 低压差线性稳压器 |
| IWDG | Independent Watchdog | 独立看门狗 |
| WWDG | Window Watchdog | 窗口看门狗 |
| ECLIC | Enhanced Core-Level Interrupt Controller | Nuclei 增强型中断控制器 |
| PMP | Physical Memory Protection | 物理内存保护 |
| NVDM | Non-Volatile Data Management | 非易失性数据管理 |
| FOTA | Firmware Over The Air | 空中固件升级 |
| SSP | Signal Processing | 信号处理（语音前段） |
| DNN | Deep Neural Network | 深度神经网络 |
| VAD | Voice Activity Detection | 语音端点检测 |

## SDK Components

| Component | Description |
|---|---|
| `audio_in_manage` | 音频输入管理，处理麦克风采集 |
| `audio_in_codec` | 音频编解码器注册 |
| `audio_pre_rslt_iis_out` | ASR 前处理结果 IIS 输出 |
| `ci_cwsl` | 命令词自学习组件 |
| `ci_nvdm` | 非易失性数据管理 |
| `cmd_info` | 命令词信息表 |
| `codec_manager` | 编解码器管理 |
| `flash_control` | Flash 控制 |
| `flash_encrypt` | Flash 加密 |
| `freertos` | FreeRTOS RTOS |
| `led` | LED 控制 |
| `log` | 日志系统 (ci_log) |
| `msg_com` | 消息通信 |
| `nuclear_com` | 双核通信 |
| `ota` | OTA 固件升级 |
| `player` | 音频播放器 |
| `status_share` | 状态共享 |
| `sys_monitor` | 系统监控 |
| `ci_ble` | BLE 协议栈 |
| `ir_remote_driver` | 红外遥控驱动 |
| `tts` | 文本转语音 |
| `VPR` | 声纹识别 |
| `nlp` | 自然语言处理 |
| `protocol` | 通信协议 |
| `factory_test` | 工厂测试 |
| `simple_audio_player` | 简易音频播放器 |
| `cias_lib` | CIAS 通信库 |
| `cias_opus` | Opus 编解码 |
| `cias_speex` | Speex 编解码 |
| `cias_g722` | G.722 编解码 |

## Algorithm Modules (CI13XX)

| Module | Description |
|---|---|
| `aec` | 回声消除 |
| `agc` | 自动增益控制 |
| `ai_denoise` | AI 降噪 |
| `ai_denoise_rtc` | AI 降噪 (RTC 模式) |
| `alc_auto_switch` | 自动电平控制 |
| `basic_alg` | 基础算法 |
| `beamforming` | 波束成形 |
| `common` | 公共算法 |
| `denoise` | 降噪 |
| `dereverb` | 去混响 |
| `doa` | 声源定位 |
| `drc` | 动态范围压缩 |
| `eq` | 均衡器 |
| `pwk` | 唤醒词检测 |
| `sound_event_detection` | 声音事件检测 (婴儿哭声/鼾声等) |
| `vp_host` | 声纹主机 |

## Board Naming Convention

```
CI-X##YGT##S/Z

X = Series indicator:
  B = CI110X series
  C = CI1122 series
  D = CI130X/CI13XX series
  F = CI13LC series
  E = CI230X series
  G = CI23LC series

## = Module number (e.g., 02, 06, 16, 24)

Y = Package type:
  G = Module (模块)
  X = Custom

S = Flash size indicator

Z = Form factor:
  J = 端子模块 (pin header)
  S = SMT module
  T = 开发板 (development board)
  D = 开发板 (dev board variant)
  U = USB dongle
```

Examples:
- `CI-D06GT01D` = CI1306 开发板 (4MB flash)
- `CI-D02GS01J` = CI1302 端子模块 (2MB flash)
- `CI-F16XGS02J` = CI13LC 端子模块
- `CI-E0XGT02S` = CI230X SMT module
- `CI-G16XGS02J` = CI23LC 端子模块

## File Extensions

| Extension | Description |
|---|---|
| `.prj` | Source file list (Lua-parsed) |
| `.lds` | Linker script (RISC-V) |
| `.ld` | Linker script (ARM) |
| `.sct` | Scatter file (Keil ARM) |
| `.mk` | Makefile include |
| `.a` | Static library |
| `.vsix` | VS Code extension |
| `.bin` | Binary firmware |
| `.elf` | ELF executable |
