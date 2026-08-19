# Recipe: Offline ASR on CI13XX Unified SDK

> Chips: CI1306, CI1311, CI1312, CI1316, CI1324, CI1332, CI2312

> Evidence: `chips/ci13xx/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13xx/recipes/offline_asr.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

---

## Overview

CI13XX unified SDK provides offline ASR with an advanced algorithm pipeline. The recognition flow runs on the DSP (nuclear) core while user code runs on the host core. CI13XX adds support for AI denoise, AI DOA, PWK (proximity wake), CWSL v2, BLE, TTS, and sound event detection compared to CI130X.

### Recognition Flow

```
Microphone -> Codec ADC -> IISDMA -> STFT -> [AEC] -> [Denoise] -> [BF] -> VAD -> ASR Engine -> Result
                                                                                                    |
                                                                                                    v
                                                                                          sys_msg_queue
                                                                                                    |
                                                                                                    v
                                                                                          UserTaskManageProcess()
                                                                                                    |
                                                                                                    v
                                                                                          deal_asr_msg_by_cmd_id()
```

---

## SDK

`CI13XX_SDK_ASR_ALG_V2.7.12` - project: `offline_asr_alg_pro_sample`

---

## Source Anchors

- `CI13XX_SDK_ASR_ALG_V2.7.12/components/asr/asr_api.h` - ASR engine API
- `CI13XX_SDK_ASR_ALG_V2.7.12/components/flash_control/flash_control_inc/ci_flash_data_info.h` - model address management
- `CI13XX_SDK_ASR_ALG_V2.7.12/projects/offline_asr_alg_pro_sample/app/app_main/system_msg_deal.h` - message types and ASR result handling
- `CI13XX_SDK_ASR_ALG_V2.7.12/projects/offline_asr_alg_pro_sample/app/app_main/user_config.h` - ASR configuration
- `CI13XX_SDK_ASR_ALG_V2.7.12/projects/offline_asr_alg_pro_sample/app/app_main/main.c` - initialization pattern

---

## API Usage

## ASR Engine API (asr_api.h)

```c
// Configure audio buffer for ASR
int asrtop_asrpcmbuf_mem_cfg(unsigned int buf_base_ptr, int frm_nums, int frm_shift);

// Create ASR system tasks, semaphores, queues; configure defaults
int asrtop_taskmanage_create(void);

// Start ASR system with model addresses
// lg_model_addr0: language model 0 address (flash/SRAM/PSRAM)
// lg_model_size0: language model 0 size
// ac_model_addr: acoustic model address (flash/PSRAM)
// ac_model_size: acoustic model size
// pdata[0]: DNN output width (1=8bit, other=16bit)
// pdata[1]: 0=single model, 1=dual model (pdata[2-4] valid)
// pdata[2]: lg_model_addr1, pdata[3]: lg_model_size1, pdata[4]: model index
int asrtop_asr_system_start(unsigned int lg_model_addr0, unsigned int lg_model_size0,
                             unsigned int ac_model_addr, unsigned int ac_model_size, void* pdata);

// Release ASR system (frees all caches; must call start to resume)
int asrtop_asr_system_release(void);

// Create model independently (does not affect flash access)
int asrtop_asr_system_create_model(unsigned int lg_model_addr, unsigned int lg_model_size,
                                    unsigned int ac_model_addr, unsigned int ac_model_size, void* pdata);

// Lite version of model create
int asrtop_asr_system_litecreate(unsigned int lg_model_addr, unsigned int lg_model_size,
                                  unsigned int ac_model_addr, unsigned int ac_model_size, void* pdata);

// Pause/resume ASR
int asrtop_asr_system_pause(void);
int asrtop_asr_system_continue(void);

// Switch language model network index
int asrtop_asr_switch_fst(int fst_idx, void* pdata);

// Check if ASR is busy
int asrtop_sys_isbusy(void);

// Get decoded frame count
short asrtop_get_decode_pcm_finished_frame(void);

// Get ASR frame shift
int get_asrtop_asrfrmshift(void);

// Flash management (request/release flash access)
int send_requset_flash_msg_to_dnn(void);
int send_release_flash_semaphore_to_dnn(void);

// CMVN update weight (debug)
int asrtop_cmvn_update_weight_config(float alpha);

// VAD configuration
int asrtop_tdvad_base_energy_cfg(float base_energy);
int asrtop_tdvad_vadend_frames_cfg(int vadend_frames);

// Confidence configuration
int asrtop_dynamic_confidence_mode_cfg(int confidence_mode);  // 0=average, -1=max
int set_asr_sigle_word_confidence_count_threshold(short confidence_thr, unsigned char valid_count_thr);
int dynmic_confidence_config(int min, int max, int step);
int dynmic_confidence_en_cfg(int en_cfg);

// Dynamic skip (multi-model)
int asr_dynamic_skip_open(void);
int asr_dynamic_skip_close(void);

// ASR version query (needs >= 80 byte buffer)
int get_asr_sys_verinfo(char* version_buf);

// ASR startup task
void asr_system_startup_task(void* p);
```

## Flash Data Info (ci_flash_data_info.h)

```c
// Initialize flash data info with default model group
// 0=command model, 1=wakeup model
uint32_t ci_flash_data_info_init(uint8_t default_model_group_id);

// Get current model addresses
uint32_t get_current_model_addr(uint32_t *p_dnn_addr, uint32_t *p_dnn_size,
                                 uint32_t *p_asr_addr, uint32_t *p_asr_size);

// OTA info structure
typedef struct {
    unsigned int cias_ota_chip_type;    // Chip type
    unsigned char cias_ota_uart_port;    // OTA UART port (0:uart0, 1:uart1, 2:uart2)
    UART_BaudRate cias_ota_baud;        // OTA baud rate
} cias_ota_flag_t;
```

## System Message Types (system_msg_deal.h)

```c
// ASR status
typedef enum {
    MSG_ASR_STATUS_GOOD_RESULT = 0x00,
    MSG_ASR_STATUS_NO_RESULT,
    MSG_ASR_STATUS_VAD_START,
    MSG_ASR_STATUS_AUDIO_PROCESS,
    MSG_ASR_STATUS_VAD_END,
    MSG_CWSL_STATUS_GOOD_RESULT,
} sys_msg_asr_status_t;

// ASR result data
typedef struct {
    sys_msg_asr_status_t asr_status;
    uint32_t asr_cmd_handle;
    uint32_t asr_pcm_base_addr;
    short asr_score;
    uint16_t asr_frames;
} sys_msg_asr_data_t;

// CMD info status (model switching)
typedef enum {
    MSG_CMD_INFO_STATUS_EXIT_WAKEUP = 0x00,
    MSG_CMD_INFO_STATUS_ENABLE_EXIT_WAKEUP,
    MSG_CMD_INFO_STATUS_ENABLE_PROCESS_ASR,
    MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD,
    MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD,
} sys_msg_cmd_info_status_t;

// System message types
typedef enum {
    SYS_MSG_TYPE_ASR = 0,
    SYS_MSG_TYPE_CMD_INFO,
    SYS_MSG_TYPE_KEY,
    SYS_MSG_TYPE_COM,
    SYS_MSG_TYPE_PLAY,
    SYS_MSG_TYPE_NET,
    SYS_MSG_TYPE_MNG,
    SYS_MSG_TYPE_AUDIO_IN_STARTED,
    SYS_MSG_TYPE_I2C,
    SYS_MSG_TYPE_SED,
    SYS_MSG_TYPE_TEXT,
    SYS_MSG_TYPE_SET_PALY_PARAMETER,
} sys_msg_type_t;

// Wakeup state
typedef enum {
    SYS_STATE_UNWAKEUP = 0,
    SYS_STATE_WAKEUP,
} sys_wakeup_state_t;

// Key functions
void userapp_initial(void);
void sys_msg_task_initial(void);
void UserTaskManageProcess(void *p_arg);
void enter_wakeup_deal(uint32_t exit_wakup_ms, cmd_handle_t cmd_handle);
void exit_wakeup_deal(uint32_t asr_busy_check);
sys_wakeup_state_t get_wakeup_state(void);
void update_awake_time(void);
BaseType_t send_msg_to_sys_task(sys_msg_t *send_msg, BaseType_t *xHigherPriorityTaskWoken);
void default_play_done_callback(cmd_handle_t cmd_handle);
uint8_t vol_set(char vol);
uint8_t vol_get(void);
void pause_asr(uint8_t voice_in_mute, uint8_t pause_asr_task);
void resume_asr(void);
```

---

## Usage Example

### user_config.h ASR Settings

```c
// Use separate wakeup word model
#define USE_SEPARATE_WAKEUP_EN   1

// Default model group: 0=command, 1=wakeup
#define DEFAULT_MODEL_GROUP_ID   1

// Exit wakeup timeout (ms)
#define EXIT_WAKEUP_TIME         15*1000

// Prompt playback
#define PLAY_WELCOME_EN          1
#define PLAY_ENTER_WAKEUP_EN     1
#define PLAY_EXIT_WAKEUP_EN      1
#define PLAY_OTHER_CMD_EN        1

// Confidence
#define ADAPTIVE_THRESHOLD       0
```

### Initialization Pattern (from main.c task_init)

```c
#include "asr_api.h"
#include "ci_flash_data_info.h"
#include "system_msg_deal.h"
#include "codec_manager.h"
#include "flash_control_inner_port.h"

static void task_init(void *p_arg)
{
    // 1. Dual-core shared memory init (dsu_init based on algorithm modules)
    dsu_init(...);

    vTaskDelay(pdMS_TO_TICKS(5));  // Required 5ms delay

    // 2. Initialize codec manager and register audio codec
    cm_init();
    audio_in_codec_registe();

    // 3. Initialize inter-core communication
    nuclear_com_init();
    decoder_port_inner_rpmsg_init();
    flash_control_inner_port_init();
    dnn_nuclear_com_outside_port_init();
    asr_top_nuclear_com_outside_port_init();
    vad_fe_nuclear_com_outside_port_init();
    flash_manage_nuclear_com_outside_port_init();
    codec_manage_inner_port_init();

    // 4. Initialize shared info system
    ciss_init();
    ciss_set(CI_SS_AUDIO_IN_BUFFER_NUM, AUDIO_IN_BUFFER_NUM);
    ciss_set(CI_SS_DECODER_MIN_ACTIVE, DECODER_MIN_ACTIVE);
    ciss_set(CI_SS_INTENT_NUM, MULT_INTENT);
    ciss_set(CI_SS_HOST_PARAM_SET_OK, 0x5A);

    // 5. Dual-core sync
    mailboxboot_sync();

    // 6. Register voice signal processing
    extern ci_ssp_config_t ci_ssp;
    extern audio_capture_t audio_capture;
    REMOTE_CALL(set_ssp_registe(&audio_capture, (ci_ssp_st*)&ci_ssp,
                                 sizeof(ci_ssp)/sizeof(ci_ssp_st)));
    REMOTE_CALL(set_freqvad_start_para_gain(VAD_SENSITIVITY));

    // 7. Initialize flash data info (model addresses)
    ci_flash_data_info_init(DEFAULT_MODEL_GROUP_ID);

    // 8. Initialize audio player
    #if AUDIO_PLAYER_ENABLE
    #if SIMPLE_AUDIO_PLAYER_ENABLE
    sap_init();
    #else
    audio_play_init();
    #endif
    #endif

    // 9. Initialize algorithm models
    alg_model_init();

    // 10. Initialize user app and system messages
    userapp_initial();
    sys_msg_task_initial();
    xTaskCreate(UserTaskManageProcess, "UserTaskManageProcess", 480, NULL, 4, NULL);

    // 11. Configure ASR parameters
    extern void config_adpt_cnt(int enable);
    config_adpt_cnt(ADAPTIVE_CNT_ENABLE);

    extern void config_max_stop_cfd(int enable, int nocnt, int cnt);
    config_max_stop_cfd(MAX_STOP_CFD_ENABLE, MAX_STOP_CFD_NOCNT, MAX_STOP_CFD_CNT);

    extern void config_max_vad_end_frm(int max_vad_end_frm);
    config_max_vad_end_frm(MAX_STOP_VAD_FRM);

    extern int config_base_confidence_count(short, unsigned char);
    config_base_confidence_count(DEFAULT_CONFIDENCE, DEFAULT_CNT);

    extern void config_recover_result(int, int, int);
    config_recover_result(RECOVER_RESULT_ENABLE, RECOVER_RESULT_MODE, RECOVER_RESULT_MAX_FRM);

    extern void config_silprob_cnt(float, int);
    config_silprob_cnt(DEFAULT_STOP_SILPROB, DEFAULT_STOP_SILCNT);
}
```

### Handling ASR Results

```c
// In UserTaskManageProcess (system_msg_deal.c pattern):
case SYS_MSG_TYPE_ASR:
{
    sys_msg_asr_data_t *asr_rev_data = &(rev_msg.msg_data.asr_data);
    // sys_deal_asr_msg processes: check status, wakeup/command handling
    // Then calls user handlers:
    //   deal_asr_msg_by_cmd_id(asr_msg, cmd_handle, cmd_id)
    //   deal_asr_msg_by_semantic_id(asr_msg, cmd_handle, semantic_id)
    break;
}
```

### Model Switching via System Messages

```c
// Switch to command model (after wakeup)
void switch_to_command_model(void)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_NORMAL_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
}

// Switch to wakeup model (on exit wakeup timeout)
void switch_to_wakeup_model(void)
{
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_POST_CHANGE_ASR_WAKEUP_WORD;
    send_msg_to_sys_task(&send_msg, NULL);
}
```

### Pause/Resume ASR During Playback

```c
// Pause ASR before critical audio playback (no AEC scenario)
pause_asr(1, 1);  // mute_voice_in=1, pause_asr_task=1

// ... play audio ...

// Resume ASR after playback
resume_asr();
```

---

## Notes/Tips

- CI13XX uses `ciss_set()` (CI Shared Info System) for inter-core parameter sharing, replacing some of the direct function calls used in CI130X.
- `dsu_init()` address depends on which algorithm modules are enabled (AEC, BF, DOA, CWSL, etc.). The correct extern symbol is selected via `#if` chains in `task_init()`.
- `mailboxboot_sync()` must be called before ASR initialization to ensure dual-core synchronization.
- The CI13XX SDK supports `SIMPLE_AUDIO_PLAYER_ENABLE` as a lightweight alternative to the full audio player.
- Algorithm modules (AI denoise, AI DOA, SED, VPR, CWSL, TTS, PWK) are enabled via `user_config.h` macros and initialized in `alg_model_init()`.
- `DEFAULT_MODEL_GROUP_ID=1` starts in wakeup-only mode; the system switches to command model (group 0) after wakeup.
- CI13XX adds `cias_ota_flag_t` for OTA configuration, storing chip type and UART port info.
- Additional model IDs in CI13XX: `VOICE_VPR_DNN_ID` (60001), `CI_AI_CRY_MODEL_ID` (60002), `CI_AI_DENOISE_MODEL_ID` (60003), `CI_AI_DOA_MODEL_ID` (60004), `CI_AI_SNORE_MODEL_ID` (60005), `CI_AI_RTC_MODEL_ID` (60006).
