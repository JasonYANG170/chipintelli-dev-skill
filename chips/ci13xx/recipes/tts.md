# Text-to-Speech (TTS) on CI13XX

## Overview

The TTS component enables dynamic speech synthesis on-chip, allowing the device to speak any text without pre-recorded audio files.

## SDK

Use `CI13XX_SDK_LLM_AIOT_2.1.2` with the `tts` component.

## Source Anchors

- `components/tts/tts_manage.h` -- TTS module init, DNN task management
- `components/tts/tts_play_ctl.h` -- Play control (start/stop/pause/resume/volume/speed)
- `components/tts/tts_serial_text_deal.h` -- Serial text receive task, message queue
- `components/tts/tts_audio.h` -- Audio player task and stream buffer
- `components/tts/tts_init.h` -- TTS initialization
- `components/tts/ci_tts.h` -- Core TTS data structures (`Buffer_split`)

## Architecture

```text
Text input (UART serial) -> tts_seial_reveice_task -> deal_serial_msg
  -> tts_dnn_manage_task (duration + acoustic DNN)
  -> tts_vocoder_task (waveform synthesis)
  -> tts_audio_player_task (DAC output)
```

## Initialization

```c
#include "tts_manage.h"
#include "tts_play_ctl.h"
#include "tts_serial_text_deal.h"

// 1. Initialize TTS module (DNN, ringbuffers, vocoder)
tts_module_init();

// 2. Initialize serial text receive ringbuffer
tts_receive_rb_init(buffer_size);

// 3. Start the serial receive task (creates FreeRTOS task)
//    tts_seial_reveice_task is started internally by sys_tts_msg_task_initial()

// 4. Start playback
tts_play_ctl_start();
```

## Play Control API

From `tts_play_ctl.h`:

```c
// Volume: 0-9
int tts_play_ctl_set_vol(int vol);

// Speed: 0-9
int tts_play_ctl_set_speed(int vol);

// Intonation: 0-9
int tts_play_ctl_set_intonation(int vol);

// Playback control
int tts_play_ctl_start();
int tts_play_ctl_stop();
int tts_play_ctl_pause();
int tts_play_ctl_resume();
int tts_play_ctl_status();
```

## Text Input via UART Serial

Text is fed into the TTS engine via the serial receive ringbuffer. The `tts_seial_reveice_task` processes incoming text messages:

```c
#include "tts_serial_text_deal.h"

// The system message queue delivers text to TTS
// deal_serial_msg() parses the message and feeds text to the DNN pipeline
uint32_t deal_serial_msg(sys_tts_msg_t *msg);

// Ringbuffer access for direct text injection
ringbuffer_t* tts_receive_get_rb_handle(void);
```

## DNN Management API

From `tts_manage.h`:

```c
// Initialize the entire TTS module
void tts_module_init(void);

// Start the TTS system message task
void sys_tts_msg_task_initial(void);

// DNN compute status
uint8_t tts_dnn_get_run_dur_status(void);
void tts_dnn_set_run_dur(uint8_t bRunDur);

// Send DNN computation message
int tts_manage_send_dnn_msg(stDnn_msg* pdnn_msg);
```

## Configuration

Enable TTS in `user_config.h`:

```c
#define USE_TTS_MODULE             1
```

The TTS engine uses GB2312-to-UTF8 conversion (`tts_gb2312toUtf8.h`) for Chinese text input.

## Use Cases

- Dynamic response to user queries (with LLM cloud integration)
- Reading text content aloud
- Weather/temperature announcements
- Time announcements
- Error messages not pre-recorded

## Limitations

- TTS requires more memory than pre-recorded prompts
- Synthesis quality depends on the TTS DNN model
- Chinese and English supported
- May have slight latency compared to pre-recorded audio
