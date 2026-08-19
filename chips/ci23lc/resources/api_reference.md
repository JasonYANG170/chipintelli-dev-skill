# CI23LC API Reference

> **SDK**: `CI23LC_SDK_BLE_V1.3.13` (latest)
> **Chips**: CI2312, CI23242
> **Architecture**: RISC-V Nuclei N300 (rv32imafc), ILP32F
> **Driver**: `ci13lc_chip_driver` (same as CI13LC)

The CI23LC is architecturally CI13LC + BLE. All peripheral driver APIs (GPIO, UART, I2C, SPI, DMA, PWM, Timer, IIS, Codec, SPIFlash, DPMU, SCU, IWDG, PVDC) are **identical** to CI13LC. See `chips/ci13lc/resources/api_reference.md` for those APIs. This document covers the **BLE-specific APIs** unique to CI23LC, found in `components/ci_ble/` and `app_ble/`.

---

## 1. Chip Driver APIs (Same as CI13LC)

CI23LC uses `driver/ci13lc_chip_driver/` -- the same driver package as CI13LC. The following APIs are identical to CI13LC and are **not duplicated** here. Refer to `chips/ci13lc/resources/api_reference.md`:

| Module | Header | Key APIs |
|--------|--------|----------|
| GPIO | `ci13lc_gpio.h` | `gpio_set_input_mode`, `gpio_set_output_mode`, `gpio_set_output_high_level`, `gpio_set_output_low_level`, `gpio_irq_trigger_config`, `gpio_irq_unmask`, `registe_gpio_callback` |
| UART | `ci13lc_uart.h` | `UARTDMAConfig`, `UART_SendData`, `UART_ReceiveData` |
| I2C | `ci13lc_iic.h` | `I2cMasterInit`, `I2cMasterWrite`, `I2cMasterRead` |
| DMA | `ci13lc_dma.c` | `dma_init`, `dma_channel_enable`, `dma_transfer_config` |
| PWM | `ci13lc_pwm.h` | `pwm_init`, `pwm_set_duty`, `pwm_enable` |
| Timer | `ci13lc_timer.h` | `timer_init`, `timer_enable`, `timer_get_current_cnt` |
| IIS | `ci13lc_iis.h` | `ci_iis_init`, `ci_iis_start` |
| Codec | `ci13lc_codec.h` | `audio_in_codec_registe`, `codec_init` |
| SPIFlash | `ci13lc_spiflash.h` | `ci_flash_read`, `ci_flash_write`, `ci_flash_erase_sector`, `post_read_flash` |
| DPMU | `ci13lc_dpmu.h` | `dpmu_software_reset_system_config`, `dpmu_iwdg_reset_system_config` |
| SCU | `ci13lc_scu.h` | `scu_set_device_gate`, `scu_set_device_reset`, `scu_set_device_reset_release` |
| IWDG | `ci13lc_iwdg.h` | `iwdg_init`, `iwdg_open`, `iwdg_config_reset` |
| Low Power | `ci13lc_lowpower.h` | `lowpower_init`, `lowpower_enter` |

**Key difference from CI13LC**: The CI23LC `user_config.h` uses `CI_CHIP_TYPE` set to `23242` or `23162` (instead of `1312`, `13242`, etc.). Board files differ:
- CI23242: `CI-G24XGS02J-V10.h`
- CI23162: `CI-G16XGS02J-V10.h`

---

## 2. BLE Stack APIs (`components/ci_ble/`)

The BLE subsystem communicates with an external/internal BLE module over UART. The BLE module runs its own firmware (patch-loaded at boot). The MCU sends commands and receives events via a UART protocol.

### 2.1 BLE Initialization Configuration

**Header**: `components/ci_ble/ble_main.h`

```c
typedef struct {
    void (*ble_send_data_callback)(uint8_t *buf, uint16_t len, uint16_t uuid);
    void (*ble_recv_data_callback)(uint8_t *recv_data, uint8_t len);
    void (*ble_recv_adv_data_callback)(uint8_t *recv_data, uint8_t len);
    void (*ble_connected_callback)(void);
    void (*ble_disconnected_callback)(void);
    void (*ble_adv_data_init)(uint8_t *adv_data, char *adv_name);
} BleInitCfg_t;
```

**Global instance**: `extern BleInitCfg_t gBleInitCfg;`

Set callbacks before calling `ble_main_task`:
```c
gBleInitCfg.ble_send_data_callback    = ble_send_payload;
gBleInitCfg.ble_recv_data_callback    = ci_ble_recv_data_handle;
gBleInitCfg.ble_recv_adv_data_callback = ci_ble_recv_adv_data_handle;
gBleInitCfg.ble_connected_callback    = ci_ble_connected_handle;
gBleInitCfg.ble_disconnected_callback = ci_ble_disconnect_handle;
gBleInitCfg.ble_adv_data_init         = ci_ble_adv_data_init;
```

### 2.2 BLE Communication API

**Header**: `components/ci_ble/ble_communicate.h`

#### Core Functions

```c
// Send payload data to connected phone via specific UUID characteristic
bool ble_send_payload(uint8_t *buf, uint16_t len, uint16_t uuid);

// Send a raw command packet to the BLE module
bool ble_send_packet(uint8_t *buf, uint16_t len, uint8_t req_cmd);

// Download BLE firmware patch to the BLE module via UART
bool ble_patch_process(void);

// Create the BLE port mutex for thread-safe UART access
bool ble_port_mutex_create(void);

// Initialize the BLE UART hardware port
int ble_port_protocol_hw_init(UART_BaudRate baud);

// Send a received BLE message to the BLE receiver queue (ISR-safe)
void ble_send_recv_msg(ble_msg_data_t *msg, BaseType_t *xHigherPriorityTaskWoken);

// BLE main task entry point (created via xTaskCreate)
void ble_main_task(void);

// Encrypt/decrypt BLE data (XOR-based, for CIAS protocol)
void cias_crypto_data(uint8_t *pack_data, uint8_t len);
```

#### BLE Module Configuration Functions (in `ble_main.c`)

```c
bool ble_mac_set(uint8_t *mac);                        // Set BLE MAC address (6 bytes)
bool ble_adv_name_set(uint8_t *adv_name, uint8_t len); // Set advertising name
bool ble_adv_data_set(uint8_t *adv_data, uint8_t adv_len);          // Set advertising data
bool ble_adv_rsp_data_set(uint8_t *adv_rsp_data, uint8_t adv_rsp_len); // Set scan response data
bool ble_adv_status_set(ble_adv_status_t adv_status, ble_adv_channel_t adv_channel); // Enable/disable advertising
bool ble_adv_para_set(uint16_t data);                  // Set advertising interval (units of 0.625ms)
bool ble_adv_scan_status_set(ble_adv_scan_status_t status, ble_adv_scan_channel_t channel, uint8_t scan_len);
bool ble_adv_scan_efficiency_set(ble_adv_scan_efficiency_t efficiency);
bool ble_tx_power_set(ble_tx_power_t power);           // Set TX power
bool ble_service_delete(void);                         // Delete all non-system services
bool ble_service_add(uint16_t uuid);                   // Add a custom service
bool ble_characteris_add(ble_characteris_t *characteris); // Add a characteristic
bool ble_service_set(ble_service_t *ble_service_config);  // Configure service + characteristics
uint8_t get_character_handle(uint16_t uuid);           // Get handle for a UUID
bool ble_disconnect(void);                             // Disconnect current connection
bool ble_status_request(void);                         // Query BLE connection status
bool ble_version_req(void);                            // Query BLE module firmware version
bool ble_reset_software(void);                         // Software reset BLE module
bool ble_uart_baud_set(uint8_t *baud);                 // Set BLE UART baud rate
bool ble_uart_control_flow_set(void);                  // Set UART flow control
bool ble_conn_updata_set(void);                        // Update connection parameters
bool ble_xtal_set(uint8_t xtal);                       // Set crystal frequency offset
bool ble_frequency_band_set(uint8_t data);             // Set specific carrier frequency
void ble_mac_get(uint8_t *mac_id);                     // Get MAC from flash unique ID
void dpmu_software_reset_system_config_ble(void);      // Reset BLE module + system
```

#### Pairing Functions (conditional on `BLE_PAIRING_ENBLE`)

```c
bool ble_set_pairing(ble_pairing_mode_t mode);         // Set pairing mode
bool ble_start_pairing(void);                           // Start pairing process
bool ble_set_nvram(uint8_t *nv_data);                   // Set pairing NV data
void ble_pairing_data_init(void);                       // Load pairing keys from flash
void ble_pairing_data_write(uint8_t *pairing_data);     // Save pairing keys to flash
```

#### Heartbeat Timer

```c
// Called periodically (10s default) to check BLE status
void ble_heart_timer_callback(void);
```

### 2.3 BLE Types and Enums

**Header**: `components/ci_ble/ble_communicate.h`

```c
// BLE connection status
typedef enum {
    BLE_STATUS_DISCONNECT = 0x04,
    BLE_STATUS_CONNECT    = 0x20,
} ble_status_t;

// Advertising status
typedef enum {
    ADV_OFF = 0x00,
    ADV_ON  = 0x01,
} ble_adv_status_t;

// Advertising channels
typedef enum {
    ADV_CHAN_ALL = 0x00,  // All channels rotate
    ADV_CHAN_0   = 0x37,  // 2402 MHz
    ADV_CHAN_1   = 0x38,  // 2426 MHz
    ADV_CHAN_2   = 0x39,  // 2480 MHz
} ble_adv_channel_t;

// TX power levels
typedef enum {
    BLE_POWER_0dB  = 0x00,  // ~30m range
    BLE_POWER_3dB  = 0x01,  // ~40-50m range
    BLE_POWER_5dB  = 0x02,  // ~60-70m range
    BLE_POWER_n3dB = 0x03,  // -3 dB
    BLE_POWER_n5dB = 0x04,  // -5 dB
} ble_tx_power_t;

// Pairing modes
typedef enum {
    BLE_PAIRING_NONE                   = 0x00,
    BLE_PAIRING_JUSTWORK               = 0x01,
    BLE_PAIRING_PASSKEY                = 0x02,
    BLE_PAIRING_SECURE_CONNECT_JUSTWORK    = 0x81,
    BLE_PAIRING_SECURE_CONNECT_NUMERIC     = 0x82,
    BLE_PAIRING_SECURE_CONNECT_PASSKEY     = 0x83,
} ble_pairing_mode_t;

// Characteristic attributes
typedef enum {
    BLE_ATTRIBUTE_BROADCAST = 0x01,
    BLE_ATTRIBUTE_READ      = 0x02,
    BLE_ATTRIBUTE_WRITE_NRQ = 0x04,
    BLE_ATTRIBUTE_WRITE     = 0x08,
    BLE_ATTRIBUTE_NOTIFY    = 0x10,
    BLE_ATTRIBUTE_INDICATE  = 0x20,
} ble_attribute_t;

// Service/characteristic configuration structs
typedef struct {
    uint16_t uuid;
    ble_attribute_t attribute;
    uint8_t handle;
} ble_characteris_t;

typedef struct {
    uint16_t uuid;
    uint8_t characteris_number;
    ble_characteris_t characteris[];
} ble_service_t;

// BLE message packet (UART protocol frame)
#pragma pack(1)
typedef struct {
    uint8_t type;       // Packet type (CMD/EVENT/PATCH)
    uint8_t opcode;     // Command/event opcode
    uint8_t length;     // Data length
    uint8_t msg_data[BLE_MSG_DATA_MAX_SIZE]; // 240 bytes max
} ble_msg_data_t;
#pragma pack()
```

### 2.4 BLE Command/Event Opcodes

```
// Commands (MCU -> BLE module)
BLE_CMD_SET_ADDR            0x01  // Set BLE address
BLE_CMD_SET_NAME            0x04  // Set BLE name
BLE_CMD_SEND_DATA           0x09  // Send BLE data
BLE_CMD_STATUS_REQ          0x0B  // Query BLE status
BLE_CMD_SET_UART_FLOW      0x0E  // Set UART flow control
BLE_CMD_SET_UART_BAUD      0x0F  // Set UART baud rate
BLE_CMD_VERSION_REQUEST    0x10  // Query firmware version
BLE_CMD_DISCONNECT         0x12  // Disconnect BLE
BLE_CMD_SET_NVRAM          0x26  // Set pairing NV data
BLE_CMD_SET_PAIRING        0x33  // Set pairing mode
BLE_CMD_ADV_DATA           0x34  // Set advertising data
BLE_CMD_ADV_RSP_DATA       0x35  // Set scan response data
BLE_CMD_CONN_UPDATE        0x36  // Update connection parameters
BLE_CMD_ADV_PARA           0x37  // Update advertising parameters
BLE_CMD_START_PAIRING      0x38  // Start pairing
BLE_CMD_SET_TX_POWER       0x42  // Set TX power
BLE_CMD_RESET_CHIP_REQ     0x51  // Software reset BLE chip
BLE_CMD_ADV_SCAN_STATUS    0x67  // Set advertising scan status
BLE_CMD_ADV_SCAN_EFFICIENCY 0x68 // Set scan efficiency
BLE_CMD_ADV_STATUS         0x69  // Set advertising status
BLE_CMD_DELETE_SERVICE     0x76  // Delete non-system services
BLE_CMD_ADD_SERVICE        0x77  // Add custom service
BLE_CMD_ADD_CHARACTERISTIC 0x78  // Add custom characteristic

// Events (BLE module -> MCU)
BLE_EVENT_CONN_REP          0x02  // BLE connection established
BLE_EVENT_DIS_REP           0x05  // BLE disconnected
BLE_EVENT_CMD_RES           0x06  // CMD completed
BLE_EVENT_DATA_REP          0x08  // BLE data received
BLE_EVENT_STACK_OK          0x09  // Module ready
BLE_EVENT_STATUS_REP        0x0A  // Status query response
BLE_EVENT_NVRAM_REP         0x0D  // NV storage event
BLE_EVENT_PATCH_ACK         0x0E  // Patch firmware ACK
BLE_EVENT_PAIRING_STATE     0x14  // Pairing state
BLE_EVENT_ENCRYPTION_STATE  0x15  // Encryption state
BLE_EVENT_UUID_HANDLE       0x29  // UUID -> handle mapping
BLE_EVENT_ADV_REP           0x2A  // Advertising data received (scanner)
```

---

## 3. BLE Application APIs (`app_ble/`)

### 3.1 BLE Message Handling (`app_ble/demo/cias_ble_msg_deal.h`)

```c
// Device type enum
typedef enum {
    IR_DEV = 1, AIRCONDITION_DEV, RGB_DEV, AUDIO_DEV,
    TEABAR_DEV, FAN_DEV, HEATTABLE_DEV, WARMER_DEV, WATERHEATED_DEV,
} dev_type_t;

// Data type (within BLE protocol V1)
typedef enum {
    ATTRIBUTE_SETUP = 1,  // Property setting
    EVENT_REPORT,         // Event report
    STATE_QUERY,          // State query
    STATE_RECOVERY,       // State recovery
} data_type_t;

// BLE protocol V1 message structure
#pragma pack(1)
typedef struct {
    uint8_t  pkg_header[2];    // 0xA5, 0x5A
    uint8_t  protocol_ver;     // 0x01
    uint8_t  manufacturer_id;
    uint8_t  dev_type;         // dev_type_t
    uint8_t  dev_number;
    uint8_t  data_type:4;      // data_type_t
    uint8_t  function_type:4;
    uint8_t  function_id;
    uint16_t data_len;         // Little-endian
    uint8_t  data[8];
} ble_msg_V1_t;
#pragma pack()

// BLE command structure (short commands)
#pragma pack(1)
typedef struct {
    uint8_t pkg_header[2];  // 0xA5, 0x5B
    uint8_t ble_cmd;
} ble_cmd_t;
#pragma pack()

// Per-device init/callback/query/report functions
void fan_init(void);
void fan_callback(ble_msg_V1_t msg);
void fan_query(ble_msg_V1_t msg);
uint8_t fan_report(uint32_t cmd_id);
// ... same pattern for: warmer, aircondition, heattable, tbm, rgb, waterheated

// BLE receive task (processes messages from ble_msg_queue)
void ci_ble_recv_task(void);

// Handle ASR result -> send BLE state report to phone
void deal_ble_send_msg(cmd_handle_t cmd_handle);

// Process received BLE data (from phone, CIAS protocol)
void ci_ble_recv_data_handle(uint8_t *recv_data, uint8_t len);

// Process received BLE data (custom protocol, user-implemented)
void custom_ble_recv_data_handle(uint8_t *recv_data, uint8_t len);

// Handle 2.4G remote control advertising data
bool ci_ble_recv_adv_data_handle(unsigned char *buf);
bool covent_data_adv_to_ble(unsigned char *rf_send_data, unsigned char *buf);

// Connection event handlers
void ci_ble_connected_handle(void);
void ci_ble_disconnect_handle(void);

// Advertising data initialization (CIAS protocol format)
void ci_ble_adv_data_init(uint8_t *adv_data, char *adv_name);

// App page customization (mini-program UI)
void app_title_set(void);
void app_background_set(void);

// Initialize device state (calls the active demo's init function)
void dev_state_init(void);
```

### 3.2 Advertising Message Handling (`app_ble/ble_adv_msg_deal.h`)

```c
// BLE message data for system message queue
typedef struct {
    uint8_t  play_type;     // 1: command ID playback; 2: command string playback
    uint32_t cmd_id;
    uint32_t select_index;
    char    *cmd_str;
} sys_msg_ble_data_t;
```

### 3.3 Demo Configuration (`app_ble/demo/cias_demo_config.h`)

```c
// Select the active demo device
#define DEV_DRIVER_EN_ID  DEV_FAN_MAIN_ID  // Change to select demo

// Device main IDs
#define DEV_IR_CONTROL_MAIN_ID      0x01  // IR remote
#define DEV_AIRCONDITION_MAIN_ID    0x02  // Air conditioner
#define DEV_LIGHT_CONTROL_MAIN_ID   0x03  // Light control
#define DEV_SOUND_MAIN_ID           0x04  // Audio device
#define DEV_TEA_BAR_MAIN_ID         0x05  // Tea bar machine
#define DEV_FAN_MAIN_ID             0x06  // Fan
#define DEV_HEATTABLE_MAIN_ID       0x07  // Heating table
#define DEV_WARMER_MAIN_ID          0x08  // Warmer
#define DEV_WATERHEATED_MAIN_ID     0x09  // Water heated blanket

// Light sub-device IDs
#define DEV_LIGHT_CONTROL_RGB_SUB_ID  0x07  // RGB light

// Protocol version (communication with mini-program)
#define CIAS_PROTOCOL_VER  1

// App page configuration
#define APP_PAGE_TITLE      "语音智能设备"  // Max 9 Chinese chars or 15 alphanumeric
#define APP_PAGE_BACKGROUND "1"             // 1-5 background options
```

---

## 4. BLE Configuration (`components/ci_ble/ble_param_config.h`)

Key configuration constants (do not modify unless noted):

```c
#define BLE_PATCH_CRC                    0x84EE     // BLE firmware CRC (do not modify)
#define BLE_FIRMWARE_ID                  7000       // BLE firmware file ID in user_file
#define BLE_ADV_NAME_MAX_LEN             29         // Max advertising name length
#define BLE_MSG_DATA_MAX_SIZE            240        // Max BLE message data size
#define BLE_RCV_MSG_QUEUE_SIZE           5          // BLE receive queue depth
#define BLE_ADV_LEN                      42         // Advertising packet length
#define BLE_DEFAULT_BAUDRATE             115200     // Initial UART baud rate
#define BLE_PROTOCOL_NUMBER              HAL_UART1_BASE  // BLE UART port
#define BLE_PROTOCOL_IRQ_NUMBER          UART1_IRQn      // BLE UART IRQ
#define BLE_RESET_PIN                    PA6              // BLE reset GPIO pin
#define BLE_RESET_GPIO_PORT              PA
#define BLE_RESET_GPIO_PIN               pin_6
#define DEV_MTU_TYPE                     0x02             // 3.5-gen BLE: long packet support

// Scan (2.4G remote) configuration
#define CIAS_BLE_SCAN_ENABLE             0    // Enable 2.4G remote scan
#define CIAS_BLE_SCAN_REMOTE_CONTROL_LEN 0x16 // Remote data length

// Heartbeat
#define CIAS_BLE_HEART_TIMER_ENABLE      1    // BLE status heartbeat

// Pairing
#define BLE_PAIRING_MODE  BLE_PAIRING_SECURE_CONNECT_JUSTWORK  // When BLE_PAIRING_ENBLE=1
#define PAIRING_DATA_LEN  170
#define FLASH_PAIRING_DATA_ADDR  0x1FE000     // Pairing key flash address
#define NVDATA_ID_PAIRING_DATA   0x70000002   // Pairing key NV ID
```

### User-Configurable BLE Settings (`user_config.h`)

```c
#define USE_BLE_MOUDLE                1           // Enable BLE
#define USE_CI_APPLET_ENABEL          1           // Use Chipintelli mini-program
#define BLE_ADV_NAME_APPEND_FLASH_ID  1           // Append flash ID to name
#define BLE_USER_DEFINE_ADV_NAME_CONTENT "CI_BLE" // Custom name (max 29 bytes, 18 if using CIAS applet)
#define USE_XFI                       _XIF_        // Run in flash (save RAM)

// Service UUIDs (must match mini-program)
#define BLE_UUID_CIAS_SERVICE       0xAE3A
#define BLE_UUID_CIAS_WRITE         0xAE3B  // Phone -> device
#define BLE_UUID_CIAS_NOTIFY        0xAE3C  // Device -> phone
#define BLE_UUID_CIAS_CMD_NOTIFY    0xAE3D  // Command word info
```

---

## 5. ASR and System Message APIs (Same as CI13LC)

The ASR pipeline, audio player, system message handling, and flash control APIs are identical to CI13LC. Key entry points:

```c
// ASR
void asr_process_init(void);
void pause_asr(void);
void resume_asr(void);

// Audio player
void smp_init(void);  // audio_play_init() wrapper
uint8_t vol_set(char vol);
uint8_t vol_get(void);

// System messages
void sys_msg_task_initial(void);
BaseType_t send_sys_msg_inner(void *msg_data, uint32_t msg_len, BaseType_t *xHigherPriorityTaskWoken);
void UserTaskManageProcess(void *p_arg);

// Prompt player
uint32_t prompt_play_by_cmd_string(char *cmd_str, int select_index, play_done_callback_t callback, bool preemptive);
void default_play_done_callback(cmd_handle_t cmd_handle);

// Command info
uint16_t cmd_info_get_command_id(cmd_handle_t cmd_handle);
uint32_t cmd_info_get_semantic_id(cmd_handle_t cmd_handle);
```

### System Message Types (CI23LC adds BLE type)

```c
typedef enum {
    SYS_MSG_TYPE_ASR = 0,
    SYS_MSG_TYPE_CMD_INFO,
    SYS_MSG_TYPE_KEY,
    SYS_MSG_TYPE_COM,
    SYS_MSG_TYPE_AUDIO_IN_STARTED,
    SYS_MSG_TYPE_I2C,
    SYS_MSG_TYPE_BLE,    // <-- CI23LC addition
} sys_msg_type_t;
```

The `SYS_MSG_TYPE_BLE` message carries a `sys_msg_ble_data_t` payload, allowing BLE-received commands to trigger voice prompt playback via the same message queue as ASR results.

---

## 6. BLE Default Service Configuration

The default BLE service is configured in `ble_main.c`:

```c
ble_service_t ble_service_config = {
    .uuid = BLE_UUID_CIAS_SERVICE,           // 0xAE3A
    .characteris_number = 3,
    .characteris[0] = {.uuid = BLE_UUID_CIAS_WRITE,      .attribute = BLE_ATTRIBUTE_WRITE},   // 0xAE3B
    .characteris[1] = {.uuid = BLE_UUID_CIAS_NOTIFY,     .attribute = BLE_ATTRIBUTE_NOTIFY},  // 0xAE3C
    .characteris[2] = {.uuid = BLE_UUID_CIAS_CMD_NOTIFY, .attribute = BLE_ATTRIBUTE_NOTIFY},  // 0xAE3D
};
```

To use a custom service, modify `ble_service_config` and the UUID defines in `user_config.h`. The mini-program must use matching UUIDs.
