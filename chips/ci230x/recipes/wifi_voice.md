# Recipe: Wi-Fi + Voice Combo Development (CI230X)

> **SDK**: `CI230X_wifi_combo_sdk_release_v1.1.1`
> **Chips**: CI2305, CI2306 (LN882H + CI13xx)

The CI230X is a dual-chip solution: CI13xx (RISC-V, offline ASR) + LN882H (ARM Cortex-M4, Wi-Fi/BLE). This recipe explains how the two chips work together, how voice commands route to the cloud, and how to develop combo applications.

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        CI230X Module                         │
│                                                              │
│  ┌──────────────────┐         SDIO          ┌─────────────┐ │
│  │   CI13xx         │ ◄──────────────────► │   LN882H    │ │
│  │   (RISC-V N300)  │                      │ (Cortex-M4) │ │
│  │                  │                      │             │ │
│  │  • Microphone    │                      │  • WiFi     │ │
│  │  • ASR Engine    │                      │  • BLE      │ │
│  │  • Audio Player  │                      │  • LwIP     │ │
│  │  • Voice Prompts │                      │  • Cloud    │ │
│  │  • Audio Algs    │                      │  • OTA      │ │
│  └──────────────────┘                      └──────┬──────┘ │
│                                                   │        │
└───────────────────────────────────────────────────┼────────┘
                                                    │
                                              ┌─────┴──────┐
                                              │  Internet  │
                                              │  (WiFi)    │
                                              └─────┬──────┘
                                                    │
                                              ┌─────┴──────┐
                                              │  Cloud     │
                                              │  (TVS,     │
                                              │   Tuya,    │
                                              │   etc.)    │
                                              └────────────┘
```

## Dual-Chip Communication

### SDIO Message Flow

The CI13xx and LN882H communicate via SDIO. The message handling is in `app/cias_aiot_wifi/cias_msg_handle/`:

- `cias_communication.c` -- Main communication protocol
- `cias_slave_message_handle.c` -- Handles messages from CI13xx (the "slave" voice chip)

### Message Types

The CI13xx sends the following message types to LN882H via SDIO:

1. **ASR Result**: Voice command recognized -> forward to cloud or execute locally
2. **Audio Data**: Compressed audio (Speex) for cloud voice assistant
3. **Status**: CI13xx status (idle, recognizing, playing)
4. **OTA Request**: CI13xx requests firmware update

The LN882H sends to CI13xx:

1. **Cloud Commands**: Text commands from cloud -> TTS playback on CI13xx
2. **Configuration**: ASR model switching, volume control
3. **OTA Firmware**: New CI13xx firmware for audio OTA

## Voice Command Routing

### Path 1: Offline Voice Command (Local)

```
1. User speaks command word
2. CI13xx ASR engine recognizes
3. CI13xx executes local action (GPIO, UART, etc.)
4. CI13xx plays voice prompt
5. CI13xx sends ASR result to LN882H via SDIO
6. LN882H reports state to cloud (optional)
```

### Path 2: Online Voice Command (Cloud)

```
1. User speaks (wakeword + query)
2. CI13xx detects wakeword, starts recording
3. CI13xx compresses audio (Speex) and sends to LN882H via SDIO
4. LN882H uploads audio to cloud (TVS/Tuya/etc.)
5. Cloud processes speech -> returns text response + TTS audio
6. LN882H receives response
7. LN882H sends TTS audio to CI13xx via SDIO for playback
8. CI13xx plays the TTS response
```

### Path 3: Cloud-to-Device Command

```
1. Cloud sends command (e.g., from mobile app)
2. LN882H receives via WebSocket/HTTP
3. LN882H sends command to CI13xx via SDIO
4. CI13xx executes action and plays voice prompt
```

### Path 4: BLE Provisioning

```
1. Phone connects to LN882H via BLE
2. Phone sends WiFi credentials
3. LN882H configures WiFi and connects to router
4. LN882H sends success status to CI13xx
5. CI13xx plays "WiFi connected" prompt
```

## Cloud Platform Integration

### Tencent TVS (Default)

The TVS integration is in `app/cias_aiot_wifi/cias_cloud/cloud_tvs/`:

- `tvs_sdk/` -- TVS SDK source
- `tvs_adapter/` -- Adapter layer for LN882H
- `cias_interface/` -- CIAS-specific interface
- `compatible/` -- API compatibility layer
- `third_party/` -- Mongoose HTTP client, net_ping

Key features:
- Voice assistant (upload audio, get TTS response)
- Device control (smart home commands)
- Audio playback (music, news, etc.)

### Tuya

The Tuya integration (`app/cias_aiot_wifi/cias_cloud/cloud_tuya/`) includes:
- Full Tuya IoT SDK
- BLE provisioning (Tuya app)
- IR remote control support (`CIAS_TUYA_IR_CTRL_ENABLE`)
- Product key-based configuration

### Hisense

The Hisense integration (`app/cias_aiot_wifi/cias_cloud/cloud_iot_hisense/`) includes:
- Hisense cloud protocol
- BLE message handling (`hisense_ble_message_deal.c`)
- WiFi message handling (`hisense_wifi_message_deal.c`)
- Ultra-low-power sleep mode (`usr_ultra_sleep.c`)

## Application Code Structure

### User Application (`app/usr_ln/usr_app.c`)

The default user application entry point. This is where you add custom logic:

```c pseudocode
// In usr_app.c
// This is an application template -- usr_app_init is your custom entry point.
void usr_app_init(void)
{
    // Initialize custom hardware
    // Register callbacks
    // Start custom tasks
}
```

### BLE Application (`app/ble_usr_ln/`)

BLE configuration and pairing:

- `usr_ble_app.c` -- BLE app entry
- `app_callback/ln_gatt_callback.c` -- GATT callbacks
- `app_callback/ln_gap_callback.c` -- GAP callbacks

### Message Handling (`app/cias_aiot_wifi/cias_msg_handle/`)

- `cias_communication.c` -- SDIO communication protocol with CI13xx
- `cias_slave_message_handle.c` -- Process messages from CI13xx

## Media Playback

The LN882H handles media playback via `app/cias_aiot_wifi/cias_media/cias_media.c`:

- Cloud TTS audio playback (received as MP3/data, forwarded to CI13xx)
- Local prompt playback (sent to CI13xx for output)
- Volume control

The CI13xx has the actual audio DAC and speaker driver. The LN882H sends audio data to CI13xx via SDIO for playback.

## WiFi Configuration

WiFi is managed by the `components/wifi/` library (`wifi_lib_export/wifi.h`). Key APIs:

```c
// WiFi initialization and mode control
int  wifi_init(void);
int  wifi_deinit(void);
int  wifi_sta_start(uint8_t *mac_addr, sta_ps_mode_t ps_mode);
int  wifi_softap_start(wifi_softap_cfg_t *ap_cfg);
int  wifi_stop(void);

wifi_mode_t wifi_current_mode_get(void);

// Station (client) operations
int  wifi_sta_scan(wifi_scan_cfg_t *scan_cfg);
int  wifi_sta_connect(wifi_sta_connect_t *connect, wifi_scan_cfg_t *scan_cfg);
int  wifi_sta_disconnect(void);
int  wifi_get_sta_status(wifi_sta_status_t *status);
int  wifi_get_sta_conn_info(const char **ssid, const uint8_t **bssid);

// MAC address
int  wifi_set_macaddr(wifi_interface_t if_index, const uint8_t *mac_addr);
int  wifi_get_macaddr(wifi_interface_t if_index, uint8_t *mac_addr);
```

Key types defined in `wifi.h`:

```c
typedef struct {
    char       *ssid;       // SSID of target AP
    char       *pwd;        // Password of target AP
    uint8_t    *bssid;      // BSSID of target AP (may be NULL)
    uint8_t    *psk_value;  // PSK for fast connect (may be NULL)
} wifi_sta_connect_t;

typedef struct {
    char               *ssid;          // SSID of softAP
    char               *pwd;           // Password of softAP
    uint8_t            *bssid;         // softAP's own MAC
    wifi_softap_ext_cfg_t ext_cfg;     // Channel, authmode, etc.
} wifi_softap_cfg_t;

typedef enum {
    WIFI_STA_STATUS_STARTUP       = 0,
    WIFI_STA_STATUS_SCANING       = 1,
    WIFI_STA_STATUS_CONNECTING    = 2,
    WIFI_STA_STATUS_CONNECTED     = 3,
    WIFI_STA_STATUS_DISCONNECTING = 4,
    WIFI_STA_STATUS_DISCONNECTED  = 5,
} wifi_sta_status_t;
```

WiFi events are handled via callbacks registered with `wifi_sta_reg_callback()` and `wifi_softap_reg_callback()` in the WiFi port layer (`app/cias_aiot_wifi/cias_wifi_port/`).

## BLE Configuration

BLE is used primarily for WiFi provisioning (BLE config). The BLE stack is in `components/ble/` (pre-compiled library).

BLE config is handled by `app/cias_aiot_wifi/cias_ble_port/`:
- `cias_ble_config.c` -- Default BLE config
- `hisense_ble_config.c` -- Hisense-specific BLE config

## Developing a Combo Application

### 1. Define Your Cloud Commands

Map cloud commands to local actions:

```c pseudocode
// In cias_slave_message_handle.c or your custom handler
// These function names are illustrative -- implement using the SDK's
// SDIO communication layer (cias_communication.c).
void handle_cloud_command(const char *command)
{
    if (strcmp(command, "turn_on") == 0) {
        // Send command to CI13xx via SDIO using cias communication layer
    } else if (strcmp(command, "set_volume") == 0) {
        // ...
    }
}
```

### 2. Handle ASR Results from CI13xx

```c pseudocode
// In cias_slave_message_handle.c
// cias_cloud_report_event and execute_local_action are illustrative
// names -- implement using your cloud platform's API and local logic.
void handle_asr_result(uint32_t cmd_id)
{
    // Forward to cloud if needed
    // cias_cloud_report_event("voice_command", cmd_id);

    // Or execute locally
    // execute_local_action(cmd_id);
}
```

### 3. Configure WiFi via BLE

The BLE config flow is automatic when `CIAS_BLE_CONFIG_ENABLE=1`. The phone app sends WiFi credentials via BLE, and the LN882H configures WiFi.

### 4. Enable OTA (Optional)

```cmake
set(CIAS_AIOT_WIFI_OTA_ENABLE  1)  # WiFi chip OTA
set(CIAS_AIOT_AUDIO_OTA_ENABLE 1)  # CI13xx audio chip OTA
```

See `recipes/ota.md` for OTA implementation details.

## Debugging

### UART Log

The LN882H outputs logs via UART. Configure in `bsp/serial_hw.c`:

```c
// Default log UART is UART0
// Check proj_config.h for UART configuration
```

### Network Debugging

- AT commands: `components/ln_at_cmd/` provides AT command interface for WiFi/BLE debugging
- Use `CIAS_LAN_NETWORK_ENABLE=1` for LAN testing mode
- Ping: `components/net/ping/` for network connectivity testing
- iperf: `components/net/iperf/` for bandwidth testing

### CI13xx Debugging

The CI13xx has its own UART log. Check the CI13xx firmware documentation for log output configuration. SDIO communication can be monitored on both sides.

## Memory Management

The LN882H has 295 KB RAM0 shared between:
- Application code and data
- WiFi library buffers (significant: ~80-100 KB)
- BLE stack
- LwIP stack
- System heap

Monitor heap:
```c
size_t free_heap = xPortGetFreeHeapSize();
```

If heap is low:
- Disable unused cloud platforms
- Reduce WiFi buffer count
- Disable Speex compression if not needed
