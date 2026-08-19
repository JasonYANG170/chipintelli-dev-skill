# Recipe: AEC (Acoustic Echo Cancellation)

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Headers: `ci_adapt_aec.h`, `ci_ssp_config.c`

> Applies to: CI1301, CI1302, CI1303, and CI1306 unless the recipe states narrower support; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/aec.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

AEC prevents the speaker's audio output from being picked up by the microphone, which would cause false ASR triggers or mask real user commands. This enables "barge-in" — the ability to speak commands while the device is playing audio.

### When to Use AEC

- Device has a speaker output (prompts, music, responses)
- User needs to speak during playback (barge-in)
- Without AEC, you must pause ASR during playback

---

## Configuration

### Step 1: Enable AEC in user_config.h

```c
// Enable AEC module
#define USE_AEC_MODULE              1

// Close headphone output when not playing (saves power, reduces noise)
#define IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY   1

// Dual-channel codec required for AEC (mic + ref channel)
#define HOST_CODEC_CHA_NUM          2
```

### Step 2: Configure AEC in ci_ssp_config.c

```c
#include "ci_adapt_aec.h"

// AEC module configuration
const aec_config_t aec_config = {
    .mic_channel_num = 1,                      // Number of microphone channels
    .ref_channel_num = 1,                       // Number of reference channels
    .aec_control_mode = COMPUTE_REF_AMPL_MODE,  // Control mode
    .aec_gain = 1.0f,                           // AEC gain
    .aec_enable_threshold = 6000.0f,            // Reference signal threshold
    .aec_mic_div_ref_thr = 0.05f,               // Mic/ref ratio threshold
    .nlp_flag = 2,                              // NLP mode (0=off, 1=mode1, 2=mode2, 3=mode2->mode1)
    .aggr_mode = 1,                             // Aggressiveness (0-2, higher=more aggressive)
    .fft_size = 256,                            // FFT size for frequency domain processing
    .alc_off_codec_adc_gain_mic = 20,           // Mic ADC gain when ALC is off
    .alc_off_codec_adc_gain_ref = 0,            // Ref ADC gain when ALC is off
};
```

### Step 3: Configure audio_capture in ci_ssp_config.c

```c
audio_capture_t audio_capture = {
    .frame_length = AUDIO_CAP_POINT_NUM_PER_FRM,  // 160 for 16kHz
    .mic_channel_num = 1,    // 1 mic channel
    .ref_channel_num = 1,    // 1 reference channel (required for AEC)
    // MICL // mic_model;
};
```

### Step 4: Enable AEC module in ci_ssp struct

```c
ci_ssp_config_t ci_ssp = {
    // ...
    #if USE_AEC_MODULE
    .aec = {.module_config = &aec_config},
    #else
    .aec = {.module_config = NULL},
    #endif
    // ...
};
```

---

## AEC Parameters Explained

### aec_control_mode

| Mode | Name | Description |
|------|------|-------------|
| 0 | `ENABLE_PLAYING_STATE_MODE` | AEC active based on playback state. Strong echo cancellation but only works when player state is known. |
| 1 | `COMPUTE_REF_AMPL_MODE` | AEC active based on reference signal amplitude. More flexible, works regardless of player state. |

**Recommended**: `COMPUTE_REF_AMPL_MODE` for most applications.

### nlp_flag (Non-Linear Processing)

| Value | Description |
|-------|-------------|
| 0 | NLP disabled (minimal distortion, weaker echo suppression) |
| 1 | NLP mode 1 (more aggressive, more distortion) |
| 2 | NLP mode 2 (balanced, recommended) |
| 3 | NLP mode 2 then mode 1 (adaptive) |

**Recommended**: `2` for balanced performance.

### aggr_mode

| Value | Description |
|-------|-------------|
| 0 | Least aggressive (minimal distortion, weaker cancellation) |
| 1 | Moderate (recommended) |
| 2 | Most aggressive (strongest cancellation, more distortion, more CPU/memory) |

### aec_enable_threshold

The reference signal amplitude threshold. When the reference signal exceeds this value, AEC processing is activated. This prevents unnecessary processing during silence.

- Default: `6000.0f`
- Increase if AEC is too aggressive on quiet sounds
- Decrease if echo is not being cancelled

### aec_mic_div_ref_thr

The mic-to-reference ratio threshold. When the mic signal divided by the reference signal is below this value, the signal is considered echo.

- Default: `0.05f`
- Lower value = more lenient (more signal passes through)
- Higher value = more aggressive echo detection

---

## How AEC Integrates with ASR

### Signal Flow with AEC

```
Mic ──► Codec ADC ──► STFT ──► AEC ◄── Ref Signal (from DAC)
                                    │
                                    ▼
                              [Denoise] ──► [BF] ──► ASR
```

### AEC and Playback Interaction

The system manages AEC-playback interaction in `system_msg_deal.c`:

```c
#if USE_AEC_MODULE
// When ASR is paused (e.g., during critical playback)
void pause_asr(uint8_t voice_in_mute, uint8_t pause_asr_task)
{
    // ...
    // Intercept ASR output to prevent false results during transition
    ciss_set(CI_SS_INTERCEPT_ASR_OUT, 1);
}

// When ASR is resumed
void resume_asr()
{
    // ...
    // Delay re-enabling ASR output to allow AEC to converge
    if (sys_manage_data.intercept_timer_handle == NULL)
    {
        sys_manage_data.intercept_timer_handle = xTimerCreate(
            "intercept_timer", pdMS_TO_TICKS(500), pdFALSE, 0, intercept_timer_callback);
    }
    xTimerStart(sys_manage_data.intercept_timer_handle, 0);
}

// Timer callback: re-enable ASR output after 500ms
void intercept_timer_callback(TimerHandle_t xTimer)
{
    ciss_set(CI_SS_INTERCEPT_ASR_OUT, 0);
    xTimerStop(sys_manage_data.intercept_timer_handle, 0);
}
#endif
```

---

## Tuning AEC

### Step 1: Verify Hardware

- Microphone and speaker are properly placed (minimum coupling)
- Reference signal is properly routed from DAC to AEC
- `HOST_CODEC_CHA_NUM=2` is set

### Step 2: Test Without Playback

1. Disable AEC temporarily
2. Verify ASR works without playback
3. Re-enable AEC

### Step 3: Test With Playback

1. Start playing a prompt
2. Speak a command word during playback
3. Verify the command is recognized (barge-in)

### Step 4: Tune Parameters

If echo is not cancelled:
- Increase `aggr_mode` to 2
- Increase `aec_enable_threshold` if false triggering
- Change `nlp_flag` to 3 (adaptive mode)

If voice sounds distorted:
- Decrease `aggr_mode` to 0
- Change `nlp_flag` to 0 or 1
- Lower `aec_gain` (e.g., 0.8f)

---

## Alternative: Pause ASR During Playback

If AEC is not available or not sufficient, pause ASR during playback:

```c
// Before playback
pause_asr(1, 1);  // Mute mic and pause ASR task

// Play prompt
play_prompt(data_addr, data_addr_num, my_callback);

// In callback (after playback completes):
void my_play_done_callback(cmd_handle_t cmd_handle)
{
    resume_asr();  // Resume ASR
}
```

---

## AEC API Reference

### Core Functions (from ci_adapt_aec.h)

```c
// Get AEC version
int ci_adapt_aec_version(void);

// Create AEC module instance
void* ci_aec_create(void* module_config);

// Destroy AEC module instance
void* ci_adapt_aec_destroy(void *handle);

// Process audio (frequency domain)
int ci_adapt_aec_deal(void* handle, float** fft_mic_in, float** fft_ref_in, float** fft_out);

// Set NLP flag at runtime
void set_nlp_flag_api(int flag);

// Set AEC parameters at runtime
void set_para_api(int alpha_l, int deta_d);
```

---

## Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| Echo not cancelled | Reference signal not connected | Verify `ref_channel_num=1` and hardware routing |
| Voice sounds distorted | AEC too aggressive | Lower `aggr_mode`, change `nlp_flag` to 0 or 1 |
| False ASR triggers during playback | AEC threshold too high | Lower `aec_enable_threshold` |
| ASR stops working after enabling AEC | Codec channel mismatch | Set `HOST_CODEC_CHA_NUM=2` |
| Barge-in doesn't work | AEC intercept timer too long | Check `intercept_timer` (500ms default) |
| Audio quality degrades | NLP too aggressive | Set `nlp_flag=0` to disable NLP |
