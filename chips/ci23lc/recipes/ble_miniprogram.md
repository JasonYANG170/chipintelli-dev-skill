# Recipe: WeChat Mini-Program + BLE Voice Control (CI23LC)

> **SDK**: `CI23LC_SDK_BLE_V1.3.13`
> **Chips**: CI2312, CI23242

This recipe covers developing a CI23LC device that works with the Chipintelli WeChat mini-program ("启英物联小程序") for BLE-based appliance control. The mini-program provides a UI for device control, status display, and command word information retrieval.

---

## Overview

The CI23LC + mini-program architecture:

```
┌─────────────────┐     BLE Connection      ┌──────────────┐
│ WeChat Mini-    │ ◄─────────────────────► │   CI23LC     │
│ Program         │  Write (0xAE3B) ──>     │   Device     │
│ (启英物联)       │  <── Notify (0xAE3C)    │              │
│                 │  <── CmdNotify(0xAE3D)  │  ASR + Voice │
└─────────────────┘                         └──────────────┘
```

**Three BLE characteristics**:
- `0xAE3B` (WRITE): Phone sends commands to device
- `0xAE3C` (NOTIFY): Device sends state/events to phone
- `0xAE3D` (NOTIFY): Device sends command word info to phone

## Enabling the Mini-Program

### 1. Configuration in `user_config.h`

```c
#define USE_BLE_MOUDLE              1    // Enable BLE
#define USE_CI_APPLET_ENABEL        1    // Use Chipintelli mini-program
#define BLE_ADV_NAME_APPEND_FLASH_ID 1   // Unique name per device (recommended)
#define BLE_USER_DEFINE_ADV_NAME_CONTENT "MyDevice"  // Max 13 bytes with applet!
```

**Name length constraint**: With `USE_CI_APPLET_ENABEL=1`, the total advertising name (including flash ID suffix) must be <= 18 bytes. Flash ID adds 5 bytes (`_XXYY`), so the custom name must be <= 13 bytes. The SDK enforces this:
```c
if (strlen(BLE_USER_DEFINE_ADV_NAME_CONTENT) + (BLE_ADV_NAME_APPEND_FLASH_ID ? 5 : 0) > 18) {
    ci_logerr(CI_LOG_ERROR, "set adv name too long, max 18 bytes!\n");
    return;
}
```

### 2. Device Type Configuration (`cias_demo_config.h`)

```c
#define DEV_DRIVER_EN_ID  DEV_FAN_MAIN_ID  // Select your appliance

// Mini-program protocol version
#define CIAS_PROTOCOL_VER  1  // V1.0 protocol

// App page customization
#define APP_PAGE_TITLE      "语音智能设备"  // Max 9 Chinese chars or 15 alphanumeric
#define APP_PAGE_BACKGROUND "1"             // 1=default, 2=gray, 3=white-black gradient,
                                            //   4=three-color gradient, 5=on/off blue-white
```

### 3. State Report Mode

```c
#define BLE_DEV_STATE_REPORT_JUDGEMENT_MODE 0
// 0: Report state based on command word ID
// 1: Report state based on semantic ID
```

## CIAS Protocol V1 (Phone -> Device)

### Message Structure

```c
#pragma pack(1)
typedef struct {
    uint8_t  pkg_header[2];    // 0xA5, 0x5A
    uint8_t  protocol_ver;     // 0x01
    uint8_t  manufacturer_id;  // Vendor ID
    uint8_t  dev_type;         // dev_type_t (e.g., FAN_DEV=6)
    uint8_t  dev_number;       // Device number
    uint8_t  data_type:4;      // data_type_t (1=setup, 2=event, 3=query, 4=recovery)
    uint8_t  function_type:4;  // Function category
    uint8_t  function_id;      // Specific function
    uint16_t data_len;         // Little-endian data length
    uint8_t  data[8];          // Payload data
} ble_msg_V1_t;
#pragma pack()
```

### Data Types

| Value | Type | Description |
|-------|------|-------------|
| 1 | ATTRIBUTE_SETUP | Set a device property |
| 2 | EVENT_REPORT | Report an event |
| 3 | STATE_QUERY | Query device state |
| 4 | STATE_RECOVERY | Recover state after reconnect |

### Short Commands (ble_cmd_t)

```c
#pragma pack(1)
typedef struct {
    uint8_t pkg_header[2];  // 0xA5, 0x5B
    uint8_t ble_cmd;        // Command code
} ble_cmd_t;
#pragma pack()
```

| ble_cmd | Description |
|---------|-------------|
| 0x01 | Disconnect BLE |
| 0x02 | Get command word info |

## CIAS Protocol (Device -> Phone)

### State Report (via NOTIFY 0xAE3C)

The device sends its full state when any property changes. The fan demo's `fan_query_all()` sends all device attributes:

```c
void fan_query_all(void)
{
    uint8_t send_data[BLE_MSG_DATA_MAX_SIZE] = {0};
    uint8_t len = 0;

    // Build ble_msg_V1_t with current state
    send_data[len++] = 0xA5;  // Header
    send_data[len++] = 0x5A;
    send_data[len++] = 0x01;  // Protocol V1
    send_data[len++] = 0x00;  // Manufacturer ID
    send_data[len++] = FAN_DEV;  // Device type
    send_data[len++] = 9;     // Device number
    // ... add each function's state ...
    send_data[len++] = fan_dev.power;
    send_data[len++] = fan_dev.speed;
    // ...

    // CRC + encrypt
    uint16_t crc = crc16_ccitt(0, send_data, len);
    send_data[len++] = crc >> 8;
    send_data[len++] = crc & 0xFF;
    cias_crypto_data(send_data, len);

    ble_send_payload(send_data, len, BLE_UUID_CIAS_NOTIFY);
}
```

### App Page Customization (via NOTIFY 0xAE3C)

The device can set the mini-program's UI:

```c
// Set page title
void app_title_set(void)
{
    uint8_t send_data[BLE_MSG_DATA_MAX_SIZE] = {0};
    uint8_t len = 0;
    send_data[len++] = 0xA5;  // Header
    send_data[len++] = 0x5A;
    send_data[len++] = 0x05;  // Page title command
    send_data[len++] = strlen(APP_PAGE_TITLE) / 256;  // Length high byte
    send_data[len++] = strlen(APP_PAGE_TITLE) % 256;  // Length low byte
    memcpy(&send_data[len], APP_PAGE_TITLE, strlen(APP_PAGE_TITLE));
    len += strlen(APP_PAGE_TITLE);
    uint16_t crc = crc16_ccitt(0, send_data, len);
    send_data[len++] = crc >> 8;
    send_data[len++] = crc & 0xFF;
    cias_crypto_data(send_data, len);
    ble_send_payload(send_data, len, BLE_UUID_CIAS_NOTIFY);
}

// Set page background (same format, command 0x06)
void app_background_set(void);
```

### Command Word Info (via CMD_NOTIFY 0xAE3D)

When the phone requests command word info (ble_cmd=0x02), the device sends all command words:

```c
USE_XFI bool app_cb_att_read()
{
    uint8_t model_number = cmd_file_get_model_number();  // Number of ASR models
    for (uint8_t model_id = 0; model_id < model_number; model_id++) {
        // First packet: model summary
        sprintf(rf_read_data, "asr_id:%d,dnn_id:%d,cur_model:%d,valid:%d,cmd number:%d,asr_uuid:%s", ...);
        ble_send_payload(rf_read_data, strlen(rf_read_data), BLE_UUID_CIAS_CMD_NOTIFY);

        // Subsequent packets: each command word's info
        // Format: cmd_str_len(1B) + cmd_str(nB) + cmd_id(2B) + semantic_id(4B)
        //         + score(1B) + wake_up_flag(1B) + special_wait_count(1B)
        //         + voice_info: select_type(1B) + option_number(1B) + combinations...
        for (each command word) {
            // Pack into 240-byte chunks, send via ble_send_payload
        }

        // End marker
        sprintf(rf_read_data, "get cmd fnish:%d", model_number);
        ble_send_payload(rf_read_data, strlen(rf_read_data), BLE_UUID_CIAS_CMD_NOTIFY);
    }
}
```

Enable this feature with `APP_GET_CMD_INFO_ENABEL 1` in `cias_demo_config.h`. Note: this consumes significant memory proportional to command word count.

## Data Encryption

All CIAS protocol data is encrypted with `cias_crypto_data()` (XOR-based, from `libcias_crypto.a`):

```c
// Encryption is applied before sending and after receiving
cias_crypto_data(send_data, len);  // Encrypt before ble_send_payload
cias_crypto_data(recv_data, len);  // Decrypt after receiving
```

The encryption key is derived from the BLE connection. This is a lightweight obfuscation, not security-grade encryption.

## Advertising Data Format (CIAS Protocol)

The advertising data is built in `ci_ble_adv_data_init()`:

```
Byte 0:     adv_name_len + 1
Byte 1:     0x09 (Complete Local Name type)
Byte 2..n:  Advertising name string
Byte n+1:   0x02 (Flags type)
Byte n+2:   0x01 (Flags length)
Byte n+3:   0x06 (LE General Discoverable + BR/EDR Not Supported)
Byte n+4:   0x07 (Manufacturer Specific Data type)
Byte n+5:   0xFF (Manufacturer data length)
Byte n+6:   DEV_MTU_TYPE (0x02 = 3.5-gen, long packet support)
Byte n+7:   CONFIG_TYPE (0=normal, 4=broadcast device)
Byte n+8:   DEV_TYPE_ID (device main ID)
Byte n+9:   DEV_NUMBER_ID (device sub ID)
Byte n+10:  CRC16 high byte
Byte n+11:  CRC16 low byte
```

The mini-program uses `DEV_MTU_TYPE`, `CONFIG_TYPE`, `DEV_TYPE_ID`, and `DEV_NUMBER_ID` to identify the device type and display the appropriate control UI.

## Implementing a Custom Appliance

### 1. Define function IDs for your device:

```c
// In your device header
#define MYDEVICE_POWER       0x01
#define MYDEVICE_MODE        0x02
#define MYDEVICE_LEVEL       0x03
#define MYDEVICE_TIMER       0x04
```

### 2. Implement the four functions:

```c
void mydevice_init(void);           // Initialize state
void mydevice_callback(ble_msg_V1_t msg);  // Handle phone command
void mydevice_query(ble_msg_V1_t msg);     // Handle state query
uint8_t mydevice_report(uint32_t cmd_id);  // Report ASR result to phone
```

### 3. Handle `STATE_RECOVERY` (data_type=4):

When the phone reconnects, it sends `STATE_RECOVERY` to get the current device state. Your callback should respond with `mydevice_query_all()`:

```c
void mydevice_callback(ble_msg_V1_t msg)
{
    if (msg.data_type == STATE_RECOVERY) {
        mydevice_query_all();  // Send full state
        return;
    }
    // ... handle ATTRIBUTE_SETUP ...
}
```

## Mini-Program Development

The WeChat mini-program itself is developed separately (not part of the SDK). Key considerations:

1. **UUID matching**: The mini-program must use the same UUIDs as `user_config.h`:
   - Service: `0xAE3A`
   - Write: `0xAE3B`
   - Notify: `0xAE3C`
   - CmdNotify: `0xAE3D`

2. **Protocol version**: Must match `CIAS_PROTOCOL_VER` (currently 1)

3. **CRC**: Must compute CRC16-CCITT and append 2 bytes

4. **Encryption**: Must apply `cias_crypto_data` equivalent (contact Chipintelli for the algorithm)

5. **Advertising identification**: The mini-program scans for devices with the CIAS manufacturer-specific data format

## Testing Flow

1. Flash the CI23LC device with your appliance demo
2. Open the Chipintelli mini-program in WeChat
3. Scan for BLE devices -- your device appears as "MyDevice_XXYY"
4. Connect to the device
5. The mini-program displays the control UI (title from `APP_PAGE_TITLE`)
6. Test voice commands -- ASR results update the mini-program state
7. Test mini-program commands -- device executes and plays voice prompts
8. Disconnect/reconnect -- device sends `STATE_RECOVERY` response
