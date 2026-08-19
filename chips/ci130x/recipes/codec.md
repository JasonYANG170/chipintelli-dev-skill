# Recipe: Inner Codec (PDM/ADC Audio)

> SDK: `CI130X_SDK_Offline_V2.1.14`
> Chips: CI1302, CI1303, CI1306, CI1312

---

## Overview

The CI130X has two audio input front-ends:
- **Inner CODEC** (`ci130x_codec.h`): ADC-based audio capture with programmable gain, ALC (Automatic Level Control), high-pass filter, and I2S/PCM data interface.
- **PDM** (`ci130x_pdm.h`): PDM (Pulse Density Modulation) microphone interface with similar feature set.

Both provide microphone amplification, ALC, digital gain, and DAC output. In typical SDK projects, the codec is initialized indirectly through `audio_in_codec_registe()` and `codec_manager`, but the low-level APIs are available for direct configuration.

---

## SDK

`CI130X_SDK_Offline_V2.1.14` - driver: `ci130x_chip_driver`

---

## Source Anchors

- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_codec.h` - Inner CODEC driver
- `ci130x_sdk/driver/ci130x_chip_driver/inc/ci130x_pdm.h` - PDM driver

---

## API Usage

## Inner CODEC (ci130x_codec.h)

### Key Enums

```c
// Master/slave mode
typedef enum {
    INNER_CODEC_MODE_MASTER = 3,
    INNER_CODEC_MODE_SLAVE  = 0,
} inner_codec_mode_t;

// MIC input mode
typedef enum {
    INNER_CODEC_INPUT_MODE_DIFF            = 1,  // Differential
    INNER_CODEC_INPUT_MODE_SINGGLE_ENDED   = 2,  // Single-ended
} inner_codec_input_mode_t;

// MIC gain (PGA)
typedef enum {
    INNER_CODEC_MIC_AMP_0dB  = 0,
    INNER_CODEC_MIC_AMP_6dB,
    INNER_CODEC_MIC_AMP_9dB,
    INNER_CODEC_MIC_AMP_12dB,
    INNER_CODEC_MIC_AMP_16dB,
    INNER_CODEC_MIC_AMP_20dB,
} inner_codec_mic_amplify_t;

// Sample rate
typedef enum {
    INNER_CODEC_SAMPLERATE_96K   = 0,
    INNER_CODEC_SAMPLERATE_48K   = 1,
    INNER_CODEC_SAMPLERATE_44_1K = 2,
    INNER_CODEC_SAMPLERATE_32K   = 3,
    INNER_CODEC_SAMPLERATE_24K   = 4,
    INNER_CODEC_SAMPLERATE_16K   = 5,  // Typical for ASR
    INNER_CODEC_SAMPLERATE_12K   = 6,
    INNER_CODEC_SAMPLERATE_8K    = 7,
} inner_codec_samplerate_t;

// Channel selection
typedef enum {
    INNER_CODEC_LEFT_CHA  = 0,
    INNER_CODEC_RIGHT_CHA = 1,
} inner_codec_cha_sel_t;

// Global enable/disable
typedef enum {
    INNER_CODEC_GATE_ENABLE  = 1,
    INNER_CODEC_GATE_DISABLE = 0,
} inner_cedoc_gate_t;

// MIC bias voltage
typedef enum {
    INNER_CODEC_MIC_BIAS_1_0 = 0,  // 1.0 * (AVDD/2)
    INNER_CODEC_MIC_BIAS_1_1 = 1,
    // ... up to INNER_CODEC_MIC_BIAS_1_7 = 7
} inner_codec_micbias_t;
```

### ADC Configuration Structure

```c
typedef struct {
    inner_codec_input_mode_t  codec_adc_input_mode_l;  // Left channel input mode
    inner_codec_input_mode_t  codec_adc_input_mode_r;  // Right channel input mode
    inner_codec_mic_amplify_t codec_adc_mic_amp_l;     // Left MIC gain
    inner_codec_mic_amplify_t codec_adc_mic_amp_r;     // Right MIC gain
    float pga_gain_l;  // Left PGA gain (when ALC disabled)
    float pga_gain_r;  // Right PGA gain
    float dig_gain_l;  // Left digital gain
    float dig_gain_r;  // Right digital gain
} inner_codec_adc_config_t;
```

### Core Functions

```c
// Power and reset
void inner_codec_reset(void);
void inner_codec_power_up(inner_codec_current_t current);
void inner_codec_power_off(void);

// High-pass filter
void inner_codec_hp_filter_config(inner_cedoc_gate_t gate, inner_codec_highpass_cut_off_t Hz);

// ADC control
void inner_codec_adc_enable(inner_codec_adc_config_t *ADC_Config);
void inner_codec_adc_disable(inner_codec_cha_sel_t cha, inner_cedoc_gate_t EN);

// DAC control
void inner_codec_dac_enable(bool is_first_enable);
void inner_codec_dac_disable(void);

// Mode configuration
void inner_codec_adc_mode_set(inner_codec_mode_t mode,
                              inner_codec_frame_1_2len_t frame_Len,
                              inner_codec_valid_word_len_t word_len,
                              inner_codec_i2s_data_famat_t data_fram);

void inner_codec_dac_mode_set(inner_codec_mode_t mode,
                              inner_codec_frame_1_2len_t frame_Len,
                              inner_codec_valid_word_len_t word_len,
                              inner_codec_i2s_data_famat_t data_fram);

// ALC control
void inner_codec_alc_disable(inner_codec_cha_sel_t cha);
void inner_codec_left_alc_enable(inner_cedoc_gate_t gate,
                                  inner_codec_use_alc_control_pgagain_t is_alc_ctr_pga);
void inner_codec_right_alc_enable(inner_cedoc_gate_t gate,
                                   inner_codec_use_alc_control_pgagain_t is_alc_ctr_pga);
void inner_codec_left_alc_pro_mode_config(inner_codec_alc_config_t* ALC_Type);
void inner_codec_right_alc_pro_mode_config(inner_codec_alc_config_t* ALC_Type);

// MIC bias
void inner_codec_micbias_set(inner_codec_micbias_t bias);
```

## PDM (ci130x_pdm.h)

### Key Enums (mirror CODEC with PDM_ prefix)

```c
typedef enum {
    PDM_MODE_MASTER = 3,
    PDM_MODE_SLAVE  = 0,
} pdm_mode_t;

typedef enum {
    PDM_INPUT_MODE_DIFF          = 1,
    PDM_INPUT_MODE_SINGGLE_ENDED = 2,
} pdm_input_mode_t;

typedef enum {
    PDM_MIC_AMP_0dB  = 0,
    PDM_MIC_AMP_6dB  = 1,
    PDM_MIC_AMP_13dB = 2,
    PDM_MIC_AMP_20dB = 3,
} pdm_mic_amplify_t;

typedef enum {
    PDM_SAMPLERATE_16K = 5,  // Typical for ASR
    // ... other rates same as CODEC
} pdm_samplerate_t;

typedef enum {
    PDM_LEFT_CHA  = 0,
    PDM_RIGHT_CHA = 1,
} pdm_cha_sel_t;
```

### PDM ADC Configuration

```c
typedef struct {
    pdm_input_mode_t   codec_adc_input_mode_l;
    pdm_input_mode_t   codec_adc_input_mode_r;
    pdm_mic_amplify_t  codec_adc_mic_amp_l;
    pdm_mic_amplify_t  codec_adc_mic_amp_r;
    float pga_gain_l;
    float pga_gain_r;
} pdm_adc_config_t;
```

### PDM Core Functions

```c
void pdm_reset(void);
void pdm_power_up(pdm_current_t current);
void pdm_power_off(void);
void pdm_hightpass_config(pdm_gate_t gate, pdm_highpass_cut_off_t Hz);
void pdm_adc_enable(pdm_adc_config_t *ADC_Config);
void pdm_adc_disable(pdm_cha_sel_t cha, pdm_gate_t EN);
void pdm_dac_enable(void);
void pdm_dac_disable(pdm_cha_sel_t cha, pdm_gate_t EN);
void pdm_dac_gain_set(int32_t l_gain, int32_t r_gain);

// ALC
void pdm_alc_disable(pdm_cha_sel_t cha, float ALC_Gain);
void pdm_left_alc_enable(pdm_gate_t gate, pdm_use_alc_control_pgagain_t is_alc_ctr_pga);
void pdm_right_alc_enable(pdm_gate_t gate, pdm_use_alc_control_pgagain_t is_alc_ctr_pga);
void pdm_alc_left_config(pdm_alc_use_config_t* ALC_str);
void pdm_alc_right_config(pdm_alc_use_config_t* ALC_str);

// Gain and mode
void pdm_adc_mode_set(pdm_mode_t mode, pdm_frame_1_2len_t frame_Len,
                       pdm_valid_word_len_t word_len, pdm_i2s_data_famat_t data_fram);
void pdm_dac_mode_set(pdm_mode_t mode, pdm_frame_1_2len_t frame_Len,
                       pdm_valid_word_len_t word_len, pdm_i2s_data_famat_t data_fram);

// MIC gain
pdm_mic_amplify_t pdm_get_mic_gain(pdm_cha_sel_t cha);
void pdm_set_mic_gain(pdm_cha_sel_t cha, pdm_mic_amplify_t gain);
void pdm_set_mic_gain_left(pdm_mic_amplify_t gain);
void pdm_set_mic_gain_right(pdm_mic_amplify_t gain);

// PGA gain (direct register access)
void pdm_pga_gain_config_via_reg43_53(pdm_cha_sel_t cha, uint32_t gain);
void pdm_pga_gain_config_via_reg43_53_db(pdm_cha_sel_t cha, float gain_db);
void pdm_pga_gain_config_via_reg27_28(pdm_cha_sel_t cha, uint32_t gain);
void pdm_pga_gain_config_via_reg27_28_db(pdm_cha_sel_t cha, float gain_db);

// Digital gain
void pdm_adc_dig_gain_set_left(uint8_t gain);
void pdm_adc_dig_gain_set_right(uint8_t gain);

// Input mode
void pdm_set_input_mode_left(pdm_input_mode_t mode);
void pdm_set_input_mode_right(pdm_input_mode_t mode);

// HPOUT mute
void pdm_hpout_mute(void);
void pdm_hpout_mute_disable(void);

// Sample rate
void pdm_set_sample_rate(pdm_samplerate_t samplerate);
```

---

## Usage Example

### Inner CODEC ADC Configuration

```c
#include "ci130x_codec.h"

void codec_adc_setup(void)
{
    // Power up codec
    inner_codec_power_up(INNER_CODEC_CURRENT_8I);

    // Configure high-pass filter at 20Hz (remove DC offset)
    inner_codec_hp_filter_config(INNER_CODEC_GATE_ENABLE,
                                  INNER_CODEC_HIGHPASS_CUT_OFF_20HZ);

    // ADC configuration
    inner_codec_adc_config_t adc_cfg;
    adc_cfg.codec_adc_input_mode_l = INNER_CODEC_INPUT_MODE_DIFF;  // Differential
    adc_cfg.codec_adc_input_mode_r = INNER_CODEC_INPUT_MODE_DIFF;
    adc_cfg.codec_adc_mic_amp_l    = INNER_CODEC_MIC_AMP_20dB;    // Max MIC gain
    adc_cfg.codec_adc_mic_amp_r    = INNER_CODEC_MIC_AMP_20dB;
    adc_cfg.pga_gain_l = 0.0f;  // PGA gain (when ALC disabled)
    adc_cfg.pga_gain_r = 0.0f;
    adc_cfg.dig_gain_l = 0.0f;  // Digital gain
    adc_cfg.dig_gain_r = 0.0f;

    inner_codec_adc_enable(&adc_cfg);

    // Set ADC mode: master, 32-bit frame, 16-bit data, I2S format
    inner_codec_adc_mode_set(INNER_CODEC_MODE_MASTER,
                              INNER_CODEC_FRAME_LEN_32BIT,
                              INNER_CODEC_VALID_LEN_16BIT,
                              INNER_CODEC_I2S_DATA_FORMAT_I2S_MODE);

    // Set MIC bias to 1.2 * AVDD/2
    inner_codec_micbias_set(INNER_CODEC_MIC_BIAS_1_2);
}
```

### PDM Microphone Configuration

```c
#include "ci130x_pdm.h"

void pdm_adc_setup(void)
{
    // Power up PDM
    pdm_power_up(PDM_CURRENT_8I);

    // High-pass filter at 20Hz
    pdm_hightpass_config(PDM_GATE_ENABLE, PDM_HIGHPASS_CUT_OFF_20HZ);

    // ADC configuration
    pdm_adc_config_t adc_cfg;
    adc_cfg.codec_adc_input_mode_l = PDM_INPUT_MODE_DIFF;
    adc_cfg.codec_adc_input_mode_r = PDM_INPUT_MODE_DIFF;
    adc_cfg.codec_adc_mic_amp_l   = PDM_MIC_AMP_20dB;
    adc_cfg.codec_adc_mic_amp_r   = PDM_MIC_AMP_20dB;
    adc_cfg.pga_gain_l = 0.0f;
    adc_cfg.pga_gain_r = 0.0f;

    pdm_adc_enable(&adc_cfg);

    // Set sample rate to 16kHz (standard for ASR)
    pdm_set_sample_rate(PDM_SAMPLERATE_16K);

    // Set ADC mode
    pdm_adc_mode_set(PDM_MODE_MASTER,
                      PDM_FRAME_LEN_32BIT,
                      PDM_VALID_LEN_16BIT,
                      PDM_I2S_DATA_FORMAT_I2S_MODE);
}
```

### Adjust MIC Gain at Runtime

```c
// Using inner CODEC
void codec_set_mic_gain(inner_codec_mic_amplify_t gain)
{
    // Note: inner_codec_adc_config_t is set at init time.
    // For runtime changes, re-configure ADC or use PGA gain.
}

// Using PDM (more flexible runtime gain control)
void pdm_adjust_gain(float gain_db)
{
    // Set PGA gain in dB directly
    pdm_pga_gain_config_via_reg43_53_db(PDM_LEFT_CHA, gain_db);
    pdm_pga_gain_config_via_reg43_53_db(PDM_RIGHT_CHA, gain_db);
}
```

### DAC Output (Audio Playback)

```c
// Inner CODEC DAC
void codec_dac_setup(void)
{
    inner_codec_dac_enable(true);  // is_first_enable=true on first call

    // Set DAC mode: master, 32-bit frame, 16-bit data, I2S format
    inner_codec_dac_mode_set(INNER_CODEC_MODE_MASTER,
                              INNER_CODEC_FRAME_LEN_32BIT,
                              INNER_CODEC_VALID_LEN_16BIT,
                              INNER_CODEC_I2S_DATA_FORMAT_I2S_MODE);
}

// PDM DAC
void pdm_dac_setup(void)
{
    pdm_dac_enable();
    pdm_dac_mode_set(PDM_MODE_MASTER,
                      PDM_FRAME_LEN_32BIT,
                      PDM_VALID_LEN_16BIT,
                      PDM_I2S_DATA_FORMAT_I2S_MODE);
}
```

---

## Notes/Tips

- In most SDK projects, codec initialization is handled by `codec_manager` (`cm_init()`, `audio_in_codec_registe()`), not by direct calls to these low-level APIs. Use the low-level APIs only when you need custom configuration.
- **16kHz sample rate** is the standard for ASR voice recognition. Use `INNER_CODEC_SAMPLERATE_16K` or `PDM_SAMPLERATE_16K`.
- **Differential mode** (`INPUT_MODE_DIFF`) provides better noise immunity and is used by most development boards. Single-ended mode saves a pin but is noisier.
- MIC bias voltage depends on the microphone specifications. Typical electret mics use `MIC_BIAS_1_2` (1.2 * AVDD/2).
- PDM gain can be adjusted at runtime via `pdm_pga_gain_config_via_reg43_53_db()` or `pdm_set_mic_gain()`.
- The ALC (Automatic Level Control) adjusts PGA gain automatically based on input signal level. Enable it with `pdm_left_alc_enable()` or configure it in detail with `pdm_alc_left_config()`.
- `inner_codec_dac_enable(true)` must be called with `is_first_enable=true` the first time; subsequent calls use `false`.
- The PDM and inner CODEC share some I2S resources; do not use both simultaneously for the same I2S interface.
