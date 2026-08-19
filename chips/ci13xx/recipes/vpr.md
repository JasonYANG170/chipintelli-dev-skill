# Voiceprint Recognition (VPR)

## Overview

VPR identifies speakers by their voice characteristics. This enables:
- Speaker authentication
- Multi-user recognition
- Personalized responses

## SDK

Use `CI13XX_SDK_LLM_AIOT_2.1.2` with the `VPR` component.

## Source Anchors

- `components/VPR/voice_print_recognition.h` -- Single-template VPR interface
- `components/VPR/voice_print_wman_recognition.h` -- Multi-template (WMAN) VPR interface

> **Variant warning:** `voice_print_recognition.h` declares `vpr_delete()` with no arguments. `voice_print_wman_recognition.h` declares `vpr_delete(int index)`. Do not mix the two headers in one project. The `vpr_delete` signature differs.

## API Usage (from `voice_print_recognition.h`)

### Initialization and Callback

The VPR module uses a callback-based event model. Register a callback at init time:

```c
#include "voice_print_recognition.h"

// Callback type: receives result status and template index
typedef void (*vpr_callback_t)(vpr_callback_rst_t rst, int rec_index);

// Result enum values:
// vpr_reg_successed     - Registration succeeded
// vpr_reg_failed        - Registration failed
// vpr_rec_successed     - Recognition succeeded
// vpr_rec_failed        - Recognition failed
// vpr_reg_resample      - Continue recording (need more samples)
// vpr_reg_resample_failed - Repeat recording failed

static void app_vpr_callback(vpr_callback_rst_t result, int rec_index)
{
    switch (result) {
    case vpr_reg_successed:
        // Template registered successfully, rec_index = template ID
        break;
    case vpr_rec_successed:
        // Speaker recognized, rec_index = matched template ID
        break;
    case vpr_reg_resample:
        // Need another voice sample for registration
        break;
    default:
        break;
    }
}
```

### Registration (Enrollment)

```c
// Initialize VPR module with callback
int ret = vpr_init(app_vpr_callback);
// 0 = success, non-zero = error code

// Start voiceprint registration
ret = vpr_start_regist();
// 0 = started, -1 = failed

// Stop registration (e.g., user cancelled)
ret = vpr_stop_regist();
// 0 = no error, non-zero = error code

// Get current registration ID
int reg_id = vpr_get_current_reg_id();
```

### Recognition

```c
// Execute one recognition cycle
int ret = vpr_run_one_recognition();
// 0 = no error, non-zero = error code
// Result arrives via the callback (vpr_rec_successed / vpr_rec_failed)
```

### Template Management

```c
// Delete the currently recognized template (single-template variant)
int ret = vpr_delete();
// 0 = success, -1 = invalid template

// For multi-template (WMAN) variant, use:
// int vpr_delete(int index);

// Delete all templates
int ret = vpr_clear();
// 0 = no error, non-zero = error code
```

### Status Check

```c
uint8_t status = vpr_get_status();
```

## Feature Guard

VPR is conditionally compiled with `USE_VPR`. Ensure this macro is defined (set to 1) in `user_config.h` or the SDK build configuration.

## Best Practices

- Enroll in a quiet environment
- Multiple enrollment samples may be needed (the callback returns `vpr_reg_resample` requesting more samples)
- Speak naturally during enrollment
- Combine with ASR for voice-command + speaker-ID
- After enrollment, the callback `vpr_reg_successed` signals the template is saved
