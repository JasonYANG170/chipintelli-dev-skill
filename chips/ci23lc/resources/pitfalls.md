# CI23LC Pitfalls

> **SDK**: `CI23LC_SDK_BLE_V1.3.13`
> **Chips**: CI2312, CI23242

CI23LC shares all CI13LC pitfalls (SCU clock gating, OS-dependent driver timing, chip type config, etc.). See `chips/ci13lc/resources/pitfalls.md` for the full CI13LC pitfall list. This document covers **BLE-specific** pitfalls unique to CI23LC.

---

## BLE + ASR Coexistence

### 1. UART1 Conflict Between BLE and Log/Protocol

**Problem**: The BLE module communicates over `HAL_UART1_BASE` (`BLE_PROTOCOL_NUMBER`). `CONFIG_CI_LOG_UART` and `UART_PROTOCOL_NUMBER` must NOT use UART1.

**Fix**: In `user_config.h`:
```c
#define CONFIG_CI_LOG_UART       HAL_UART0_BASE  // NOT HAL_UART1_BASE
#define UART_PROTOCOL_NUMBER     HAL_UART2_BASE  // NOT HAL_UART1_BASE
```

The SDK asserts this: `CI_ASSERT(0,"Log uart and protocol uart confict!\n")`.

### 2. BLE Task Stack Size Insufficient

**Problem**: The `ble_main_task` and `ci_ble_recv_task` are each created with 480 words (1920 bytes) of stack. If you add heavy processing or deep call chains in BLE callbacks, the stack overflows.

**Fix**: Increase stack size in `xTaskCreate` calls in `main.c` `task_init()`:
```c
xTaskCreate(ble_main_task, "ble_main_task", 600, NULL, 4, &ble_task_handle);
xTaskCreate(ci_ble_recv_task, "ci_ble_recv_task", 600, NULL, 4, NULL);
```
Monitor with `usStackHighWaterMark` in the task status printout.

### 3. BLE Firmware Patch Download Failure

**Problem**: `ble_patch_process()` reads the BLE firmware from flash `user_file` (ID 7000). If the firmware file is missing or corrupted, BLE fails to initialize and the system hangs in a retry loop.

**Fix**: 
- Ensure `firmware/user_file/` contains the BLE firmware file (ID 7000)
- Verify `BLE_PATCH_CRC` (0x84EE) matches the firmware CRC
- The boot sequence retries: `while (!ble_patch_process()) { ble_reset_hardware(); }`
- If CRC fails, the log shows: `"ble firmware crc erro! %x"`

### 4. BLE UART Baud Rate Switch Race Condition

**Problem**: After patch download at 115200 baud, the code switches to 921600. If `ble_uart_baud_set("921600")` fails, it resets and retries from the beginning.

**Fix**: This is handled in `ble_main_task`:
```c
if (ble_uart_baud_set("921600"))
    ble_port_protocol_hw_init(UART_BaudRate921600);
else {
    ble_reset_software();
    goto ble_load_start;  // Full restart
}
```
Do not bypass this retry logic. If the baud switch consistently fails, check UART signal integrity.

### 5. BLE Heart Timer False Reset

**Problem**: The heartbeat timer (10s) queries BLE status. If the BLE module doesn't respond (e.g., during heavy ASR processing), the system resets the BLE module unnecessarily.

**Fix**: The heartbeat timer is reset whenever BLE data is received:
```c
xTimerReset(ble_heart_timer, 10);  // Reset on data receive
```
If you have long-running BLE operations, ensure they don't block the BLE receive path for >10s. Adjust `CIAS_BLE_HEART_TIMER_ENABLE` to 0 to disable if needed.

---

## Memory Sharing and Heap

### 6. BLE Reduces Available System Heap

**Problem**: The BLE subsystem consumes significant RAM:
- `ble_main_task`: 480 words stack
- `ci_ble_recv_task`: 480 words stack  
- `ble_recever_queue`: 5 * sizeof(ble_msg_data_t) = 5 * 243 = 1215 bytes
- `ble_msg_queue` (in cias_ble_msg_deal.c): 10 * sizeof(ble_msg_V1_t) = 10 * 16 = 160 bytes
- BLE UART RX/TX buffers
- `adv_ind[BLE_ADV_LEN]`: 42 bytes
- Various `ble_set_data[BLE_SET_LEN]`: 40 bytes

Total BLE RAM overhead is approximately 5-6 KB. If the system heap is tight, ASR model loading or player initialization may fail.

**Fix**: 
- Check heap with `xPortGetFreeHeapSize()` after BLE init
- Reduce `BLE_RCV_MSG_QUEUE_SIZE` if queue depth is excessive
- Use `USE_XFI _XIF_` to run BLE functions from flash (saves RAM, slower execution)
- Consider `ASR_FE_REDUCE_MEM 1` to save ~15KB in ASR front-end

### 7. `msg_data.cmd_str` Memory Leak in Fan Demo

**Problem**: In `cias_fan_msg_deal.c`, `fan_callback()` allocates 128 bytes for `msg_data.cmd_str` via `pvPortMalloc` but never frees it. This is intentional (reused across calls), but if you copy this pattern, ensure the buffer is allocated once and reused.

**Fix**: Follow the fan demo pattern -- allocate once, reuse:
```c
static sys_msg_ble_data_t msg_data = {.cmd_str = NULL};
if (msg_data.cmd_str == NULL) {
    msg_data.cmd_str = pvPortMalloc(128 * sizeof(char));
}
```

### 8. BLE Advertising Name Length with Flash ID Append

**Problem**: When `BLE_ADV_NAME_APPEND_FLASH_ID=1`, 5 bytes are appended to the name (`_XXYY`). If using the CIAS mini-program (`USE_CI_APPLET_ENABEL=1`), the total name must be <= 18 bytes. A 14-byte custom name + 5-byte flash ID = 19 bytes > 18, causing a silent return.

**Fix**: 
- With CIAS applet: `BLE_USER_DEFINE_ADV_NAME_CONTENT` must be <= 13 bytes
- Without CIAS applet: up to 24 bytes (29 - 5)
- The SDK logs: `"set adv name too long, max 18 bytes!"`

### 9. BLE Pairing Data Flash Address Conflict

**Problem**: `FLASH_PAIRING_DATA_ADDR` is `0x1FE000`. If your ASR model or voice data extends into this region, pairing data will be corrupted.

**Fix**: Verify flash layout. The pairing data occupies 170 bytes at 0x1FE000. If you have large voice models, ensure they don't overlap. Only relevant when `BLE_PAIRING_ENBLE=1`.

---

## BLE Protocol and Application

### 10. CIAS Protocol CRC Verification

**Problem**: The CIAS protocol uses CRC16-CCITT for data integrity. Received data that fails CRC is silently dropped. If your mini-program sends data without proper CRC, the device appears to ignore it.

**Fix**: The `ci_ble_recv_data_handle` function:
1. Decrypts data with `cias_crypto_data()`
2. Verifies CRC: `crc16_ccitt(0, recv_data, len - 2)` must match last 2 bytes
3. If CRC fails: logs `"crc erro"` and returns

Ensure your app computes CRC16-CCITT correctly and appends it as 2 bytes (little-endian).

### 11. BLE Message Routing Through sys_msg_queue

**Problem**: BLE commands from the phone must trigger voice prompts. The routing goes through `sys_msg_queue` with `SYS_MSG_TYPE_BLE`. If the system message task is blocked or the queue is full, BLE commands appear unresponsive.

**Flow**: 
```
Phone -> BLE -> ci_ble_recv_data_handle -> ble_msg_queue -> ci_ble_recv_task
  -> fan_callback -> send_sys_msg_inner(SYS_MSG_TYPE_BLE) -> sys_msg_queue
  -> UserTaskManageProcess -> play voice prompt
```

**Fix**: Ensure `UserTaskManageProcess` has adequate priority (4) and stack. The `sys_msg_t` struct has `SYS_MAX_MSG_LEN=32` bytes for data -- `sys_msg_ble_data_t` (with a pointer) fits, but if you add fields, check alignment.

### 12. dev_state_init Must Match DEV_DRIVER_EN_ID

**Problem**: `dev_state_init()` uses compile-time `#if` to call the correct device init function. If `DEV_DRIVER_EN_ID` in `cias_demo_config.h` doesn't match the included source files, the device won't initialize.

**Fix**: `DEV_DRIVER_EN_ID` must be set to one of:
- `DEV_AIRCONDITION_MAIN_ID` (0x02)
- `DEV_LIGHT_CONTROL_MAIN_ID` (0x03) -- uses `rgb_init()`
- `DEV_TEA_BAR_MAIN_ID` (0x05)
- `DEV_FAN_MAIN_ID` (0x06)
- `DEV_HEATTABLE_MAIN_ID` (0x07)
- `DEV_WARMER_MAIN_ID` (0x08)
- `DEV_WATERHEATED_MAIN_ID` (0x09)

All demo source files are compiled regardless (see `source_file.prj`), but only the matching one is called at runtime.

### 13. BLE Scan (2.4G Remote) Requires Specific Channel

**Problem**: When `CIAS_BLE_SCAN_ENABLE=1`, the BLE module scans for 2.4G remote control advertising data. The scan data length (`CIAS_BLE_SCAN_REMOTE_CONTROL_LEN=0x16=22`) must match the remote's payload format.

**Fix**: The remote data format is documented in `ble_adv_scan_status_set`:
- Header: cmd(0x02) + opcode(0x2A) + len(0x27) = 3 bytes
- Head: 2 bytes, MAC: 6 bytes  
- Valid data: from byte[11] to end (31 bytes)
- Set `adv_scan_len` = valid data + 12 = 43 bytes

### 14. BLE Module Reset Pin Configuration

**Problem**: The BLE module reset pin is `PA6` (`BLE_RESET_PIN`). If PA6 is used for another function (board LED, button, etc.), the BLE module won't reset properly.

**Fix**: Check board file pin assignments. `BLE_RESET_PIN`, `BLE_RESET_GPIO_PORT`, `BLE_RESET_GPIO_PIN` in `ble_param_config.h` must not conflict with your board design.

---

## General CI13LC Pitfalls (Still Apply)

These pitfalls from CI13LC apply equally to CI23LC. See `chips/ci13lc/resources/pitfalls.md`:

1. SCU clock gate not enabled before peripheral init
2. Device reset not performed after clock gate
3. OS-dependent drivers (QSPIFlash, DMA, I2C, SPI) called before RTOS start
4. Wrong `CI_CHIP_TYPE` (must be 23242 or 23162 for CI23LC)
5. Board file mismatch
6. Mic mode mismatch (differential vs single-end)
7. Log UART and protocol UART conflict (now also BLE UART)
8. PLL frequency mismatch
9. Flash write without 4KB erase
10. `source_file.prj` not updated when adding BLE source files
11. LTO settings for debug
12. Heap size insufficient (now worse with BLE)
13. Watchdog not fed during BLE operations
