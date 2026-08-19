# Command Word Self-Learning (CWSL)

## Overview

CWSL allows users to train custom command words on-device without cloud connectivity. The user speaks the command word multiple times, and the device learns to recognize it.

## SDK

Use `CI13LC_SDK_V2.0.15` with `cwsl_sample` project.

## Source Anchors

- `components/ci_cwsl/cwsl_manage.h` -- CWSL management API
- `components/ci_cwsl/cwsl_template_manager.h` -- Template storage
- `projects/cwsl_sample/src/cwsl_app_sample1.c` -- Sample application logic
- `projects/cwsl_sample/src/cwsl_app_sample1.h` -- App-layer callback declarations
- `components/asr/asr_process_callback.c` -- ASR integration (calls `cwsl_init()`)

## How CWSL Works

1. User enters CWSL mode (via voice command or button)
2. Device prompts user to speak the command word
3. User speaks the word 1-3 times (configurable via `cwsl_init_parameter_t.sg_reg_times`)
4. Device creates a voiceprint template from the recordings
5. The new command word is added to the recognition list

## Configuration

CWSL is enabled via `USE_CWSL` macro. Initialization happens in `asr_process_callback.c`:

```c
#if USE_CWSL
    cwsl_set_vad_alc_config(1);
    cwsl_init();
#endif
```

## CWSL States (from `cwsl_manage.h`)

```c
typedef enum {
    CWSL_STA_IDLE,          // Idle
    CWSL_STA_RECOGNIZATION, // Recognition mode
    CWSL_STA_REG_TEMPLATE,  // Learning/registration mode
    CWSL_STA_DEL_TEMPLATE,  // Delete mode
} cwsl_manage_status_t;
```

## CWSL Registration Result (from `cwsl_manage.h`)

```c
typedef enum {
    CWSL_RECORD_SUCCESSED,              // Recording succeeded
    CWSL_RECORD_FAILED,                 // Recording failed
    CWSL_REG_FINISHED,                  // Registration complete
    CWSL_REG_ABORT,                     // Registration aborted
    CWSL_NOT_ENOUGH_FRAME,              // Not enough audio frames
    CWSL_REG_INVALID_DATA,              // Invalid data
    CWSL_RECORD_FAILED_BY_DEFAULTCMD,   // Conflicts with default command
} cwsl_reg_result_t;
```

## API Usage (from `cwsl_manage.h`)

### Initialization

```c
#include "cwsl_manage.h"

// Called automatically in asr_process_callback.c when USE_CWSL=1
void cwsl_set_vad_alc_config(int cmd);
void cwsl_init();
```

### Registration (Learning)

```c
// Start registering a new command word
// cmd_id: command ID from cmd_info
// group_id: model group ID
// word_type: CMD_WORD, WAKEUP_WORD, or ALL_WORD
int ret = cwsl_reg_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type);

// Start recording a template
int cwsl_reg_record_start();

// Stop recording
int cwsl_reg_record_stop();

// Exit registration mode
int cwsl_exit_reg_word();

// Restart registration
int cwsl_reg_restart();
```

### Status and Management

```c
// Get current CWSL state
cwsl_manage_status_t status = cwsl_get_status();
// Check: if (status == CWSL_STA_REG_TEMPLATE) { ... }

// Reset CWSL module (called on exit from wakeup state)
int cwsl_manage_reset();

// Delete a learned command
// cmd_id: -1 = wildcard (delete all matching)
// group_id: -1 = wildcard
// word_type: CMD_WORD, WAKEUP_WORD, ALL_WORD
int cwsl_delete_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type);

// Delete during registration mode (sends delete message)
int cwsl_delete_word_when_reg(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type);
```

### Recognition

```c
// Start recognition mode for specific word type
int cwsl_recognize_start(cwsl_word_type_t word_type);

// Stop recognition
int cwsl_recognize_stop();
```

### Threshold Tuning

```c
// Set registration and recognition distance thresholds
void cwsl_set_distance_threshold(uint8_t reg_max_distance, uint8_t rec_max_distance);

// Set word info for registration
void cwsl_set_wordinfo(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t wordtype);
```

## Integration Pattern

In `user_msg_deal.c`, CWSL results arrive through the normal ASR message flow. The sample `cwsl_app_sample1.c` demonstrates the callback pattern:

```c
// on_cwsl_reg_start: called when learning starts
_XIF_ int on_cwsl_reg_start(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type)
{
    cwsl_reg_record_stop();  // Stop recording during prompt playback
    cwsl_save_prev_info(cmd_id, group_id, word_type);
    // Play "start learning" prompt, then start recording via callback
}

// cwsl_play_done_callback_with_start_record: called after prompt playback
_XIF_ static void cwsl_play_done_callback_with_start_record(cmd_handle_t cmd_handle)
{
    cwsl_reg_record_start();  // Start recording after prompt finishes
}

// cwsl_app_reset: called on wakeup exit
int cwsl_app_reset();
```

## Sample Project

The `cwsl_sample` project demonstrates:
- Entering CWSL mode via voice command
- Learning custom commands (configurable count via `reg_cmd_list[]`)
- Recognizing learned commands
- Deleting learned commands
- Continuous learning mode

## Best Practices

- Train in a quiet environment
- Speak at normal volume and speed
- 1-3 training repetitions (configurable via `cwsl_init_parameter_t.sg_reg_times`)
- CWSL commands have lower accuracy than pre-built models
- Use CWSL for supplementary commands, not primary commands
