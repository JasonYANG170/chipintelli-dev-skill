# Recipe: Create a New CI130X Project

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> Template: `projects/offline_asr_pro_sample`

---

## Overview

New CI130X projects should always start by copying the closest existing sample. This ensures all required files, build configuration, and firmware structure are in place.

---

## Step 1: Copy the Template

```bash
SDK_ROOT="/d/启英泰伦/CI130X_SDK_Offline_V2.1.14/ci130x_sdk"

# Copy the sample project
cp -r "$SDK_ROOT/projects/offline_asr_pro_sample" "$SDK_ROOT/projects/my_project"
```

---

## Step 2: Configure user_config.h

Edit `projects/my_project/src/user_config.h`:

### 2.1 Select Board

```c
// Select your board (only one should be 1)
#define USE_CI_D02GS01J_BOARD       0   // CI1302, 2MB, SSOP24
#define USE_CI_D02GS02S_BOARD       0   // CI1302, 2MB, SSOP24 (SMT)
#define USE_CI_D12GS01J_BOARD       0   // CI1312, 2MB, SSOP16
#define USE_CI_D06GT01D_BOARD       1   // CI1306, 4MB, QFN40 (dev board)
#define USE_CUS_XXXXXXX_BOARD       0   // Custom board
```

### 2.2 Configure Microphone

```c
// 0 = differential (standard), 1 = single-end (cost-saving, MICN_L to GND)
#define MIC_DIFF_SINGLE             0
```

### 2.3 Configure UART

```c
// Log output UART (must differ from protocol UART)
#define CONFIG_CI_LOG_UART          HAL_UART0_BASE

// Voice module protocol UART
#define MSG_COM_USE_UART_EN         1
#define UART_PROTOCOL_NUMBER        (HAL_UART2_BASE)
#define UART_PROTOCOL_BAUDRATE      (UART_BaudRate9600)
#define UART_PROTOCOL_VER           2   // 1=legacy, 2=current, 255=platform
```

### 2.4 Configure Clock Source

```c
// For CI1302/CI1306 with external crystal:
#define USE_EXTERNAL_CRYSTAL_OSC    1

// For CI1312/CI1311 (no external crystal support):
#define USE_EXTERNAL_CRYSTAL_OSC    0
```

### 2.5 Configure ASR

```c
// Use separate wakeup word model
#define USE_SEPARATE_WAKEUP_EN      1
#define DEFAULT_MODEL_GROUP_ID      1   // 1=wakeup model at boot

// Wakeup timeout (ms) - return to wakeup-only mode after this
#define EXIT_WAKEUP_TIME            15*1000

// Prompt playback
#define PLAY_WELCOME_EN             1   // Play welcome on boot
#define PLAY_ENTER_WAKEUP_EN        1   // Play on wakeup
#define PLAY_EXIT_WAKEUP_EN         1   // Play on exit wakeup
#define PLAY_OTHER_CMD_EN           1   // Play on command recognition
```

### 2.6 Configure Player

```c
#define AUDIO_PLAYER_ENABLE         1
#define PLAYER_CONTROL_PA           0   // 0=PA always on, 1=PA controlled by player
#define VOLUME_MAX                  7
#define VOLUME_MIN                  1
#define VOLUME_DEFAULT              5

#if AUDIO_PLAYER_ENABLE
#define USE_PROMPT_DECODER          1   // Prompt format support
#define USE_MP3_DECODER             1   // MP3 support
#define AUDIO_PLAY_SUPPT_MP3_PROMPT 1   // MP3 prompt files
#endif
```

### 2.7 Configure Algorithms (Optional)

```c
#define USE_ALC_AUTO_SWITCH_MODULE  0   // Dynamic ALC
#define USE_DENOISE_MODULE          0   // Noise reduction
#define USE_AEC_MODULE              0   // Echo cancellation

#if USE_AEC_MODULE
#define IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY   1
#define HOST_CODEC_CHA_NUM          2
#endif
```

---

## Step 3: Add Custom Code in user_msg_deal.c

Edit `projects/my_project/src/user_msg_deal.c`:

### 3.1 Handle ASR Results by Command ID

```c
uint32_t deal_asr_msg_by_cmd_id(sys_msg_asr_data_t *asr_msg, cmd_handle_t cmd_handle, uint16_t cmd_id)
{
    uint32_t ret = 1;
    int select_index = -1;
    switch(cmd_id)
    {
        ///tag-asr-msg-deal-by-cmd-id-start
        case 2: // "打开空调" (Turn on AC)
        {
            // Add your control code here
            // e.g., control GPIO, send UART command, etc.
            break;
        }
        case 3: // "关闭空调" (Turn off AC)
        {
            break;
        }
        case 14: // "除湿模式" (Dehumidify mode)
        {
            break;
        }
        ///tag-asr-msg-deal-by-cmd-id-end
        default:
            ret = 0;
            break;
    }

    if (ret && select_index >= -1)
    {
        #if PLAY_OTHER_CMD_EN
        prompt_play_by_cmd_handle(cmd_handle, select_index, default_play_done_callback, true);
        #endif
    }
    return ret;
}
```

### 3.2 Handle ASR Results by Semantic ID

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

### 3.3 Add GPIO Init in userapp_initial()

```c
void userapp_initial(void)
{
    #if MSG_COM_USE_UART_EN
    #if (UART_PROTOCOL_VER == 2)
    vmup_communicate_init();
    #endif
    #endif

    ///tag-gpio-init
    // Add GPIO initialization here
    scu_set_device_gate(HAL_GPIOA_BASE, ENABLE);
    scu_set_device_reset(HAL_GPIOA_BASE);
    scu_set_device_reset_release(HAL_GPIOA_BASE);

    gpio_set_output_mode(PA, pin_0);
    gpio_set_output_low_level(PA, pin_0);
}
```

---

## Step 4: Update source_file.prj

If you add new source files, update `projects/my_project/project_file/source_file.prj`:

```
// Add your new source files
source-file: projects/my_project/src/my_driver.c
source-file: projects/my_project/src/my_protocol.c

// Add new include paths if needed
include-path: projects/my_project/src
```

---

## Step 5: Replace ASR Model and Voice Files

### 5.1 Generate ASR Models

1. Go to https://aiplatform.chipintelli.com
2. Create a project for your chip type (e.g., CI1306)
3. Define wakeup word and command words
4. Generate and download model files

### 5.2 Replace Model Files

```bash
# Replace ASR model files
cp downloaded_asr_cmd.dat "projects/my_project/firmware/asr/[0]asr_chinese_CI1306_Vxxxxx_cmd.dat"
cp downloaded_asr_wake.dat "projects/my_project/firmware/asr/[1]asr_chinese_CI1306_Vxxxxx_wake.dat"

# Replace DNN model files
cp downloaded_dnn.fefixbin "projects/my_project/firmware/dnn/[0]G3-NLP-CH-S-Vxxxxx.fefixbin458"
```

### 5.3 Replace Voice Prompts

```bash
# Replace voice prompt WAV files (named by command ID)
# Format: [cmd_id]prompt_text.wav
cp my_prompt.wav "projects/my_project/firmware/voice/src/[2]好的,打开空调.wav"
```

### 5.4 Update Command Info

Edit `projects/my_project/firmware/user_file/cmd_info/[60000]{智能管家}V2.xlsx` to match your command words and semantic IDs.

---

## Step 6: Build and Flash

### 6.1 Build

```bash
cd projects/my_project/project_file
make clean
make -j4
```

### 6.2 Flash

Use `ci-tool-kit` or the VS Code ci-tool extension to flash the firmware:

```bash
# Using ci-tool-kit
$SDK_ROOT/tools/ci-tool-kit program -p COM3 -b 115200
```

Or use `code_program.exe` from the SDK tools.

---

## File Checklist

Verify these files exist and are correctly configured:

- [ ] `src/user_config.h` - Board, chip, UART, ASR, player config
- [ ] `src/user_msg_deal.c` - Your custom ASR result handling
- [ ] `src/ci_ssp_config.c` - Voice signal processing config
- [ ] `src/ci130x.lds` - Linker script (usually unchanged)
- [ ] `project_file/Makefile` - Build file (usually unchanged)
- [ ] `project_file/source_file.prj` - Source file list
- [ ] `firmware/asr/` - ASR model files (correct chip type)
- [ ] `firmware/dnn/` - DNN model files
- [ ] `firmware/voice/src/` - Voice prompt files
- [ ] `firmware/user_file/cmd_info/` - Command word definition
