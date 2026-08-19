# IR Remote Control with CI13LC

> Applies to: CI1311, CI1312, CI1316x, CI1324x, and CI1332x; confirm exact chip, board, and voice/connectivity feature set before coding.
> SDK: CI13LC SDK selected by the project.
> Evidence: `chips/ci13lc/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13lc/recipes/ir_control.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

## Overview

The CI13LC SDK IR variant (`CI13LC_SDK_IR_V2.0.15`) adds infrared remote control capabilities to the voice recognition system. This allows voice-controlled IR learning and transmission.

## SDK

Use `CI13LC_SDK_IR_V2.0.15`.

## Features

- **IR Learn**: Learn IR codes from existing remote controls
- **IR Send**: Transmit IR codes to control appliances (AC, TV, etc.)
- **Voice-triggered IR**: Use voice commands to send IR codes
- **IR code storage**: Store learned codes in flash

## Configuration

IR control is enabled by default in the IR SDK variant. Check the IR-related component headers in the SDK for the exact API functions available in your version.

## API Usage

The exact IR API function names depend on the SDK version. Check the IR component headers in the `components/` or `driver/` directories of `CI13LC_SDK_IR_V2.0.15` for the real function signatures.

```c pseudocode
// Conceptual IR control flow - check SDK headers for real API
// 1. Learn: Enter IR learning mode, capture IR signal from remote
// 2. Store: Save learned IR code to flash with an ID
// 3. Send: Transmit stored IR code by ID
// 4. Delete: Remove a stored IR code
```

## Voice Integration

IR commands are triggered by voice through the ASR message handler:

```c pseudocode
// In user_msg_deal.c
case MSG_ASR_RESULT:
    // Get semantic ID from ASR message
    // Map semantic ID to stored IR code ID
    // Send IR code
    break;
```

## Typical Application Flow

1. User says "Learn IR code" -> Device enters IR learning mode
2. User points original remote at device and presses button
3. Device learns and stores the IR code
4. User says "Turn on AC" -> Device sends learned AC IR code

## Hardware Requirements

- IR LED (transmitter)
- IR receiver (e.g., HS0038 or equivalent)
- Connected to CI13LC GPIO pins (defined in board file)
