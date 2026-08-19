# Recipe: Two-Mic Beamforming

> SDK: `CI130X_SDK_TwoMic_V1.0.2`  
> Chips: CI1306 (4MB flash, QFN40)

> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/two_mic.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

Two-microphone beamforming uses dual microphones to enhance voice from a specific direction while suppressing noise from other directions. This improves ASR accuracy in noisy environments.

### Signal Flow with Beamforming

```
Mic L ──► Codec ADC L ──► STFT ──┐
                                  ├──► BF ──► [Denoise] ──► ASR
Mic R ──► Codec ADC R ──► STFT ──┘
```

---

## Configuration

### Step 1: Set user_config.h

```c
// Chip type (TwoMic requires CI1306 with 4MB flash)
#define CI_CHIP_TYPE 1306

// Player
#define AUDIO_PLAYER_ENABLE 1
#define USE_PROMPT_DECODER 1
#define USE_MP3_DECODER 1
#define AUDIO_PLAY_SUPPT_MP3_PROMPT 1

// Algorithm modules
#define USE_ALC_AUTO_SWITCH_MODULE  1   // Dynamic ALC
#define USE_DENOISE_MODULE          0   // Optional denoise
#define USE_DOA_MODULE              0   // Optional direction of arrival
#define USE_DEREVERB_MODULE         1   // Dereverb (recommended with BF)
#define USE_BEAMFORMING_MODULE      1   // Enable beamforming
#define USE_BEAMFORMING_OLD_METHOD  0   // Use new BF method
#define USE_AEC_MODULE              0   // AEC (usually not needed with BF)

// Dual-channel codec for two mics
#if USE_BEAMFORMING_MODULE || USE_AEC_MODULE || USE_DOA_MODULE || USE_DEREVERB_MODULE
#define HOST_CODEC_CHA_NUM  2
#endif

// UART
#define CONFIG_CI_LOG_UART HAL_UART0_BASE
#define MSG_COM_USE_UART_EN 1
#define UART_PROTOCOL_NUMBER (HAL_UART2_BASE)
#define UART_PROTOCOL_BAUDRATE (UART_BaudRate9600)
#define UART_PROTOCOL_VER 2
```

### Step 2: Configure ci_ssp_config.c

#### STFT Configuration (1024-point FFT for beamforming)

```c
#define FFT_MODEL 1024

const stft_istft_config_t stft_istft_config = {
    .sample_frequency = 32000,        // 32kHz (with resample to 16kHz)
    .frame_size = 1024,               // Window size
    .frame_shift = AUDIO_CAP_POINT_NUM_PER_FRM * 2,  // Frame shift (320 for 32kHz)
    .fft_frm_size = 1024,             // FFT input size
    .fft_size = 513,                  // FFT positive frequency bins
    .result_out_channel = 2,          // 2 output channels (L and R for BF)
    .psd_compute_channel_num = 1,     // Compute PSD on mic channel 1
    .time_pre_emphasis_enable = false,
    .downsampled_enable = true,       // Downsample 32kHz -> 16kHz
    .fe_psd_enable = true             // Enable PSD for feature computation
};
```

#### Beamforming Configuration

```c
#include "ci_bf.h"

const bf_config_t bf_config = {
    .distance = 40,               // Mic spacing in mm
    .angle = 90,                  // Enhancement angle (90 = front)
    .freq = 20,                   // Minimum frequency bin for channel selection
    .frame_wkup = 90,             // Wakeup word frame length
    .frame_rt = 40,               // Response time frame length
    .wkup_result_thr = 35,        // Wakeup minimum score threshold
    .set_bf_threshold = 500,      // BF activation threshold
    .set_bf_thr_window_size = 40, // Threshold judgment window
    #if USE_BEAMFORMING_OLD_METHOD
    .bf_new_method = 0,           // Old method
    #else
    .bf_new_method = 1,           // New method (recommended)
    #endif
};
```

#### Dereverb Configuration (recommended with BF)

```c
const dereverb_config_t dereverb_config = {
    .startHz = 160.0f,
    .endHz = 4800.0f
};
```

#### Audio Capture (dual mic)

```c
audio_capture_t audio_capture = {
    .frame_length = AUDIO_CAP_POINT_NUM_PER_FRM * 2,  // 320 for 32kHz
    .mic_channel_num = 2,    // 2 microphones
    .ref_channel_num = 0,    // No reference (no AEC)
    // MICL // mic_model;
};
```

#### Enable BF in ci_ssp struct

```c
ci_ssp_config_t ci_ssp = {
    // ...
    #if USE_BEAMFORMING_MODULE
    .bf = {.module_config = &bf_config},
    #else
    .bf = {.module_config = NULL},
    #endif

    #if USE_DEREVERB_MODULE
    .dereverb = {.module_config = &dereverb_config},
    #else
    .dereverb = {.module_config = NULL},
    #endif
    // ...
};
```

---

## BF Parameters Explained

### distance
Microphone spacing in millimeters. Must match physical hardware.

- Common values: 40mm, 50mm, 65mm
- Measure the center-to-center distance between the two microphone capsules

### angle
The enhancement direction in degrees.

| Angle | Direction |
|-------|-----------|
| 0 | Right side |
| 45 | Front-right |
| 90 | Front (center) — **recommended** |
| 135 | Front-left |
| 180 | Left side |

### freq
Minimum frequency bin for channel selection. Lower frequencies are less directional.

- Default: 20
- Increase to make BF more aggressive at higher frequencies only

### frame_wkup
Frame length for wakeup word processing.

- Default: 90 (frames)
- Longer wakeup words may need higher values

### frame_rt
Frame length for response time processing.

- Default: 40 (frames)
- Affects how quickly BF responds to direction changes

### wkup_result_thr
Minimum wakeup score threshold for BF to activate.

- Default: 35
- Lower = BF activates more easily
- Higher = BF only activates on strong wakeup

### set_bf_threshold
BF activation amplitude threshold.

- Default: 500
- Lower = BF processes more often (may process noise)
- Higher = BF only processes on stronger signals

### bf_new_method
| Value | Description |
|-------|-------------|
| 0 | Old method (legacy) |
| 1 | New method (recommended, better performance) |

---

## Hardware Requirements

### Microphone Placement

```
          Front (enhancement direction, angle=90)
              ▲
              │
         ┌────┴────┐
         │         │
    Mic L │  ← d →  │ Mic R    (d = distance parameter)
         │         │
         └─────────┘
```

- Both mics should be the same model
- Symmetric placement relative to the enhancement direction
- Typical spacing: 40-65mm
- Avoid obstructions between mics

### Codec Configuration

Two-mic requires dual-channel codec input:

```c
// HOST_CODEC_CHA_NUM must be 2
// Both ADC channels (L and R) must be enabled
// Mic L connects to MICP_L / MICN_L
// Mic R connects to MICP_R / MICN_R
```

### Board Compatibility

| Board | Two-Mic Support | Notes |
|-------|----------------|-------|
| CI-D06GT01D | Yes | Dev board, CI1306, 4MB flash |
| CI-D02GS01J | Limited | CI1302, may need hardware mod |
| Custom board | Yes | Design with dual mic pads |

---

## BF API Reference

```c
// From ci_bf.h

// Get BF version
int ci_bf_version(void);

// Create BF module
void* ci_bf_create(void* module_config);

// Process audio (frequency domain)
// handle: from ci_bf_create()
// leftbuff, rightbuff: L/R channel STFT output
// fftout_data: BF processed output
int ci_bf_deal(void *handle, float *leftbuff, float *rightbuff, float **fftout_data);

// Check if BF has processed the current frame
// Returns: 0=original audio, 1=BF processed audio
int get_bf_process_state(void);
```

---

## Combining BF with Other Algorithms

### BF + Dereverb (recommended)

```c
#define USE_BEAMFORMING_MODULE  1
#define USE_DEREVERB_MODULE     1
```

Dereverb removes room reverberation before BF processes the signal, improving directional accuracy.

### BF + Denoise

```c
#define USE_BEAMFORMING_MODULE  1
#define USE_DENOISE_MODULE      1
```

Denoise can run after BF to further reduce residual noise.

### BF + DOA (Direction of Arrival)

```c
#define USE_BEAMFORMING_MODULE  1
#define USE_DOA_MODULE          1
```

DOA estimates the sound source direction. Can be used to dynamically adjust the BF enhancement angle.

### BF + AEC

```c
#define USE_BEAMFORMING_MODULE  1
#define USE_AEC_MODULE          1
```

AEC runs before BF in the pipeline. This combination is memory-intensive; ensure sufficient heap.

---

## Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| BF not improving recognition | Wrong mic spacing | Verify `distance` matches physical measurement |
| Recognition worse with BF | Wrong enhancement angle | Set `angle=90` for front-facing mics |
| Audio sounds muffled | BF too aggressive | Increase `set_bf_threshold` |
| BF never activates | Threshold too high | Lower `set_bf_threshold` and `wkup_result_thr` |
| Insufficient memory | BF + other algorithms | Check heap, disable unused algorithms |
| One mic not working | Codec channel issue | Verify `HOST_CODEC_CHA_NUM=2` and hardware wiring |
| Intermittent BF | Frame length mismatch | Verify `frame_length` matches `AUDIO_CAP_POINT_NUM_PER_FRM * 2` |
