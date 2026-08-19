# Recipe: Audio Player (CI13LC)

> SDK: `CI13LC_SDK_V2.0.15`
> Chips: CI1311, CI1312, CI1316, CI1324, CI1332

---

## Overview

The CI13LC SDK provides a simple MP3 player (`simple_mp3_player`) for audio playback from flash, and a `codec_manager` for codec (ADC/DAC) configuration. The player supports MP3 decoding and volume/PA control.

---

## SDK

`CI13LC_SDK_V2.0.15` - components: `player/simple_mp3_player`, `codec_manager`

---

## Source Anchors

- `components/player/simple_mp3_player/simple_mp3_player.h` - player API
- `components/codec_manager/codec_manager.h` - codec management API

---

## API Usage

## Simple MP3 Player (simple_mp3_player.h)

### Audio Format Info

```c
typedef struct {
    uint32_t samprate;       // Sample rate
    uint8_t  nChans;         // Channel count
    int32_t  out_min_size;   // Frame length
} audio_format_info_t;
```

### Message IDs

```c
typedef enum {
    SMP_MSG_START_PLAY = 0,  // Start playing (stops current if busy)
    SMP_MSG_STOP_PLAY,        // Force stop
} smp_msg_id_t;
```

### Callback Type

```c
typedef void(*SMP_PLAY_END_CALLBACK)(int32_t arg);
```

### Functions

```c
// Initialize the simple MP3 player module
// Returns 1 on success, other on failure
int smp_init(void);

// Start playing MP3 data from flash address
// data_addr: flash address of MP3 data
// play_end_callback: called when playback completes
// Returns 1 on success, other on failure
int smp_play(uint32_t data_addr, void* play_end_callback);

// Stop playback
void smp_stop(void);

// Control PA (power amplifier) and DAC
// cmd: ENABLE to turn on, DISABLE to turn off
// is_control_pa: true=control PA, false=only DAC
void audio_play_hw_pa_da_ctl(FunctionalState cmd, bool is_control_pa);

// Set volume gain
// gain: hardware gain value
void audio_play_set_vol_gain(int32_t gain);
```

## Codec Manager (codec_manager.h)

### Over-Sampling Rate

```c
typedef enum {
    AUDIO_PLAY_OVER_SAMPLE_128 = 128,
    AUDIO_PLAY_OVER_SAMPLE_192 = 192,
    AUDIO_PLAY_OVER_SAMPLE_256 = 256,
    AUDIO_PLAY_OVER_SAMPLE_384 = 384,
} audio_play_card_over_sample_t;
```

### Clock Source

```c
typedef enum {
    AUDIO_PLAY_CLK_SOURCE_IPCORE       = 0,
    AUDIO_PLAY_CLK_SOURCE_EXT_OSC       = 1,
    AUDIO_PLAY_CLK_SOURCE_INTER_RC      = 2,
    AUDIO_PLAY_CLK_SOURCE_PAD_IN        = 3,
    AUDIO_PLAY_CLK_SOURCE_OSC_OR_INEER_RC = 0xff,
} audio_play_card_clk_source_t;
```

### IIS Direction

```c
typedef enum {
    CM_IIS_TX,  // IIS TX
    CM_IIS_RX,  // IIS RX
} cm_iis_txrx_t;
```

### IO Direction

```c
typedef enum {
    CODEC_INPUT,   // ADC
    CODEC_OUTPUT,  // DAC
} io_direction_t;
```

### IO Control Commands

```c
typedef enum {
    CM_IOCTRL_SET_DAC_GAIN,     // param1: left gain, param2: right gain
    CM_IOCTRL_SET_ADC_GAIN,     // param1: left gain, param2: right gain
    CM_IOCTRL_ALC_ENABLE,       // param1: alc enable, param2: control PGA
    CM_IOCTRL_ALC_DISABLE,      // param1: alc enable, param2: control PGA
    CM_IOCTRL_DAC_ENABLE,       // param1: channel, param2: enable
    CM_IOCTRL_MUTE,             // param1: channel, param2: enable
} cm_io_ctrl_cmd_t;
```

### Channel Selection

```c
typedef enum {
    CM_CHA_LEFT     = 1,
    CM_CHA_RIGHT    = 2,
    CM_CHA_TWO_CHA  = 3,
} cm_cha_sel_t;
```

### Sound Info

```c
typedef struct {
    uint32_t sample_rate;              // Sample rate
    iis_data_width_t sample_depth;     // Bit depth
    uint8_t channel_flag;              // bit[0]=left, bit[1]=right
} cm_sound_info_t;
```

### Buffer Info

```c
typedef struct {
    void *pcm_buffer;       // Total buffer
    uint16_t block_size;    // Block size in bytes
    uint16_t buffer_size;   // Buffer size in bytes
    uint8_t block_num;      // Block count
    uint8_t buffer_num;     // Buffer count
} cm_play_buffer_info_t;

typedef struct {
    void *pcm_buffer;
    uint16_t buffer_size;
    uint16_t block_size;
    uint8_t block_num;
} cm_record_buffer_info_t;

typedef union {
    cm_play_buffer_info_t play_buffer_info;
    cm_record_buffer_info_t record_buffer_info;
} cm_pcm_buffer_info_t;
```

### Codec Interface

```c
typedef struct {
    int (*codec_init)(cm_codec_hw_info_t*);
    int (*codec_config)(cm_sound_info_t *audio_info, io_direction_t io_dir);
    int (*codec_start)(io_direction_t);
    int (*codec_stop)(io_direction_t);
    int (*codec_ioctl)(io_direction_t, uint32_t, uint32_t, uint32_t);
} cm_codec_interface_t;
```

### Core Functions

```c
// Initialize codec manager
void cm_init(void);

// Register a codec at given index (0 to MAX_CODEC_NUM-1)
int cm_reg_codec(int codec_index, cm_codec_hw_info_t *p_codec_hw_info);

// Register callback for async events
int cm_register_codec_callback(int codec_index, void (*callback_func)(void));

// Configure PCM buffer
int cm_config_pcm_buffer(int codec_index, io_direction_t io_dir,
                          cm_pcm_buffer_info_t *pcm_buffer_info);

// Configure codec sound parameters
int cm_config_codec(int codec_index, io_direction_t io_dir, cm_sound_info_t *sound_info);

// Start codec
int cm_start_codec(int codec_index, io_direction_t io_dir);

// Stop codec
int cm_stop_codec(int codec_index, io_direction_t io_dir);

// Read recorded data from codec
int cm_read_codec(int codec_index, uint32_t *data_addr, uint32_t *data_size, uint32_t wait_tick);

// Write PCM data for playback
int cm_write_codec(int codec_index, void *pcm_buffer, uint32_t wait_tick);

// Get/release PCM buffer
void cm_get_pcm_buffer(int codec_index, uint32_t* ret_buf, uint32_t wait_tick);
int cm_release_pcm_buffer(int codec_index, io_direction_t io_dir, void *pcm_buffer);

// Gain control
int cm_set_codec_dac_gain(int codec_index, cm_cha_sel_t cha, int gain);
int cm_set_codec_adc_gain(int codec_index, cm_cha_sel_t cha, int gain);

// ALC control
int cm_set_codec_alc(int codec_index, cm_cha_sel_t cha, FunctionalState alc_enable);

// DAC channel enable
int cm_set_codec_dac_enable(int codec_index, int channel, FunctionalState en);

// Mute control
int cm_set_codec_mute(int codec_index, io_direction_t io_dir, int channel_flag, FunctionalState en);

// Buffer status
int cm_get_codec_empty_buffer_number(int codec_index, io_direction_t io_dir);
int cm_get_codec_busy_buffer_number(int codec_index, io_direction_t io_dir);

// Inner codec built-in functions
int icodec_init(cm_codec_hw_info_t *codec_hw_info);
int icodec_start(io_direction_t io_dir);
int icodec_config(cm_sound_info_t *audio_info, io_direction_t io_dir);
int icodec_stop(io_direction_t io_dir);
int icodec_ioctl(io_direction_t io_dir, uint32_t param0, uint32_t param1, uint32_t param2);
```

---

## Usage Example

### Initialize Player

```c
#include "simple_mp3_player.h"

void audio_player_init(void)
{
    // Initialize the simple MP3 player
    if (smp_init() != 1)
    {
        // Handle error
        return;
    }

    // Set default volume gain
    audio_play_set_vol_gain(50);

    // Enable PA
    audio_play_hw_pa_da_ctl(ENABLE, true);
}
```

### Play Audio from Flash

```c
// Callback when playback completes
void play_done_cb(int32_t arg)
{
    // Playback finished
}

void play_voice_prompt(uint32_t flash_addr)
{
    // Play MP3 data from flash address
    smp_play(flash_addr, play_done_cb);
}
```

### Stop Playback

```c
void stop_audio(void)
{
    smp_stop();
    audio_play_hw_pa_da_ctl(DISABLE, true);
}
```

### Adjust Volume

```c
void set_volume(uint8_t level)
{
    // Map level (0-100) to hardware gain
    int32_t gain = (level * 85) / 100 + 7;
    audio_play_set_vol_gain(gain);
}
```

### Register and Configure Codec via codec_manager

```c
#include "codec_manager.h"

#define HOST_MIC_RECORD_CODEC_ID  0
#define AUDIO_PLAY_CODEC_ID       1

void codec_manager_setup(void)
{
    // Initialize codec manager
    cm_init();

    // Register inner codec for recording (ADC)
    cm_codec_hw_info_t codec_hw_info;
    // ... fill in IIC/IIS/codec interface fields ...
    icodec_init(&codec_hw_info);

    cm_reg_codec(HOST_MIC_RECORD_CODEC_ID, &codec_hw_info);

    // Configure ADC gain
    cm_set_codec_adc_gain(HOST_MIC_RECORD_CODEC_ID, CM_CHA_LEFT, 20);
    cm_set_codec_alc(HOST_MIC_RECORD_CODEC_ID, CM_CHA_LEFT, DISABLE);
}
```

---

## Notes/Tips

- `smp_init()` must be called before any playback. It creates the player FreeRTOS task.
- `smp_play()` takes a flash address (uint32_t) directly, not a file path. The MP3 data must be pre-loaded into flash.
- The callback function is called from the player task context; keep it short.
- `audio_play_hw_pa_da_ctl(ENABLE, true)` turns on both PA and DAC. Use `is_control_pa=false` to only control DAC.
- Volume gain range is typically 7-85 (hardware register values).
- `codec_manager` (`cm_*` functions) is the higher-level API for managing multiple codecs. Use `cm_set_codec_adc_gain()` for recording gain and `cm_set_codec_dac_gain()` for playback gain.
- `MAX_CODEC_NUM` is 2: typically index 0 for recording codec, index 1 for playback codec.
- When ASR is running, pause ASR during critical playback to avoid resource conflicts: use `pause_asr(1, 1)` before and `resume_asr()` after.
- The inner codec built-in functions (`icodec_*`) are pre-registered implementations for the on-chip CODEC; use them through `cm_reg_codec()`.
