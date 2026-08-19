# CI13LC Example Projects

## CI13LC_SDK_V2.0.15 Projects

| Project | Path | Description |
|---|---|---|
| offline_asr_sample | `projects/offline_asr_sample/` | Standard offline ASR with wakeup + command words |
| cwsl_sample | `projects/cwsl_sample/` | Command Word Self-Learning (CWSL) - on-device custom word training |
| multi_intents_asr_sample | `projects/multi_intents_asr_sample/` | Multi-intent ASR (one utterance -> multiple actions) |
| nl_asr_sample | `projects/nl_asr_sample/` | Natural Language ASR (离线自然说) - free-form command understanding |
| cwsl_multi_intents_sample | `projects/cwsl_multi_intents_sample/` | CWSL + multi-intent combined |

## CI13LC_SDK_2.1.5 Projects

Similar project set, newer SDK version. Prefer this for new projects.

## CI13LC_SDK_IR_V2.0.15 Projects

| Project | Description |
|---|---|
| IR control sample | Voice + IR remote control (learn/send IR codes) |

## CI13LC_SDK_NN_ENC_V1.2.6 Projects

| Project | Description |
|---|---|
| NN encoder sample | Neural network encoder for custom model deployment |

## Project Structure

Each project follows the standard CI13LC structure:
```
projects/<sample>/
  firmware/
    asr/          # ASR model files (from voice AI platform)
    dnn/          # DNN model files
    user_file/    # User data files
    voice/        # Voice prompt audio files
  project_file/
    Makefile      # Build file (includes common_head.mk)
    source_file.prj  # Source list (Lua-parsed)
  src/
    main.c        # Main entry
    user_config.h # User configuration
    system_msg_deal.c  # System message handler
    user_msg_deal.c    # User message handler
    ci_ssp_config.c    # Voice signal processing config
    ci13lc.lds    # Linker script
```
