# BLE + Voice Combo on CI13XX

> Applies to: CI13XX unified SDK targets documented by the selected SDK; confirm exact chip, board, and voice/connectivity feature set before coding.
> Evidence: `chips/ci13xx/resources/`, closest SDK `projects/` example, and this recipe path `chips/ci13xx/recipes/ble_voice.md`.
> Validation: draft metadata added from repository routing; verify APIs, `user_config.h`, `source_file.prj`, and pack-tool requirements against the selected SDK.

## Overview

The CI13XX LLM AIoT SDK includes BLE support via the `ci_ble` component, enabling voice control with BLE connectivity.

## SDK

Use `CI13XX_SDK_LLM_AIOT_2.1.2` with the `ci_ble` component.

## Component

`components/ci_ble/` contains the BLE stack for CI13XX.

## When to Use CI13XX vs CI23LC for BLE

| Feature | CI13XX (ci_ble) | CI23LC (app_ble) |
|---|---|---|
| BLE stack | Basic | Full (app_ble demos) |
| BLE demos | No | Yes (fan, AC, heater, RGB, etc.) |
| CWSL | Yes | Yes |
| NL ASR | Yes | Yes |
| LLM/TTS/VPR | Yes | No |
| Recommended for | Custom BLE + AI | Product BLE demos |

## Configuration

Enable BLE in `user_config.h`:
```c
#define USE_BLE_MODULE             1
```

## Message Flow

BLE messages and ASR messages both route through `sys_msg_queue`:

```c
case MSG_BLE_RECEIVED:
{
    uint8_t *data = msg->data.ble_data;
    uint16_t len = msg->data.ble_len;
    // Handle BLE data
    break;
}
```
