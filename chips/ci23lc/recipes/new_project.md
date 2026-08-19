# Recipe: Creating a New CI23LC BLE+Voice Project

> **SDK**: `CI23LC_SDK_BLE_V1.3.13`
> **Chips**: CI2312, CI23242

CI23LC is CI13LC + BLE. The project structure, build system (Make + Lua), and driver are identical to CI13LC. This recipe covers the BLE-specific additions.

---

## Step 1: Copy the Template

Copy `projects/offline_asr_sample/` to your new project name:

```bash
cd CI23LC_SDK_BLE_V1.3.13/CI23LC_SDK_BLE_V1.3.13
cp -r projects/offline_asr_sample projects/my_ble_device
```

## Step 2: Configure Chip Type

Edit `projects/my_ble_device/src/user_config.h`:

```c
#define CI_CHIP_TYPE  23242  // or 23162

#if (CI_CHIP_TYPE == 23162)
  #define BOARD_CONFIG_FILE  "CI-G16XGS02J-V10.h"
#elif (CI_CHIP_TYPE == 23242)
  #define BOARD_CONFIG_FILE  "CI-G24XGS02J-V10.h"
#endif

#define MAIN_FREQUENCY  210000000  // 210 MHz
```

## Step 3: Configure UART (Avoid UART1 -- BLE uses it)

```c
#define CONFIG_CI_LOG_UART       HAL_UART0_BASE  // Log output
#define MSG_COM_USE_UART_EN      1               // Enable voice module protocol
#define UART_PROTOCOL_NUMBER     HAL_UART2_BASE  // Protocol UART (NOT UART1)
#define UART_PROTOCOL_BAUDRATE   9600
```

**Critical**: UART1 is used internally by BLE (`BLE_PROTOCOL_NUMBER = HAL_UART1_BASE`). Never assign log or protocol to UART1.

## Step 4: Configure BLE

In `user_config.h`:

```c
#define USE_BLE_MOUDLE                      1           // Enable BLE
#define USE_CI_APPLET_ENABEL                1           // Use Chipintelli mini-program
#define BLE_ADV_NAME_APPEND_FLASH_ID        1           // Unique name per device
#define BLE_USER_DEFINE_ADV_NAME_CONTENT   "MyDevice"   // Max 13 bytes with CIAS applet

// Service UUIDs (must match your mini-program)
#define BLE_UUID_CIAS_SERVICE       0xAE3A
#define BLE_UUID_CIAS_WRITE         0xAE3B
#define BLE_UUID_CIAS_NOTIFY        0xAE3C
#define BLE_UUID_CIAS_CMD_NOTIFY    0xAE3D
```

## Step 5: Select Demo Device Type

Edit `app_ble/demo/cias_demo_config.h`:

```c
#define DEV_DRIVER_EN_ID  DEV_FAN_MAIN_ID  // Pick your appliance type

// App page customization
#define APP_PAGE_TITLE      "My Smart Device"
#define APP_PAGE_BACKGROUND "1"
```

Or create a custom device type by adding a new `#elif` block and implementing `mydevice_init()`, `mydevice_callback()`, `mydevice_query()`, `mydevice_report()`.

## Step 6: Configure ASR

```c
#define USE_SEPARATE_WAKEUP_EN  1   // Separate wakeup model
#define DEFAULT_MODEL_GROUP_ID  1   // Start in wakeup mode
#define EXIT_WAKEUP_TIME        15*1000  // 15s exit wakeup timeout

#define AUDIO_PLAYER_ENABLE     1
#define USE_PROMPT_DECODER      1
#define USE_MP3_DECODER         1
#define VOLUME_DEFAULT          5
```

## Step 7: Update source_file.prj

The `source_file.prj` already includes all BLE sources. If you add custom files, append them:

```
// Add your custom device logic
source-file: $(SDK_PATH)/app_ble/demo/cias_mydevice_msg_deal.c
```

Ensure include paths are present:
```
include-path: $(SDK_PATH)/app_ble/demo
include-path: $(SDK_PATH)/app_ble
include-path: $(SDK_PATH)/components/ci_ble
```

## Step 8: Prepare Firmware Files

```
projects/my_ble_device/
  firmware/
    asr/           # ASR model files (from voice AI platform)
    dnn/           # DNN model files
    user_file/     # Must contain BLE firmware (ID 7000)
    voice/         # Voice prompt MP3/WAV files
```

**Critical**: The `user_file/` directory must contain the BLE module firmware file (ID 7000). Without it, `ble_patch_process()` fails and BLE won't initialize.

## Step 9: Build

```bash
export SDK_PATH="$(pwd)"
export PATH="$SDK_PATH/tools/build-tools/bin:$GCC_ROOT/gcc_fix_raissrc/bin:$PATH"

cd projects/my_ble_device/project_file
make -j4
```

## Step 10: Flash

Use `ci-tool-kit.exe` or `code_program.exe` from `tools/`:
1. Run `合成分区bin文件.bat` to generate partition bin
2. Run `打包升级.bat` to package and flash

## Step 11: Verify

After boot, check UART0 log for:
```
Welcome to CI23LC_SDK.
ble firmware download ok!
adv_name = MyDevice_XXYY
```

The BLE module will:
1. Download firmware patch via UART1 at 115200
2. Switch to 921600 baud
3. Set MAC, name, advertising data
4. Start advertising on channel 2 (2480 MHz)
5. Configure CIAS service with 3 characteristics
6. Start heartbeat timer (10s)

## Init Sequence (in main.c task_init)

```c
static void task_init(void *p_arg)
{
    ciss_init();                    // BNPU init (after mailbox sync)
    cm_init();                      // Codec manager init
    audio_in_codec_registe();       // Register audio codec
    set_ssp_registe(...);           // Register signal processing
    asr_process_init();             // ASR engine init
    smp_init();                     // Audio player init

    sys_msg_task_initial();         // System message task
    xTaskCreate(UserTaskManageProcess, ...);  // User task manager

    // BLE initialization
    gBleInitCfg.ble_send_data_callback    = ble_send_payload;
    gBleInitCfg.ble_recv_data_callback    = ci_ble_recv_data_handle;
    gBleInitCfg.ble_recv_adv_data_callback = ci_ble_recv_adv_data_handle;
    gBleInitCfg.ble_connected_callback    = ci_ble_connected_handle;
    gBleInitCfg.ble_disconnected_callback = ci_ble_disconnect_handle;
    gBleInitCfg.ble_adv_data_init         = ci_ble_adv_data_init;

    xTaskCreate(ble_main_task, "ble_main_task", 480, NULL, 4, &ble_task_handle);
    xTaskCreate(ci_ble_recv_task, "ci_ble_recv_task", 480, NULL, 4, NULL);

    vTaskDelete(NULL);
}
```
