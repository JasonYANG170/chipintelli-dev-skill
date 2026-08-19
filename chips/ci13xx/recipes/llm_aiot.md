# LLM AIoT Development Guide

> Applies to: CI13XX unified SDK targets documented by the selected SDK; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci13xx/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13xx/recipes/llm_aiot.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

## Overview

The LLM AIoT SDK enables both offline voice recognition and cloud-based Large Language Model (LLM) interaction. This allows:
- Offline command recognition (fast, no network needed)
- Online NLP for complex queries (cloud LLM)
- TTS for dynamic speech synthesis
- VPR for speaker identification

## SDK

Use `CI13XX_SDK_LLM_AIOT_2.1.2` (latest) or `CI130X_SDK_LLM_AIOT_2.2.7(1)`.

## Architecture

```text
Voice Input -> ASR (offline) -> Simple command? -> Execute locally
                           -> Complex query? -> Cloud LLM -> TTS response -> Speaker
```

## Components

| Component | Directory | Description |
|---|---|---|
| NLP | `components/nlp/` | Natural language processing (`ci_nlp.h`, `ci_nlp_control.h`) |
| TTS | `components/tts/` | Text-to-speech synthesis (see `chips/ci13xx/recipes/tts.md`) |
| VPR | `components/VPR/` | Voiceprint recognition (see `chips/ci13xx/recipes/vpr.md`) |
| Protocol | `components/protocol/` | Cloud communication protocol |
| CIAS lib | `components/cias_lib/` | CIAS communication library |
| Opus | `components/cias_opus/` | Opus audio codec |
| Speex | `components/cias_speex/` | Speex audio codec |
| G.722 | `components/cias_g722/` | G.722 audio codec |

## Cloud LLM Integration

The LLM AIoT SDK communicates with a cloud LLM service:

1. Voice captured by microphone
2. Offline ASR detects query intent
3. If complex query: audio sent to cloud via Wi-Fi/BLE
4. Cloud LLM processes and returns response
5. TTS synthesizes speech response
6. Audio played through speaker

## Configuration

Enable the desired modules in `user_config.h`:

```c
// Enable TTS (see chips/ci13xx/recipes/tts.md for API)
#define USE_TTS_MODULE             1

// Enable VPR (see chips/ci13xx/recipes/vpr.md for API)
// VPR uses USE_VPR guard in voice_print_recognition.h
```

## TTS Integration

TTS uses a task-based pipeline, not a single convenience API. See `chips/ci13xx/recipes/tts.md` for the real initialization and playback flow using `tts_module_init()`, `tts_play_ctl_start()`, and the serial text receive task.

## VPR Integration

VPR uses a callback-based model with `vpr_init()`, `vpr_start_regist()`, and `vpr_run_one_recognition()`. See `chips/ci13xx/recipes/vpr.md` for the complete API.

## Sample Projects

The `CI13XX_SDK_LLM_AIOT_2.1.2` contains three project variants:
- `offline_asr_llm_aiot_hpout_sample` -- LLM AIoT with headphone output
- `offline_asr_llm_aiot_iis_sample` -- LLM AIoT with IIS audio output
- `offline_asr_llm_aiot_uart_sample` -- LLM AIoT with UART audio output
