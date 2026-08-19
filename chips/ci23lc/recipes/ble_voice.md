# Recipe: BLE + Voice Combo Development (CI23LC)

> **SDK**: `CI23LC_SDK_BLE_V1.3.13`
> **Chips**: CI2312, CI23242

This recipe explains how BLE and ASR run concurrently on CI23LC, how messages route between them, and how to implement bidirectional voice + BLE control.

---

## Architecture Overview

```
┌──────────────────────────────────────────────────────────┐
│                    CI23LC (RISC-V N300)                    │
│                                                           │
│  ┌─────────┐    ┌──────────────┐    ┌─────────────────┐  │
│  │  ASR    │───▶│ sys_msg_queue│───▶│UserTaskManage   │  │
│  │ Engine  │    │              │    │Process          │  │
│  └─────────┘    │              │    │ (plays prompts, │  │
│                 │              │    │  executes cmds) │  │
│  ┌─────────┐    │              │    └─────────────────┘  │
│  │  BLE    │───▶│              │           ▲             │
│  │ Module  │    └──────────────┘           │             │
│  │ (UART1) │───────────────────────────────┘             │
│  └─────────┘         │                                    │
│       ▲              ▼                                    │
│       │       ┌──────────────┐                           │
│       │       │ci_ble_recv_  │                           │
│       │       │task          │                           │
│       │       └──────────────┘                           │
│       │                  │                                │
│  ┌────┴─────┐      ┌─────┴──────┐                        │
│  │ble_main_ │      │ble_msg_    │                        │
│  │task      │      │queue       │                        │
│  │(UART     │      └────────────┘                        │
│  │ protocol)│                                            │
│  └──────────┘                                            │
└──────────────────────────────────────────────────────────┘
         │ UART1 (921600 baud)
         ▼
   ┌───────────┐
   │ BLE Chip  │ ◄──────► Phone (WeChat Mini-Program)
   │ (patch    │
   │  loaded)  │
   └───────────┘
```

## Concurrent Operation

ASR and BLE run in **separate FreeRTOS tasks** with no mutual blocking:

| Task | Stack | Priority | Role |
|------|-------|----------|------|
| `ble_main_task` | 480 words | 4 | BLE UART protocol, firmware download, advertising, event loop |
| `ci_ble_recv_task` | 480 words | 4 | Process received BLE messages, dispatch to device callbacks |
| `UserTaskManageProcess` | 480 words | 4 | Handle system messages (ASR + BLE), play prompts |
| ASR tasks | (internal) | (internal) | Audio capture, feature extraction, recognition |

The ASR engine runs on the Nuclei N300 core with hardware acceleration (BNPU). BLE runs entirely in software on the host core via UART1. They share the system heap but use separate message queues.

## Message Routing

### Path 1: Voice Command -> BLE Report

```
1. User speaks command word
2. ASR engine recognizes -> MSG_ASR_RESULT in sys_msg_queue
3. UserTaskManageProcess receives -> calls deal_ble_send_msg(cmd_handle)
4. deal_ble_send_msg extracts cmd_id -> calls e.g. fan_report(cmd_id)
5. fan_report updates device state -> calls fan_query_all()
6. fan_query_all sends BLE state packet via ble_send_payload()
7. Phone receives state update via NOTIFY characteristic (0xAE3C)
```

### Path 2: Phone Command -> Voice Prompt

```
1. Phone sends BLE command via WRITE characteristic (0xAE3B)
2. ble_main_task receives BLE_EVENT_DATA_REP
3. Calls gBleInitCfg.ble_recv_data_callback (= ci_ble_recv_data_handle)
4. ci_ble_recv_data_handle:
   a. Decrypts with cias_crypto_data()
   b. Verifies CRC16
   c. Parses as ble_msg_V1_t (header 0xA5 0x5A)
   d. Sends to ble_msg_queue
5. ci_ble_recv_task receives from ble_msg_queue
6. Dispatches to device callback (e.g., fan_callback)
7. fan_callback executes action, sends SYS_MSG_TYPE_BLE to sys_msg_queue
8. UserTaskManageProcess receives SYS_MSG_TYPE_BLE
9. Plays voice prompt via prompt_play_by_cmd_string() or cmd_id
```

### Path 3: 2.4G Remote -> Voice + BLE

```
1. 2.4G remote broadcasts advertising data
2. BLE module scans (if CIAS_BLE_SCAN_ENABLE=1)
3. ble_main_task receives BLE_EVENT_ADV_REP
4. Calls gBleInitCfg.ble_recv_adv_data_callback (= ci_ble_recv_adv_data_handle)
5. ci_ble_recv_adv_data_handle:
   a. Validates CID (0x03 0x09 0x4C 0x5A)
   b. Checks frame counter (dedup)
   c. Maps remote key to cmd_id (e.g., key 1 -> TURN_ON)
   d. Calls fan_report(cmd_id) to update BLE state
   e. Sends SYS_MSG_TYPE_BLE to sys_msg_queue for voice playback
```

## System Message Structure

```c
// System message (32 bytes data max)
typedef struct {
    sys_msg_type_t msg_type;    // SYS_MSG_TYPE_ASR, SYS_MSG_TYPE_BLE, etc.
    uint8_t msg_data[SYS_MAX_MSG_LEN];  // 32 bytes
} sys_msg_t;

// BLE message payload (fits in msg_data)
typedef struct {
    uint8_t  play_type;     // 1: play by cmd_id; 2: play by cmd_str
    uint32_t cmd_id;        // Command word ID for prompt playback
    uint32_t select_index;  // For multi-prompt commands (-1 = default)
    char    *cmd_str;       // String for play_type=2
} sys_msg_ble_data_t;
```

## Implementation: Custom Device

### 1. Define your device type in `cias_demo_config.h`:

```c
#define DEV_MYDEVICE_MAIN_ID  0x0A

#if (DEV_DRIVER_EN_ID == DEV_MYDEVICE_MAIN_ID)
    #define DEV_TYPE_ID    DEV_MYDEVICE_MAIN_ID
    #define DEV_NUMBER_ID  1
    #define CONFIG_TYPE    0
#endif
```

### 2. Implement device logic (`cias_mydevice_msg_deal.c`):

```c
#include "cias_ble_msg_deal.h"

static mydevice_dev_t mydevice_dev;

void mydevice_init(void)
{
    mydevice_dev.power = FUN_TURN_OFF;
    // Initialize device state
}

uint8_t mydevice_report(uint32_t cmd_id)
{
    // Called when ASR recognizes a command
    // Update device state based on cmd_id
    switch (cmd_id) {
        case TURN_ON:  mydevice_dev.power = FUN_TURN_ON;  break;
        case TURN_OFF: mydevice_dev.power = FUN_TURN_OFF; break;
        default: return 0;  // Not handled
    }
    mydevice_query_all();  // Report full state to phone
    return 1;
}

void mydevice_callback(ble_msg_V1_t msg)
{
    // Called when phone sends a command via BLE
    sys_msg_ble_data_t msg_data = {.cmd_str = NULL};
    msg_data.play_type = 1;
    msg_data.select_index = -1;

    switch (msg.function_id) {
        case MYDEVICE_POWER:
            if (msg.data[0] == FUN_TURN_ON) {
                msg_data.cmd_id = TURN_ON;
                mydevice_dev.power = FUN_TURN_ON;
            } else {
                msg_data.cmd_id = TURN_OFF;
                mydevice_dev.power = FUN_TURN_OFF;
            }
            break;
    }

    // Send to system message queue for voice prompt playback
    if (msg_data.cmd_id) {
        sys_msg_t ble_sys_msg = {.msg_type = SYS_MSG_TYPE_BLE};
        memcpy(ble_sys_msg.msg_data, &msg_data, sizeof(sys_msg_ble_data_t));
        send_sys_msg_inner(&ble_sys_msg, sizeof(ble_sys_msg), NULL);
    }
    mydevice_query_all();
}

void mydevice_query(ble_msg_V1_t msg) { /* Query specific attribute */ }

// Send full state to phone via BLE NOTIFY
void mydevice_query_all(void)
{
    uint8_t send_data[BLE_MSG_DATA_MAX_SIZE] = {0};
    // Build ble_msg_V1_t packet with current state
    // ... fill header, dev_type, function states ...
    cias_crypto_data(send_data, len);
    ble_send_payload(send_data, len, BLE_UUID_CIAS_NOTIFY);
}
```

### 3. Add to `dev_state_init()` in `cias_ble_msg_deal.c`:

```c
USE_XFI void dev_state_init(void)
{
    // ... existing device inits ...
    #elif (DEV_DRIVER_EN_ID == DEV_MYDEVICE_MAIN_ID)
        mydevice_init();
    #endif
}
```

### 4. Add to `ci_ble_recv_task()` switch:

```c
case MYDEVICE_DEV:
{
    if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
        mydevice_callback(recv_ble_msg);
    else if (recv_ble_msg.function_type == STATE_QUERY)
        mydevice_query(recv_ble_msg);
}
break;
```

### 5. Add to `deal_ble_send_msg()`:

```c
#elif (DEV_DRIVER_EN_ID == DEV_MYDEVICE_MAIN_ID)
    mydevice_report(cmd_id);
#endif
```

## ASR Pause/Resume via BLE

The fan demo shows how to control ASR from the phone:

```c
// In fan_callback, FAN_ASR case:
if (msg.data[0] == FUN_TURN_OFF) {
    fan_dev.asr_status = FUN_TURN_OFF;
    pause_asr();   // Suspend ASR engine
    // Play "<ASR disabled>" prompt
    msg_data.cmd_str = "<已关闭语音识别>";
    msg_data.play_type = 2;  // String playback
} else if (msg.data[0] == FUN_TURN_ON) {
    fan_dev.asr_status = FUN_TURN_ON;
    resume_asr();  // Resume ASR engine
    msg_data.cmd_str = "<已打开语音识别>";
    msg_data.play_type = 2;
}
```

## Volume Control via BLE

```c
// In fan_callback, FAN_SPEAKER case:
case FAN_SPEAKER:
    if (msg.data[0] == VOICE_UP_DATA) {
        uint8_t vol = vol_set(vol_get() + 1);
        msg_data.cmd_id = VOICE_UP;
        msg_data.select_index = (vol == VOLUME_MAX) ? 1 : 0;
    }
    break;
```

## Debugging Tips

1. **Monitor task status**: The main loop prints task stats every 15s (when `COMMAND_LINE_CONSOLE_EN=0`):
   ```
   TaskName        Priority  TaskNumber  MinStk
   ble_main_task   4         2           120
   ci_ble_recv_t.. 4         3           200
   ```

2. **Check heap**: `system heap free:XXKB` in the periodic printout. If <5KB after BLE init, reduce queue sizes.

3. **BLE log messages**: 
   - `"ble app connect"` -- phone connected
   - `"ble app disconnect"` -- phone disconnected
   - `"ble_recv_msg type = 0x06"` -- received fan command
   - `"crc erro"` -- CRC mismatch in received data

4. **Use `USE_XFI _XIF_`**: Run BLE code from flash to save RAM. The tradeoff is slower execution, but BLE UART protocol is not latency-critical.
