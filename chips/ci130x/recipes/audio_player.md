# Recipe: Audio Player

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Header: `audio_play_api.h`, `prompt_player.h`

> Applies to: CI1301, CI1302, CI1303, and CI1306 unless the recipe states narrower support; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci130x/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci130x/recipes/audio_player.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

The CI130X audio player supports multiple audio formats and is used for:
- Playing voice prompts (responses to ASR commands)
- Playing MP3 files
- Volume control
- PA (power amplifier) control

---

## Configuration

### user_config.h

```c
// Enable audio player
#define AUDIO_PLAYER_ENABLE         1

// PA control: 0=always on, 1=player controls PA
#define PLAYER_CONTROL_PA           0

// Volume range
#define VOLUME_MAX                  7
#define VOLUME_MIN                  1
#define VOLUME_DEFAULT              5

// Decoders
#if AUDIO_PLAYER_ENABLE
#define USE_PROMPT_DECODER          1   // Prompt format (proprietary)
#define USE_MP3_DECODER             1   // MP3 format
#define AUDIO_PLAY_SUPPT_MP3_PROMPT 1   // MP3 prompt support
#endif
```

### source_file.prj

Ensure these source files are included:

```
source-file: components/player/audio_play/audio_play_api.c
source-file: components/player/audio_play/audio_play_decoder.c
source-file: components/player/audio_play/audio_play_process.c
source-file: components/player/audio_play/audio_play_os_port.c
source-file: components/player/audio_play/audio_play_device.c
source-file: components/player/audio_play/get_play_data.c
source-file: components/player/adpcm/adpcmdec.c
source-file: components/player/adpcm/adpcm.c
source-file: components/player/m4a/parse_m4a_atom_containers_port.c
source-file: components/player/m4a/parse_m4a_atom_containers.c
source-file: components/player/flacdec/bitstreamf.c
source-file: components/player/flacdec/flacdecoder.c
source-file: components/player/flacdec/tables.c
source-file: components/cmd_info/prompt_player.c
```

---

## Initialization

The player is initialized in `main.c` `task_init()`:

```c
#if AUDIO_PLAYER_ENABLE
audio_play_init();  // Create player task
#endif
```

Volume is restored from NVData in `UserTaskManageProcess()`:

```c
case SYS_MSG_TYPE_AUDIO_IN_STARTED:
{
    uint8_t volume;
    uint16_t real_len;

    // Read saved volume from NVData
    if (CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len))
    {
        volume = VOLUME_DEFAULT;
        cinv_item_init(NVDATA_ID_VOLUME, sizeof(volume), &volume);
    }
    vol_set(volume);
    break;
}
```

---

## Playing Prompts

### Play by Command Handle

When an ASR command is recognized, play its associated prompt:

```c
#include "prompt_player.h"

// Play prompt associated with a recognized command
// cmd_handle: from ASR result
// select_index: -1=default, 0/1=variant selection
// callback: called when playback completes
// mute_mic: true=mute mic during playback (recommended without AEC)
prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback, true);
```

### Play by Command String

```c
// Play by command string name (defined in cmd_info)
prompt_play_by_cmd_string("<welcome>", -1, NULL, true);
prompt_play_by_cmd_string("<inactivate>", -1, play_exit_wakeup_done_cb, false);
```

### Play Done Callback

```c
// Default callback (does nothing)
void default_play_done_callback(cmd_handle_t cmd_handle)
{
    // Called when playback completes
    // Add post-playback logic here
}

// Custom callback for wakeup entry
// (play_enter_wakeup_done_cb is defined in system_msg_deal.c)
void play_enter_wakeup_done_cb(cmd_handle_t cmd_handle)
{
    // Send message to switch to command model
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
}
```

---

## Playing Audio Files

### Play Audio File

```c
#include "audio_play_api.h"

// Play audio file from flash
// dir_or_url: directory path (e.g., "voice/src")
// name: file name (e.g., "my_audio")
// offset: start offset in file (0 = beginning)
// decoder_name: "mp3", "prompt", "adpcm", "m4a", "flac"
// callback: called when playback completes
int32_t ret = play_audio("voice/src", "my_audio", 0, "mp3", my_play_done_callback);
```

### Play Prompt Data

```c
// Play prompt data at specific address
// data_addr: flash address of prompt data
// data_addr_num: number of data blocks
// callback: completion callback
play_prompt(data_addr, data_addr_num, my_play_done_callback);
```

### Stop and Pause

```c
// Stop playback
stop_play(my_stop_done_callback, NULL);

// Pause playback
pause_play(my_pause_done_callback, NULL);

// Continue from paused position
continue_history_play(my_continue_done_callback);
```

### Get Playback State

```c
// Get current playback state
audio_play_state_t state = get_audio_play_state();
if (state != AUDIO_PLAY_STATE_IDLE)
{
    // Currently playing
}

// Check if currently playing (boolean)
bool playing = check_current_playing();

// Get current playback offset
uint32_t offset = get_play_offset();
```

---

## Volume Control

### Set Volume

```c
// Volume is set through the system volume manager
// Values range from VOLUME_MIN to VOLUME_MAX

// Increase volume
uint8_t vol = vol_set(vol_get() + 1);

// Decrease volume
uint8_t vol = vol_set(vol_get() - 1);

// Set to specific level
vol_set(VOLUME_MAX);
vol_set(VOLUME_MID);
vol_set(VOLUME_MIN);
```

### Direct Hardware Volume

```c
// Set raw hardware volume gain (7-74 range)
audio_play_set_vol_gain(67 * vol / VOLUME_MAX + 7);

// Get current hardware gain
int32_t gain = audio_play_get_vol_gain();

// Mute/unmute
audio_play_set_mute(true);   // Mute
audio_play_set_mute(false);  // Unmute
```

### Volume Persistence

Volume is saved to NVData and restored on boot:

```c
// Save volume to NVData
cinv_item_write(NVDATA_ID_VOLUME, sizeof(vol), &vol);

// Read volume from NVData
uint8_t volume;
uint16_t real_len;
cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len);
```

---

## Playback Speed

```c
// Set playback speed (1.0 = normal, 0.5 = half speed, 2.0 = double speed)
set_play_speed(1.0f);   // Normal
set_play_speed(0.75f);  // Slower
set_play_speed(1.5f);   // Faster
```

---

## PA (Power Amplifier) Control

### Board PA Control

```c
// Turn PA on/off
power_amplifier_on();   // Enable power amplifier
power_amplifier_off();  // Disable power amplifier
```

### Player-Controlled PA

With `PLAYER_CONTROL_PA=1`, the player automatically controls the PA:

```c
// Control PA and DAC
// cmd: ENABLE to turn on, DISABLE to turn off
// is_control_pa: true=control PA, false=only DAC
audio_play_hw_pa_da_ctl(ENABLE, true);   // Turn on PA and DAC
audio_play_hw_pa_da_ctl(DISABLE, true);  // Turn off PA and DAC
```

---

## Voice Prompt Files

### File Naming Convention

Voice prompt files are named by command ID:

```
firmware/voice/src/
  [0]谢谢使用.wav          # cmd_id 0: "Thank you for using"
  [1]你好.wav              # cmd_id 1: "Hello"
  [2]好的,打开空调.wav      # cmd_id 2: "OK, turning on AC"
  [3]好的,关闭空调.wav      # cmd_id 3: "OK, turning off AC"
  [10]好的,制冷模式.wav     # cmd_id 10: "OK, cooling mode"
  [100]欢迎使用...wav      # cmd_id 100: Welcome message
  [1000]beep.wav           # cmd_id 1000: Beep sound
```

### Supported Formats

| Format | Decoder | Config Macro |
|--------|---------|-------------|
| Prompt | Prompt decoder | `USE_PROMPT_DECODER=1` |
| MP3 | MP3 decoder | `USE_MP3_DECODER=1` |
| ADPCM | ADPCM decoder | (included by default) |
| M4A/AAC | M4A parser + AAC | (included by default) |
| FLAC | FLAC decoder | (included by default) |

---

## Callback States

```c
enum
{
    AUDIO_PLAY_CB_STATE_UNKNOWN_ERR              = -99,
    AUDIO_PLAY_CB_STATE_DECODER_MEM_ERR          = -5,
    AUDIO_PLAY_CB_STATE_PARSE_MP3_MEM_ERR        = -4,
    AUDIO_PLAY_CB_STATE_PARSE_M4A_MEM_ERR        = -3,
    AUDIO_PLAY_CB_STATE_PARSE_FILE_LEASTDATA_ERR = -2,
    AUDIO_PLAY_CB_STATE_INTERNAL_ERR             = -1,
    AUDIO_PLAY_CB_STATE_DONE                     = 0,   // Playback complete
    AUDIO_PLAY_CB_STATE_PAUSE                    = 1,   // Paused
    AUDIO_PLAY_CB_STATE_PAUSE_BEFORE_THRESHOLD   = 2,
    AUDIO_PLAY_CB_STATE_PAUSE_AFTER_THRESHOLD    = 3,
    AUDIO_PLAY_CB_STATE_PLAY_THRESHOLD           = 4,   // Reached play threshold
};
```

---

## Common Patterns

### Play Prompt After ASR Recognition

```c
// In deal_asr_msg_by_cmd_id():
case 2: // "打开空调"
{
    // Perform action
    gpio_set_output_high_level(PA, pin_0);

    // Play response prompt
    // select_index: -1=default prompt, 0="OK", 1="Already at max"
    int select_index = 0;
    prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback, true);
    break;
}
```

### Play Welcome on Boot

```c
// In UserTaskManageProcess() - SYS_MSG_TYPE_AUDIO_IN_STARTED:
// play_exit_wakeup_done_cb() is defined in system_msg_deal.c (project source).
// It sends MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD to switch model.
#if PLAY_WELCOME_EN
vTaskDelay(pdMS_TO_TICKS(300));  // Wait for PA to stabilize
play_exit_wakeup_done_cb(INVALID_HANDLE);  // From system_msg_deal.c
prompt_play_by_cmd_string("<welcome>", -1, NULL, true);
#endif
```

### Disable Prompt Playback

```c
// Disable all prompt playback
prompt_player_enable(DISABLE);

// Re-enable
prompt_player_enable(ENABLE);
```

---

## Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| No sound | PA not enabled | Check `PLAYER_CONTROL_PA` or call `power_amplifier_on()` |
| Distorted audio | Volume too high | Lower `VOLUME_MAX` or `VOLUME_DEFAULT` |
| Crackle at start | PA not stabilized | Add `vTaskDelay(pdMS_TO_TICKS(300))` before first play |
| MP3 not playing | MP3 decoder disabled | Set `USE_MP3_DECODER=1` |
| Prompt not found | Wrong cmd_id mapping | Verify voice file names match command IDs |
| Memory error | Insufficient heap | Check `xPortGetFreeHeapSize()`, increase `SYS_HEAP_SIZE` |
| Audio cuts off | ASR steals CPU | Use `pause_asr(1,1)` before critical playback |
