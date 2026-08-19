# chipintelli-dev-skill

启英泰伦 (Chipintelli) 语音 MCU 全生态统一 AI 技能。覆盖 7 大芯片系列、20+ SDK 版本，
所有 recipe、API 参考、陷阱与示例工程均基于本地 `docs/` 文档（1128 页，来源 document.chipintelli.com）与真实 SDK 源码接地。

支持芯片：CI1102/CI1103、CI1122、CI1301/CI1302/CI1303/CI1306、CI1311/CI1312/CI13161/CI13162/CI13241/CI13242/CI13322、CI2305/CI2306、CI2312/CI23242。

## 功能特性

- 基于 20+ Chipintelli SDK 的真实源码与 1128 页官方文档
- 覆盖芯片系列：CI110X (1代)、CI112X (1代主控)、CI130X (2代)、CI13LC (3代低成本)、CI13XX (3代统一)、CI230X (Wi-Fi+BLE)、CI23LC (3代+BLE)
- 覆盖核心功能：离线 ASR、唤醒+命令词、CWSL 自学习、多意图识别、自然说、双麦波束、声源定位、声事件检测、LLM AIoT、VPR 声纹
- 覆盖音频算法：AEC 回声消除、降噪、波束成形、DOA、去混响、AGC/DRC/EQ、深度降噪
- 覆盖连接能力：BLE 语音、BLE 广播、蓝牙小程序、Wi-Fi+语音、红外控制、串口协议、OTA
- 覆盖外设驱动：GPIO/UART/I2C/IIS/ADC/PWM/Timer/DMA/SPIFlash/Watchdog/低功耗/Flash控制/Codec
- 每个 chip family 含 recipe（场景指南）、API 速查、内存布局、陷阱、示例索引
- RISC-V (Nuclei N300) + ARM Cortex-M4 (LN882H) 双架构支持
- 编译工具链：riscv-nuclei-elf-gcc 9.2.0 + Make / CMake + GCC-ARM

## 芯片系列一览

| 系列 | 代表芯片 | 架构 | 核心特性 | 典型应用 |
|---|---|---|---|---|
| CI110X | CI1102, CI1103 | RISC-V Nuclei N201 | 1代离线语音识别 | 基础语音控制 |
| CI112X | CI1122 | RISC-V Nuclei N201 | 1代主控 MCU | USB Dongle、空调遥控 |
| CI130X | CI1302, CI1306 | RISC-V Nuclei N300 | 2代离线 ASR，双核 | 语音模块、灯控、风扇 |
| CI13LC | CI1312, CI13242, CI13322 | RISC-V Nuclei N300 | 3代低成本，单/双麦 | 吸顶灯、取暖器、茶吧机 |
| CI13XX | CI130X + CI13LC 统一 | RISC-V Nuclei N300 | 3代统一 SDK | 全系列通用开发 |
| CI230X | CI2305, CI2306 | ARM Cortex-M4 (LN882H) | Wi-Fi + BLE + 语音 | 离在线语音 AIoT |
| CI23LC | CI2312, CI23242 | RISC-V Nuclei N300 | 3代 + BLE | 语音+蓝牙小程序 |

## 安装说明

### 1. 复制 skill 到 Hermes skill 目录

```bash
# Hermes Agent (Windows)
cp -r D:/启英泰伦/chipintelli-dev-skill ~/AppData/Local/hermes/skills/

# Claude Code: ~/.claude/skills 或 .claude/skills
# OpenCode: ~/.config/opencode/skills 或 .opencode/skills
```

### 2. 验证加载

```bash
# Hermes Agent
skill_view(name='chipintelli-dev-skill')
# 预期: readiness_status: available
```

## 工作原理

| 步骤 | 名称 | 说明 |
|------|------|------|
| 1 | 定位芯片 | 确定目标芯片型号，导航到 `chips/<family>/` |
| 2 | 选择 SDK | 根据芯片和应用场景选择合适的 SDK 版本 |
| 3 | 读 recipe | 找匹配场景的 `chips/<family>/recipes/*.md`，按步骤实现 |
| 4 | 查 API | `chips/<family>/resources/api_reference.md` 或真实头文件核实 |
| 5 | 校验 | 核对 `user_config.h` 配置、初始化顺序、引脚分配、模型文件 |
| 6 | 确认 | 向用户给出实现方案（配置、引脚、入口、构建命令） |
| 7 | 执行 | 复制最近的示例工程，修改 `user_config.h` + `user_msg_deal.c` |
| 8 | 构建 | RISC-V: `make -j` / ARM: `cmake --build` |
| 9 | 烧录 | `ci-tool-kit.exe` (CI13xx) / `JFlash.exe` (LN882H) |
| 10 | 调试 | UART 日志 + OpenOCD/GDB |

## 通用原则

- **双核架构**：CI130X/CI13XX 有 host 核 (N300) 和 nuclear 核 (DSP)，通过 `nuclear_com`/`mailbox` 通信
- **SCU 时钟门控**：使用外设前必须 `scu_set_device_gate()` 使能时钟
- **OS 依赖驱动**：QSPIFlash/DMA/I2C/SPI 驱动使用 FreeRTOS API，需在 RTOS 启动后初始化
- **`user_config.h` 统一配置**：板级、芯片型号、麦克风模式、串口、ASR、播放器、算法全在此文件
- **`source_file.prj` 控制编译**：添加源文件在此文件中添加，不是 Makefile
- **接地优先**：所有内容基于真实 SDK 源码和官方文档，不臆造 API
- **模型文件匹配**：ASR 模型必须与芯片型号匹配，在语音 AI 平台生成

## 本地资源

- 文档中心：`docs/` (1128 页，来源 document.chipintelli.com)
- 编译器：`riscv-nuclei-elf-gcc-9.2.0/` (GCC 9.2.0 + OpenOCD)
- SDK 集合：20+ SDK 目录，覆盖所有芯片系列和应用场景
- 芯片索引：`resources/chip_index.md`
- SDK 索引：`resources/sdk_index.md`
- Recipe 总表：`resources/recipe_index.md`
- 术语表：`resources/glossary.md`
- 启英泰伦官网：https://www.chipintelli.com
- 语音 AI 平台：https://aiplatform.chipintelli.com

## 许可

SDK 内容版权归 Chipintelli (启英泰伦) 所有。本聚合技能元数据 MIT。
