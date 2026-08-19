# Creating a New CI13XX Unified SDK Project

## Prerequisites

- `riscv-nuclei-elf-gcc-9.2.0` toolchain
- VS Code with `ci-tool` extension
- CI13XX SDK (recommend `CI13XX_SDK_ASR_ALG_V2.7.12` or `CI13XX_SDK_LLM_AIOT_2.1.2`)

## Steps

### 1. Choose SDK variant

- `CI13XX_SDK_ASR_ALG_V2.7.12` - For ASR + audio algorithms
- `CI13XX_SDK_LLM_AIOT_2.1.2` - For LLM AIoT (TTS, VPR, NLP, BLE, IR)

### 2. Copy the closest sample

```bash
cd CI13XX_SDK_ASR_ALG_V2.7.12
cp -r projects/offline_asr_sample projects/my_project
```

### 3. Configure for your chip

In `user_config.h`:
```c
// Select chip type (1302, 1306, 13242, 13322, etc.)
#define CI_CHIP_TYPE                13242

// Select board
#define USE_CI_F16XGS02J_BOARD      1
#define BOARD_PORT_FILE             "CI-F16XGS02J.c"
```

### 4. Enable desired components

In `user_config.h`:
```c
// Audio algorithms
#define USE_AEC_MODULE              0  // Echo cancellation
#define USE_DENOISE_MODULE          0  // Noise reduction
#define USE_ALC_AUTO_SWITCH_MODULE  0  // Auto level control

// Player
#define AUDIO_PLAYER_ENABLE         1
#define USE_PROMPT_DECODER           1
#define USE_MP3_DECODER             1
```

### 5. Build and flash

Same as CI130X/CI13LC: `make -j` in `project_file/`, then `PACK_UPDATE_TOOL.exe`.

## Component Selection

| Component | ASR_ALG SDK | LLM_AIOT SDK |
|---|---|---|
| Offline ASR | Yes | Yes |
| AEC/Denoise/BF/DOA | Yes | Yes |
| Dereverb/AGC/DRC/EQ | Yes | Yes |
| TTS | No | Yes |
| VPR | No | Yes |
| NLP | No | Yes |
| BLE | No | Yes |
| IR | No | Yes |
| Sound event detection | Yes | No |
