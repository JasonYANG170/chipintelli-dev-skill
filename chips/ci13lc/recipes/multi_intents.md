# Multi-Intent ASR Recognition

> Applies to: CI1311, CI1312, CI1316x, CI1324x, and CI1332x; confirm exact chip, board, and voice/connectivity feature set before coding.
> SDK: CI13LC SDK selected by the project.
> Evidence: `chips/ci13lc/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13lc/recipes/multi_intents.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

## Overview

Multi-intent recognition allows a single voice command to trigger multiple actions. For example: "Turn on the light and set brightness to 50%" triggers two actions.

## SDK

Use `CI13LC_SDK_V2.0.15` with `multi_intents_asr_sample` project.

## How It Works

The ASR engine parses the utterance and identifies multiple intents within a single command. Each intent has its own semantic ID.

## Configuration

Multi-intent is configured via the voice AI platform when generating the ASR model. In the SDK, results arrive through the standard ASR message flow.

## Handling Multi-Intent Results

Multi-intent results arrive through the standard `sys_msg_queue` as ASR messages. The sample project `multi_intents_asr_sample` demonstrates how to process multiple semantic IDs from a single recognition event.

```c pseudocode
// In user_msg_deal.c
// The exact message structure depends on the SDK version
// Check system_msg_deal.h and the NLP headers for real types
case MSG_ASR_RESULT:
    // Process the ASR result, which may contain multiple intents
    // Use cmd_info_get_command_id() and related cmd_info API
    // to extract semantic IDs and parameters
    break;
```

## Source Anchors

- `projects/multi_intents_asr_sample/src/user_msg_deal.c` -- Sample message handling
- `projects/multi_intents_asr_sample/src/ci_nlp_user.h` -- NLP user configuration
- `components/nlp/ci_nlp.h` -- NLP API
- `components/nlp/ci_nlp_control.h` -- NLP control
- `components/cmd_info/command_info.h` -- Command info API for extracting semantic IDs

## Sample Project

The `multi_intents_asr_sample` demonstrates handling multi-intent commands for a smart home scenario.
