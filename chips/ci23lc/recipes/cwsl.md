# CWSL on CI23LC

## Overview

CI23LC supports Command Word Self-Learning (CWSL), the same as CI13LC since both use the `ci13lc_chip_driver` and the same `ci_cwsl` component.

## SDK

Use `CI23LC_SDK_BLE_V1.3.13` with `cwsl_sample` project.

## API

See `chips/ci13lc/recipes/cwsl.md` for the complete CWSL API reference. The CI23LC SDK uses the same `cwsl_manage.h` header from `components/ci_cwsl/`.

## Key APIs (from `components/ci_cwsl/cwsl_manage.h`)

```c
void cwsl_init(void);
cwsl_manage_status_t cwsl_get_status(void);
int cwsl_reg_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type);
int cwsl_reg_record_start(void);
int cwsl_reg_record_stop(void);
int cwsl_exit_reg_word(void);
int cwsl_manage_reset(void);
int cwsl_delete_word(uint32_t cmd_id, uint16_t group_id, cwsl_word_type_t word_type);
int cwsl_recognize_start(cwsl_word_type_t word_type);
int cwsl_recognize_stop(void);
```

## BLE Integration

On CI23LC, CWSL and BLE operate concurrently. CWSL results route through the same `sys_msg_queue` as BLE messages. See `chips/ci23lc/recipes/ble_voice.md` for the combined message handling pattern.
