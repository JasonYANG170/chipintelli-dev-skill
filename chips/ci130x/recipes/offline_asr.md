# Recipe: Offline ASR Development

> Applies to: CI1302 and CI1306 offline ASR projects.
> Excludes: CI1312 unless the selected unified CI13XX SDK explicitly documents support for this flow.
> Evidence: `chips/ci130x/resources/api_reference.md`, `chips/ci130x/resources/build_system.md`, and CI130X SDK `projects/` ASR examples.
> Validation: source-matched.

---

## Overview

CI130X offline ASR performs voice recognition entirely on-chip without network connectivity. The recognition pipeline runs on the nuclear (DSP) core while user code runs on the host core.

### Recognition Flow

```
Microphone ──► Codec ADC ──► IISDMA ──► STFT ──► [AEC] ──► [Denoise] ──► [BF] ──► VAD ──► ASR Engine ──► Result
                                                                                                       │
                                                                                                       ▼
                                                                                            sys_msg_queue
                                                                                                       │
                                                                                                       ▼
                                                                                            UserTaskManageProcess()
                                                                                                       │
                                                                                                       ▼
                                                                                            deal_asr_msg_by_cmd_id()
```

---

## ASR Configuration

### user_config.h Settings

```c
// Use separate wakeup word model (recommended)
#define USE_SEPARATE_WAKEUP_EN      1

// Default model group: 0=command model, 1=wakeup model
#define DEFAULT_MODEL_GROUP_ID      1   // Start in wakeup-only mode

// Exit wakeup timeout (ms): return to wakeup-only after this idle period
#define EXIT_WAKEUP_TIME            15*1000  // 15 seconds

// Prompt playback controls
#define PLAY_WELCOME_EN             1   // Boot welcome
#define PLAY_ENTER_WAKEUP_EN        1   // Wakeup confirmation
#define PLAY_EXIT_WAKEUP_EN         1   // Exit wakeup
#define PLAY_OTHER_CMD_EN           1   // Command acknowledgment

// Confidence and sensitivity
#define ADAPTIVE_THRESHOLD          0   // 1=adaptive confidence, 0=fixed
#define ASR_SKIP_FRAME_CONFIG       0   // Frame skipping for multi-model
```

### Model Files

| File Location | Description |
|---------------|-------------|
| `firmware/asr/[0]asr_*_cmd.dat` | Command word ASR model |
| `firmware/asr/[1]asr_*_wake.dat` | Wakeup word ASR model |
| `firmware/dnn/[0]*.fefixbin*` | DNN neural network model |
| `firmware/user_file/cmd_info/[60000]*.xlsx` | Command word definitions |

Generate models at: https://aiplatform.chipintelli.com

---

## Handling Recognition Results

### ASR Message Flow

ASR results arrive as system messages in `UserTaskManageProcess()`:

```c
// In system_msg_deal.c - UserTaskManageProcess()
case SYS_MSG_TYPE_ASR:
{
    sys_msg_asr_data_t *asr_rev_data = &(rev_msg.msg_data.asr_data);
    sys_deal_asr_msg(asr_rev_data);
    break;
}
```

`sys_deal_asr_msg()` processes the result:
1. Checks if result status is `MSG_ASR_STATUS_GOOD_RESULT`
2. Checks if it's a wakeup word → calls `enter_wakeup_deal()`
3. If in wakeup state and it's a command word → calls user handlers

### Handling by Command ID

In `user_msg_deal.c`:

```c
uint32_t deal_asr_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg, cmd_handle_t cmd_handle, uint16_t cmd_id)
{
    uint32_t ret = 1;
    int select_index = -1;

    switch(cmd_id)
    {
        case 2:  // "打开空调" (Turn on AC)
            // Your action code here
            gpio_set_output_high_level(PA, pin_0);  // Example: turn on GPIO
            break;

        case 3:  // "关闭空调" (Turn off AC)
            gpio_set_output_low_level(PA, pin_0);
            break;

        case 10: // "制冷模式" (Cooling mode)
            select_index = 0;  // Play first variant of prompt
            break;

        default:
            ret = 0;  // Not handled by user
            break;
    }

    // Play prompt if handled
    if (ret && select_index >= -1)
    {
        #if PLAY_OTHER_CMD_EN
        prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback, true);
        #endif
    }
    return ret;
}
```

### Handling by Semantic ID

Semantic IDs are defined in the command info Excel file. They encode product ID + function ID:

```c
uint32_t deal_asr_msg_by_semantic_id(sys_msg_asr_data_t *asr_msg, cmd_handle_t cmd_handle, uint32_t semantic_id)
{
    uint32_t ret = 1;

    if (PRODUCT_GENERAL == get_product_id_from_semantic_id(semantic_id))
    {
        switch(get_function_id_from_semantic_id(semantic_id))
        {
        case VOLUME_UP:
            vol_set(vol_get() + 1);
            break;
        case VOLUME_DOWN:
            vol_set(vol_get() - 1);
            break;
        case MAXIMUM_VOLUME:
            vol_set(VOLUME_MAX);
            break;
        case MEDIUM_VOLUME:
            vol_set(VOLUME_MID);
            break;
        case MINIMUM_VOLUME:
            vol_set(VOLUME_MIN);
            break;
        case TURN_ON_VOICE_BROADCAST:
            prompt_player_enable(ENABLE);
            break;
        case TURN_OFF_VOICE_BROADCAST:
            prompt_player_enable(DISABLE);
            break;
        default:
            ret = 0;
            break;
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}
```

### Priority: cmd_id first, then semantic_id

The system calls `deal_asr_msg_by_cmd_id()` first. If it returns 0 (not handled), `deal_asr_msg_by_semantic_id()` is called:

```c
// In system_msg_deal.c - sys_deal_asr_msg()
if (0 == deal_asr_msg_by_cmd_id(asr_msg, cmd_handle, cmd_id))
{
    if (0 == deal_asr_msg_by_semantic_id(asr_msg, cmd_handle, semantic_id))
    {
        // Default: play prompt if auto-play is enabled
        #if PLAY_OTHER_CMD_EN
        prompt_play_by_cmd_handle(cmd_handle, -1, default_play_done_callback, true);
        #endif
    }
}
```

---

## Wakeup and Command Word Configuration

### Wakeup Word Model

With `USE_SEPARATE_WAKEUP_EN=1`, the system uses two separate ASR models:
- **Wakeup model** (group 1): Only recognizes the wakeup word
- **Command model** (group 0): Recognizes all command words

### State Machine

```
Power On
    │
    ▼
[Wakeup-only Mode] ◄──────────────────────────┐
    │                                           │
    │ User says wakeup word                     │
    ▼                                           │
[Wakeup State] ──► Play wakeup prompt           │
    │                                           │
    │ User says command word                    │
    ▼                                           │
[Command Processing] ──► Play response          │
    │                                           │
    │ No command for EXIT_WAKEUP_TIME (15s)     │
    ▼                                           │
[Exit Wakeup] ──► Play exit prompt ─────────────┘
                  Switch to wakeup model
```

### Model Switching

Model switching is handled by `system_msg_deal.c` and is serialized through the system message queue:

```c
// Enter wakeup state (called when wakeup word recognized)
void enter_wakeup_deal(uint32_t exit_wakup_ms, cmd_handle_t cmd_handle)
{
    // Switch from wakeup model to command model
    // Play wakeup prompt
    // Start exit wakeup timer
    set_state_enter_wakeup(exit_wakup_ms);
}

// Exit wakeup state (called on timeout)
void exit_wakeup_deal(uint32_t asr_busy_check)
{
    // Play exit prompt
    // Switch back to wakeup model
    // Enter low-power mode
}
```

### Custom Wakeup Timeout

```c
// Change wakeup timeout duration (ms)
#define EXIT_WAKEUP_TIME  30*1000  // 30 seconds instead of 15

// Or dynamically update wakeup time
void update_awake_time(void)
{
    if (sys_manage_data.wakeup_state == SYS_STATE_WAKEUP)
    {
        set_state_enter_wakeup(EXIT_WAKEUP_TIME);
    }
}
```

---

## Confidence Tuning

### Fixed Confidence

In `main.c` `task_init()`:

```c
// Configure base confidence and valid count
// DEFAULT_CONFIDENCE: base confidence threshold (lower = more sensitive)
// DEFAULT_CNT: number of consecutive frames needed to confirm
extern int config_base_confidence_count(short base_confidence, unsigned char valid_count);
config_base_confidence_count(DEFAULT_CONFIDENCE, DEFAULT_CNT);
```

### Adaptive Confidence

Enable in `user_config.h`:

```c
#define ADAPTIVE_THRESHOLD  1   // Enable adaptive threshold
```

When enabled, the system dynamically adjusts confidence based on noise level:

```c
// In system_msg_deal.c - change_asr_normal_word()
#if ADAPTIVE_THRESHOLD
dynmic_confidence_en_cfg(1);
dynmic_confidence_config(-20, 10, 1);  // min_offset, max_offset, step
#endif
```

### Other ASR Tuning Parameters

```c
// In main.c task_init():

// Adaptive count (enable/disable adaptive counting)
extern void config_adpt_cnt(int enable);
config_adpt_cnt(ADAPTIVE_CNT_ENABLE);

// Max stop confidence (stop recognition after N consecutive matches)
extern void config_max_stop_cfd(int enable, int nocnt_max_stop_cfd, int cnt_max_stop_cfd);
config_max_stop_cfd(MAX_STOP_CFD_ENABLE, MAX_STOP_CFD_NOCNT, MAX_STOP_CFD_CNT);

// Max VAD end frames (maximum frames before VAD end)
extern void config_max_vad_end_frm(int max_vad_end_frm);
config_max_vad_end_frm(MAX_STOP_VAD_FRM);

// Result recovery (recover from partial recognition)
extern void config_recover_result(int enable, int mode, int max_frm);
config_recover_result(RECOVER_RESULT_ENABLE, RECOVER_RESULT_MODE, RECOVER_RESULT_MAX_FRM);

// Silence probability (stop on silence)
extern void config_silprob_cnt(float base_silprob, int base_silcnt);
config_silprob_cnt(DEFAULT_STOP_SILPROB, DEFAULT_STOP_SILCNT);

// VAD sensitivity
REMOTE_CALL(set_freqvad_start_para_gain(VAD_SENSITIVITY));

// Decoder beam (search width)
float beam = DECODER_BEAM;
ciss_set(CI_SS_DECODER_BEAM, *(uint32_t*)&beam);
```

### ASR Score Access

The ASR score is available in the message:

```c
// In sys_deal_asr_msg():
ciss_set(CI_SS_CMD_SCORE, asr_msg->asr_score);

// Access score in user code:
// asr_msg->asr_score contains the recognition confidence score
```

---

## Voice Signal Processing (ci_ssp_config.c)

Configure the audio processing pipeline in `ci_ssp_config.c`:

### STFT Configuration

```c
const stft_istft_config_t stft_istft_config = {
    .sample_frequency = 16000,        // 16kHz sampling
    .frame_size = 512,                 // Window size
    .frame_shift = AUDIO_CAP_POINT_NUM_PER_FRM,  // Frame shift (160 for 16kHz)
    .fft_frm_size = 512,              // FFT input size
    .fft_size = 257,                  // FFT positive frequency bins
    .result_out_channel = 1,          // Output channels
    .time_pre_emphasis_enable = false,
    .downsampled_enable = false,
    .fe_psd_enable = false
};
```

### Audio Capture Configuration

```c
audio_capture_t audio_capture = {
    .frame_length = AUDIO_CAP_POINT_NUM_PER_FRM,  // 160 for 16kHz
    .mic_channel_num = 1,    // 1 for single mic, 2 for dual mic
    .ref_channel_num = 0,    // 1 for AEC, 0 otherwise
};
```

---

## Debugging ASR

### Log Output

```c
// ASR result logging (in sys_deal_asr_msg):
ci_loginfo(LOG_USER, "asr cmd_id:%d,semantic_id:%08x\n", cmd_id, semantic_id);

// System status monitoring (in main loop):
mprintf("asr heap min free:%dKB\n", get_heap_bytes_remaining_size()/1024);
mprintf("system heap min free:%dKB\n", xPortGetMinimumEverFreeHeapSize()/1024);
```

### IIS Audio Output for Debugging

Enable pre-result audio output to capture processed audio:

```c
// In user_config.h:
#define USE_IIS1_OUT_PRE_RSLT_AUDIO  1  // Enable IIS audio output (uses PA2-PA6)

// In ci_ssp_config.c:
const iis_out_audio_config_t iis_out_audio_config = {
    .iis_out_enable = USE_IIS1_OUT_PRE_RSLT_AUDIO,
    .iis_left_channel = MICL,    // Raw mic left
    .iis_right_channel = DST1,   // Processed audio
    .vad_mark_enable = true,
    .ssp_dst_cover_micl_enble = false
};
```

### Task Monitoring

```c
// Task status printout (in main loop):
mprintf("TaskName\t\tPriority\tTaskNumber\tMinStk\t%d\n", ArraySize2);
for (int i = 0; i < ArraySize2; i++)
{
    mprintf("%-16s\t%d\t\t%d\t\t%d\r\n",
        StatusArray[i].pcTaskName,
        (int)StatusArray[i].uxCurrentPriority,
        (int)StatusArray[i].xTaskNumber,
        (int)StatusArray[i].usStackHighWaterMark);
}
```

---

## Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| No recognition at all | Wrong chip type or model files | Verify `CI_CHIP_TYPE` matches model files |
| Recognition too insensitive | Confidence threshold too high | Lower `DEFAULT_CONFIDENCE` or enable `ADAPTIVE_THRESHOLD` |
| False triggers | Confidence too low or noisy environment | Increase `DEFAULT_CONFIDENCE` or enable `USE_DENOISE_MODULE` |
| Recognition during playback | No AEC | Enable `USE_AEC_MODULE=1` or pause ASR during playback |
| Wakeup timeout too short | `EXIT_WAKEUP_TIME` too low | Increase to `30*1000` for 30 seconds |
| ASR doesn't start | Dual-core sync missing | Verify `mailboxboot_sync()` is called |
| Model switch fails | Race condition | Use `send_msg_to_sys_task()` for model switches |
