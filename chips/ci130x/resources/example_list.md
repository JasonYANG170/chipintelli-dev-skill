# CI130X Example Projects

> SDK: `CI130X_SDK_Offline_V2.1.14` and related SDKs

---

## CI130X_SDK_Offline_V2.1.14

### projects/offline_asr_pro_sample

**Description**: Standard offline voice recognition sample. This is the primary template for new projects.

**Key Features**:
- Offline ASR with wakeup word + command words
- Separate wakeup model (`USE_SEPARATE_WAKEUP_EN=1`)
- Audio player with prompt and MP3 support
- Voice module UART protocol (v2)
- Configurable for CI1302/CI1306/CI1312 boards
- IIS pre-result audio output option
- AEC, Denoise, ALC algorithm support (disabled by default)

**Default Board**: CI-D06GT01D (CI1306, 4MB flash)

**Source Files**:
| File | Description |
|------|-------------|
| `src/main.c` | Main entry: hardware init, platform init, task creation |
| `src/user_config.h` | Board/chip/UART/ASR/player configuration |
| `src/system_msg_deal.c` | System message processing (wakeup, ASR results, model switch) |
| `src/user_msg_deal.c` | User message handling (ASR result dispatch, key, COM) |
| `src/ci_ssp_config.c` | Voice signal processing config (STFT, AEC, BF, denoise, DOA) |
| `src/system_hook.c` | System hooks (wakeup, sleep, ASR result) |
| `src/ci130x.lds` | Linker script |

**Firmware Structure**:
| Directory | Contents |
|-----------|----------|
| `firmware/asr/` | ASR model files (cmd + wake) |
| `firmware/dnn/` | DNN model files |
| `firmware/voice/src/` | Voice prompt WAV files (numbered by cmd_id) |
| `firmware/user_file/cmd_info/` | Command word definition Excel file |

---

## CI130X_SDK_Offline_IR_V2.0.10

### projects/offline_asr_pro_sample (IR variant)

**Description**: Offline voice recognition with IR remote control support. Adds IR send/receive capability.

**Key Features**:
- All features of offline_asr_pro_sample
- IR remote control driver (`components/ir_remote_driver/`)
- IR data storage in flash (`firmware/user_file/[0]ir_data_*.bin`)
- Air conditioner device control demo (`src/device/air_device.c`)
- IR UART message processing (`src/LinkMsgProc/ir_uart_msg_deal.c`)
- ALC auto-switch enabled by default

**Default Board**: CI-D02GS01J (CI1302, 2MB flash)

**Additional Source Files**:
| File | Description |
|------|-------------|
| `components/ir_remote_driver/ir_remote_driver.c` | IR send/receive driver |
| `src/device/air_device.c` | Air conditioner device control demo |
| `src/LinkMsgProc/ir_uart_msg_deal.c` | IR UART message processing |
| `src/ir_src/ir_data.h` | IR data definitions |
| `src/ir_src/libir_data.a` | Pre-compiled IR data library |

**IR Hardware Config**:
- IR output: PA2 (PWM0, 5th function)
- IR receive: PA4 (GPIO, 1st function)
- IR timer: TIMER0

---

## CI130X_SDK_TwoMic_V1.0.2

### projects/offline_asr_sample

**Description**: Two-microphone beamforming ASR sample. Uses dual mics for directional voice enhancement.

**Key Features**:
- Beamforming (`USE_BEAMFORMING_MODULE=1`)
- Dereverb (`USE_DEREVERB_MODULE=1`)
- ALC auto-switch enabled
- Dual-channel codec (`HOST_CODEC_CHA_NUM=2`)
- 1024-point FFT for beamforming (vs 512 for single-mic)
- CWSL (command word self-learning) support
- CI1306 chip (4MB flash)

**Default Board**: CI1306 (4MB flash, QFN40)

**Key Differences from offline_asr_pro_sample**:
| Feature | offline_asr_pro_sample | TwoMic offline_asr_sample |
|---------|----------------------|--------------------------|
| Mic channels | 1 | 2 |
| Beamforming | Disabled | Enabled |
| Dereverb | Disabled | Enabled |
| FFT size | 512 | 1024 |
| Sample rate | 16kHz | 32kHz (with resample) |
| STFT frame size | 512 | 1024 |

### projects/cwsl_sample

**Description**: Command word self-learning (CWSL) sample based on two-mic platform.

**Key Features**:
- All two-mic beamforming features
- Command word self-learning capability
- User can record and train custom command words on-device

---

## CI130X_SDK_ALG_V2.7.14

Algorithm-focused SDK for audio processing (AEC, Denoise, BF, DOA). Contains algorithm component source code and test projects.

---

## CI130X_SDK_LLM_AIOT_2.2.7(1)

LLM AIoT SDK combining offline ASR with online LLM (Large Language Model) capabilities. Requires network connectivity.

---

## Board Reference

| Board Name | Chip | Flash | Package | Typical SDK |
|------------|------|-------|---------|-------------|
| CI-D02GS01J | CI1302 | 2MB | SSOP24 | Offline, IR |
| CI-D02GS02S | CI1302 | 2MB | SSOP24 (SMT) | Offline |
| CI-D12GS01J | CI1312 | 2MB | SSOP16 | Offline |
| CI-D06GT01D | CI1306 | 4MB | QFN40 | Offline, TwoMic, ALG |

---

## Choosing a Template

| Use Case | Template Project | SDK |
|----------|-----------------|-----|
| Standard offline ASR | `offline_asr_pro_sample` | `CI130X_SDK_Offline_V2.1.14` |
| Voice + IR remote control | `offline_asr_pro_sample` (IR) | `CI130X_SDK_Offline_IR_V2.0.10` |
| Two-mic beamforming | `offline_asr_sample` | `CI130X_SDK_TwoMic_V1.0.2` |
| Command word self-learning | `cwsl_sample` | `CI130X_SDK_TwoMic_V1.0.2` |
| Audio algorithm development | Algorithm projects | `CI130X_SDK_ALG_V2.7.14` |
| LLM + AIoT | LLM AIoT projects | `CI130X_SDK_LLM_AIOT_2.2.7(1)` |
