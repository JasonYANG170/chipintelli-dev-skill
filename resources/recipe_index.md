# Recipe Index

Cross-chip recipe total index. Find the right recipe by scenario.

## Project Setup

| Scenario | CI110X | CI112X | CI130X | CI13LC | CI13XX | CI230X | CI23LC |
|---|---|---|---|---|---|---|---|
| New project | `chips/ci110x/recipes/new_project.md` | `chips/ci112x/recipes/new_project.md` | `chips/ci130x/recipes/new_project.md` | `chips/ci13lc/recipes/new_project.md` | `chips/ci13xx/recipes/new_project.md` | `chips/ci230x/recipes/new_project.md` | `chips/ci23lc/recipes/new_project.md` |

## Voice Recognition

| Scenario | CI130X | CI13LC | CI13XX | CI23LC | CI230X |
|---|---|---|---|---|---|
| Offline ASR | `chips/ci130x/recipes/offline_asr.md` | `chips/ci13lc/recipes/offline_asr.md` | `chips/ci13xx/recipes/offline_asr.md` | - | - |
| Wakeup + command | `chips/ci130x/recipes/offline_asr.md` | `chips/ci13lc/recipes/offline_asr.md` | `chips/ci13xx/recipes/offline_asr.md` | - | - |
| CWSL (self-learning) | - | `chips/ci13lc/recipes/cwsl.md` | - | `chips/ci23lc/recipes/cwsl.md` | - |
| Multi-intent | - | `chips/ci13lc/recipes/multi_intents.md` | - | - | - |
| Natural language (NL) | - | `chips/ci13lc/recipes/nl_asr.md` | - | - | - |
| LLM AIoT | - | - | `chips/ci13xx/recipes/llm_aiot.md` | - | `chips/ci230x/recipes/wifi_voice.md` |
| Sound event detection | - | - | `chips/ci13xx/recipes/sound_event.md` | - | - |
| VPR (voiceprint) | - | - | `chips/ci13xx/recipes/vpr.md` | - | - |
| Two-mic beamforming | `chips/ci130x/recipes/two_mic.md` | - | - | - | - |

## Audio Algorithms

| Scenario | CI130X | CI13XX |
|---|---|---|
| AEC (echo cancel) | `chips/ci130x/recipes/aec.md` | - |
| Beamforming / DOA | `chips/ci130x/recipes/two_mic.md` | - |
| Other algorithms | See `chips/ci130x/resources/api_reference.md` | See `chips/ci13xx/resources/example_list.md` |

## Audio Output

| Scenario | CI130X | CI13LC | CI13XX |
|---|---|---|---|
| Audio player | `chips/ci130x/recipes/audio_player.md` | `chips/ci13lc/recipes/audio_player.md` | - |
| TTS | - | - | `chips/ci13xx/recipes/tts.md` |

## Connectivity

| Scenario | CI130X | CI13LC | CI13XX | CI230X | CI23LC |
|---|---|---|---|---|---|
| IR control | `chips/ci130x/recipes/ir_control.md` | `chips/ci13lc/recipes/ir_control.md` | `chips/ci13xx/recipes/ir_control.md` | - | - |
| UART communication | `chips/ci130x/recipes/uart_comm.md` | - | - | - | - |
| BLE voice | - | - | `chips/ci13xx/recipes/ble_voice.md` | - | `chips/ci23lc/recipes/ble_voice.md` |
| BLE broadcast | - | - | - | - | `chips/ci23lc/recipes/ble_broadcast.md` |
| BLE mini-program | - | - | - | - | `chips/ci23lc/recipes/ble_miniprogram.md` |
| Wi-Fi + voice | - | - | - | `chips/ci230x/recipes/wifi_voice.md` | - |
| OTA | `chips/ci130x/recipes/ota.md` | `chips/ci13lc/recipes/ota.md` | `chips/ci13xx/recipes/ota.md` | `chips/ci230x/recipes/ota.md` | - |

## Peripherals

| Scenario | CI130X | CI13LC |
|---|---|---|
| GPIO | `chips/ci130x/recipes/gpio_control.md` | See `chips/ci13lc/resources/api_reference.md` |
| UART | `chips/ci130x/recipes/uart_comm.md` | See `chips/ci13lc/resources/api_reference.md` |
| PWM | `chips/ci130x/recipes/pwm_output.md` | See `chips/ci13lc/resources/api_reference.md` |
| Timer | `chips/ci130x/recipes/timer.md` | See `chips/ci13lc/resources/api_reference.md` |
| SPI Flash | `chips/ci130x/recipes/spiflash.md` | See `chips/ci13lc/resources/api_reference.md` |
| Watchdog | `chips/ci130x/recipes/watchdog.md` | See `chips/ci13lc/resources/api_reference.md` |
| Flash control | `chips/ci130x/recipes/flash_control.md` | - |
| Low power | `chips/ci130x/recipes/low_power.md` | - |
| Codec (audio) | `chips/ci130x/recipes/codec.md` | - |
| Other peripherals | See `chips/ci130x/resources/api_reference.md` | See `chips/ci13lc/resources/api_reference.md` |

## Build & Debug

| Scenario | Available In |
|---|---|
| Build system (Make + Lua) | `chips/ci130x/resources/build_system.md` |
| Flash programming | `chips/ci130x/recipes/new_project.md` |
| OpenOCD debug | `chips/ci130x/resources/build_system.md` |
| Firmware packing | `chips/ci130x/recipes/new_project.md` |
