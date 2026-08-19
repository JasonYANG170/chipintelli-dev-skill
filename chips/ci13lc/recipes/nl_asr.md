# Natural Language ASR (离线自然说)

> Applies to: CI1311, CI1312, CI1316x, CI1324x, and CI1332x; confirm exact chip, board, and voice/connectivity feature set before coding.
> SDK: CI13LC SDK selected by the project.
> Evidence: `chips/ci13lc/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13lc/recipes/nl_asr.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

## Overview

NL ASR (Natural Language / 离线自然说) allows free-form natural language commands instead of fixed command words. Users can say things in many different ways and the device understands the intent.

For example, all of these are recognized as "turn on the light":
- "打开灯" (Turn on the light)
- "把灯打开" (Open the light)
- "开灯" (Light on)
- "帮我开一下灯" (Help me turn on the light)

## SDK

Use `CI13LC_SDK_V2.0.15` with `nl_asr_sample` project.

## How It Works

1. The NL model is trained on the voice AI platform with many sentence variations
2. Each sentence is mapped to a semantic intent
3. The model can also extract parameters (e.g., "set to 50" -> intent=SET, value=50)
4. Recognition runs fully offline on the chip

## Configuration

NL ASR requires a special NL model generated on the voice AI platform:
- Place NL model in `firmware/asr/`
- Configure the model type in the SDK

## Handling NL Results

NL results arrive through the standard ASR message flow, same as regular command words. The semantic ID from the NL model maps to user-defined actions.

```c pseudocode
// In user_msg_deal.c
// NL results arrive as standard ASR messages
// Use cmd_info API to look up the semantic ID
case MSG_ASR_RESULT:
    // Get semantic ID from the ASR message
    // Map semantic ID to action
    // NL models may return additional parameter data
    break;
```

## Source Anchors

- `projects/nl_asr_sample/src/user_msg_deal.c` -- Sample message handling
- `projects/nl_asr_sample/src/ci_nlp_user.h` -- NLP user configuration
- `components/nlp/ci_nlp.h` -- NLP API
- `components/cmd_info/command_info.h` -- Command info API

## Sample Project

The `nl_asr_sample` demonstrates natural language understanding for a smart home device.

## Advantages over Fixed Commands

- More natural user experience
- No need to memorize exact command words
- Handles dialects and speech variations
- Supports parameter extraction (e.g., "set temperature to 25")
