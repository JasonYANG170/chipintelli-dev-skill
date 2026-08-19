# Sound Event Detection

> Applies to: CI13XX unified SDK targets documented by the selected SDK; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci13xx/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13xx/recipes/sound_event.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

## Overview

Sound event detection identifies non-speech audio events such as:
- Baby crying (婴儿哭声检测)
- Snoring (鼾声检测)
- Glass breaking
- Alarm sounds
- Dog barking

## SDK

Use `CI13XX_SDK_ASR_ALG_V2.7.12` with the `sound_event_detection` algorithm module.

## Component

The `components/alg/sound_event_detection/` directory contains the detection algorithm.

## Configuration

Enable sound event detection in the SDK configuration. Check the component header for the exact macro and event type enums available in your SDK version.

## Handling Detection Results

Sound event detection results arrive through the system message queue. The exact message type and data structure depend on the SDK version. Refer to the component header in `components/alg/sound_event_detection/` for the specific types.

```c pseudocode
// Conceptual flow - check the actual component header for real types
// In user_msg_deal.c or system_msg_deal.c:
case MSG_SOUND_EVENT_TYPE:
    // Read event type and confidence from the message
    // The exact field names depend on the SDK version
    break;
```

## Use Cases

- Baby monitors: Detect crying and alert parents
- Sleep monitoring: Detect snoring patterns
- Security: Detect glass break or alarm sounds
- Elderly care: Detect distress sounds
