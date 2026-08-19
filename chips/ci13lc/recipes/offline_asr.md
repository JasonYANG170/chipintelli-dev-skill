# Offline ASR on CI13LC

> Applies to: CI1311, CI1312, CI1316x, CI1324x, and CI1332x; confirm exact chip, board, and voice/connectivity feature set before coding.
> SDK: CI13LC SDK selected by the project.
> Evidence: `chips/ci13lc/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13lc/recipes/offline_asr.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

## Overview

CI13LC provides offline speech recognition with:
- Wakeup word detection
- Command word recognition
- Multi-model switching
- Configurable confidence thresholds

## Configuration (user_config.h)

```c
// Wakeup configuration
#define USE_SEPARATE_WAKEUP_EN      1  // Separate wakeup model
#define DEFAULT_MODEL_GROUP_ID      1  // 0=command model, 1=wakeup model

// Exit wakeup timeout (ms)
#define EXIT_WAKEUP_TIME            15000

// Prompt playback
#define PLAY_WELCOME_EN             1  // Play welcome on boot
#define PLAY_ENTER_WAKEUP_EN        1  // Play prompt on wakeup
#define PLAY_EXIT_WAKEUP_EN         1  // Play prompt on exit wakeup
#define PLAY_OTHER_CMD_EN           1  // Play prompt on command

// Player
#define AUDIO_PLAYER_ENABLE         1
#define VOLUME_MAX                  7
#define VOLUME_MIN                  1
#define VOLUME_DEFAULT              5
```

## Handling ASR Results

In `user_msg_deal.c`:

```c
void user_msg_deal(sys_msg_t *msg)
{
    switch (msg->type) {
    case MSG_ASR_RESULT:
        {
            uint16_t semantic_id = msg->data.asr_id;
            // Map semantic ID to action
            switch (semantic_id) {
            case SEMANTIC_ID_TURN_ON:
                // Handle turn on command
                break;
            case SEMANTIC_ID_TURN_OFF:
                // Handle turn off command
                break;
            // ... more commands
            }
        }
        break;

    case MSG_WAKEUP:
        // System entered wakeup state
        break;

    case MSG_EXIT_WAKEUP:
        // System exited wakeup state (timeout)
        break;
    }
}
```

## ASR Model Files

Models are generated on the voice AI platform (https://aiplatform.chipintelli.com):

- `firmware/asr/` - ASR acoustic model
- `firmware/dnn/` - DNN neural network model

## Tuning Recognition

### Confidence thresholds (in ci_ssp_config.c or sdk_default_config.h):

```c
// Base confidence and valid count
#define DEFAULT_CONFIDENCE          60   // Base confidence (0-100)
#define DEFAULT_CNT                3    // Frames needed to confirm

// Max stop confidence
#define MAX_STOP_CFD_ENABLE        1
#define MAX_STOP_CFD_NOCNT         10
#define MAX_STOP_CFD_CNT           5

// VAD sensitivity
#define VAD_SENSITIVITY            50   // 0-100, higher=more sensitive

// Adaptive threshold
#define ADAPTIVE_THRESHOLD         0    // 0=off, 1=adaptive

// Recover result (fill missed frames)
#define RECOVER_RESULT_ENABLE      1
#define RECOVER_RESULT_MODE        0
#define RECOVER_RESULT_MAX_FRM     3
```

## Model Switching

Switch between language/accent models at runtime:

```c
#include "ci_flash_data_info.h"

// Switch to model group 2
ci_flash_data_info_init(2);

// Or use API to switch
extern int asr_model_switch(uint8_t model_id);
asr_model_switch(2);
```
