# Recipe: BLE Broadcast Mode for Voice Control (CI23LC)

> **SDK**: `CI23LC_SDK_BLE_V1.3.13`
> **Chips**: CI2312, CI23242

BLE broadcast mode allows CI23LC to receive commands from a 2.4G remote controller (Chipintelli proprietary protocol) without establishing a BLE connection. The BLE module scans for advertising packets from the remote, parses the key code, and routes it as a voice command.

---

## Overview

In broadcast mode, the CI23LC BLE module acts as a **scanner** rather than an advertiser (though it can do both simultaneously). A Chipintelli 2.4G remote controller broadcasts advertising packets containing key codes. The BLE module receives these via the scan channel and forwards the raw data to the MCU.

```
┌──────────────┐    BLE Advertising    ┌──────────┐    UART1    ┌──────────┐
│ 2.4G Remote  │ ────────────────────> │ BLE Chip │ ─────────> │  CI23LC  │
│ (broadcaster)│  2402/2426/2480 MHz  │ (scanner)│  921600    │  (MCU)   │
└──────────────┘                       └──────────┘            └──────────┘
                                                                  │
                                                          Voice prompt + action
```

## Configuration

### 1. Enable Scan in `ble_param_config.h`

```c
#define CIAS_BLE_SCAN_ENABLE              1    // Enable 2.4G remote scan
#define CIAS_BLE_SCAN_REMOTE_CONTROL_LEN  0x16 // 22 bytes - remote data length
```

### 2. Set Scan Channel and Efficiency

The scan is configured in `ble_main_task()`:

```c
#if CIAS_BLE_SCAN_ENABLE
    ble_adv_scan_efficiency_set(LOW);  // LOW (0x40) = lower power, less frequent scan
    ble_adv_scan_status_set(ADV_SCAN_ON, ADV_SCAN_CHAN_0, CIAS_BLE_SCAN_REMOTE_CONTROL_LEN);
#endif
```

**Scan efficiency options** (from `ble_communicate.h`):
```c
typedef enum {
    VERY_HIGH = 0x10,  // Highest scan frequency, most power
    HIGH      = 0x20,
    MIDDLE    = 0x30,
    LOW       = 0x40,  // Default for remote scan
    VERY_LOW  = 0x50,  // Lowest power, may miss packets
} ble_adv_scan_efficiency_t;
```

**Scan channels**:
```c
ADV_SCAN_CHAN_0 = 0x37,  // 2402 MHz
ADV_SCAN_CHAN_1 = 0x38,  // 2426 MHz
ADV_SCAN_CHAN_2 = 0x39,  // 2480 MHz
```

The default config scans on channel 0 (2402 MHz). The remote must broadcast on the same channel.

### 3. Advertising + Scanning Simultaneously

CI23LC can advertise (for phone connection) AND scan (for 2.4G remote) at the same time:

```c
// In ble_main_task:
ble_adv_status_set(ADV_ON, ADV_CHAN_2);      // Advertise on channel 2
// ...
ble_adv_scan_status_set(ADV_SCAN_ON, ADV_SCAN_CHAN_0, CIAS_BLE_SCAN_REMOTE_CONTROL_LEN);  // Scan on channel 0
```

## Remote Data Format

The 2.4G remote advertising packet format (received by `BLE_EVENT_ADV_REP`):

```
Offset  Length  Description
0       1       cmd: 0x02
1       1       opcode: 0x2A
2       1       length: 0x27 (39 bytes total)
3       2       Header: 0x42 0x25
5       6       MAC address (e.g., 0x46 0x0B 0xAF 0x43 0x98 0xAF)
11      4       CID: 0x03 0x09 0x4C 0x5A (Chipintelli ID)
15      1       Frame marker: 0xCC
16      1       Frame counter (for dedup)
17      2       Padding
19      1       Key code (1-12)
20+     ...     Additional data
```

The `adv_scan_len` parameter in `ble_adv_scan_status_set()` must be set to valid_data + 12. For the default remote: 31 (valid) + 12 = 43 bytes. However, `CIAS_BLE_SCAN_REMOTE_CONTROL_LEN` is set to 0x16 (22), which is the raw data portion length.

## Processing Remote Data

The received advertising data is handled in `ble_adv_msg_deal.c`:

```c
// CID for Chipintelli 2.4G remote
const uint8_t cid[4] = {0x03, 0x09, 0x4c, 0x5a};

bool ci_ble_recv_adv_data_handle(unsigned char *buf)
{
    static uint16_t recv_counter = 0xff;

    // 1. Validate CID and frame counter (dedup)
    if ((memcmp(cid, &buf[8], 4) != 0) || (0xcc != buf[14]) || (recv_counter == buf[16]))
        return 0;

    recv_counter = buf[16];  // Update last seen counter

    // 2. Extract key code
    uint8_t recv_cmd = buf[19];
    uint32_t cmd_id = 0;
    uint32_t select_index = 0;

    // 3. Map remote key to ASR command ID
    switch (recv_cmd) {
        case 1:  cmd_id = TURN_ON;       break;
        case 2:  cmd_id = TURN_OFF;      break;
        case 3:  cmd_id = LIGHT_RAISE;   break;
        case 4:  cmd_id = SPEED_RAISE;   break;
        case 5:  // Toggle light
            if (fan_dev.lamp == FUN_TURN_OFF)
                cmd_id = LAMP_ON;
            else
                cmd_id = LAMP_OFF;
            select_index = 1;
            break;
        case 6:  cmd_id = SPEED_REDUCE;  break;
        case 7:  cmd_id = LIGHT_REDUCE;  break;
        case 8:  cmd_id = LAMP_ON;       break;
        case 9:  cmd_id = WARM_LAMP_ON;  break;
        case 10: cmd_id = TIMING_OFF;    break;
        case 11: // Timer decrease
            if (fan_dev.timing == FUN_TURN_OFF) {
                cmd_id = TIMMING_1H;
            } else {
                if (--fan_dev.timing <= DEV_TIMING_MIN)
                    cmd_id = TIMMING_1H;
                else
                    cmd_id = fan_dev.timing - DEV_TIMING_MIN + TIMMING_1H;
            }
            break;
        case 12: // Timer increase
            if (fan_dev.timing == FUN_TURN_OFF) {
                cmd_id = TIMMING_1H;
            } else {
                if (++fan_dev.timing >= DEV_TIMING_MAX)
                    cmd_id = TIMMING_8H;
                else
                    cmd_id = fan_dev.timing - DEV_TIMING_MIN + TIMMING_1H;
            }
            break;
    }

    // 4. Update BLE state + trigger voice prompt
    if (cmd_id) {
        fan_report(cmd_id);  // Update device state for BLE reporting
        sys_msg_t sys_msg;
        sys_msg.msg_type = SYS_MSG_TYPE_BLE;
        sys_msg_ble_data_t *msg_data = (sys_msg_ble_data_t *)sys_msg.msg_data;
        msg_data->play_type = 1;
        msg_data->cmd_id = cmd_id;
        msg_data->select_index = select_index;
        send_sys_msg_inner(&sys_msg, sizeof(sys_msg), NULL);
    }
    return 0;
}
```

## Customizing for Your Remote

### 1. Update the CID

If your remote uses a different manufacturer CID, change:
```c
const uint8_t cid[4] = {0x03, 0x09, 0x4c, 0x5a};  // Replace with your CID
```

### 2. Update the Key Mapping

Modify the `switch (recv_cmd)` block to match your remote's key codes and your ASR command IDs.

### 3. Optional: Decrypt Remote Data

The `covent_data_adv_to_ble()` function shows how to decrypt encrypted remote data:
```c
bool covent_data_adv_to_ble(unsigned char *recv_data, unsigned char *buf)
{
    uint8_t secret_key = (buf[8] ^ buf[9]) + buf[2];  // XOR-based key derivation
    for (int i = 0; i < PACKET_LEN; i++) {
        recv_data[i] = buf[3+i] ^ secret_key;  // Decrypt each byte
    }
}
```

## Broadcast Group Mode

For RGB light devices, there's a broadcast group mode. When `CIAS_BLE_ADV_GROUP_MODE_ENABEL` is defined:

```c
// In cias_demo_config.h
#if CIAS_BLE_ADV_GROUP_MODE_ENABEL
    #define CONFIG_TYPE  4  // Distinguishes broadcast device in mini-program
#endif
```

This sets `CONFIG_TYPE=4` in the advertising data, allowing the mini-program to identify the device as a broadcast group device.

## Power Considerations

- **Scanning increases power consumption**: The BLE module radio is active more frequently
- `LOW` efficiency (0x40) is the default tradeoff
- For battery-powered devices, use `VERY_LOW` (0x50) and accept some packet loss
- For responsiveness, use `MIDDLE` (0x30) or `HIGH` (0x20)
- The scan window/duty cycle is set internally by the BLE module firmware

## Limitations

1. **One-way communication**: The remote can only send commands; no acknowledgment or state query
2. **Channel matching**: Remote and scanner must use the same channel
3. **Frame dedup**: The `recv_counter` prevents duplicate processing of the same key press
4. **No encryption by default**: The raw advertising data is unencrypted (optional XOR decryption available)
5. **BLE connection priority**: If a phone is connected, scanning may be less frequent due to BLE scheduling
