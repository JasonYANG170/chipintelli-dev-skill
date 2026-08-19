# Chipintelli Development Workflow

Use this reference when writing, porting, or reviewing Chipintelli voice MCU firmware.

## Evidence Order

1. User project files: `user_config.h`, `source_file.prj`, board config, flash layout, and existing application code.
2. Exact SDK headers/source for the selected chip family and version.
3. Closest SDK `projects/` example.
4. Family `resources/` and recipes.
5. Generic FreeRTOS, RISC-V, or ARM knowledge only after SDK symbols are verified.

## Code Generation Rules

- Treat `user_config.h`, ASR/audio model files, voice assets, and firmware packing as part of the implementation.
- Add source files through `source_file.prj` unless the selected SDK uses a different documented build flow.
- Verify SCU clock gates, reset release, pin mux, interrupt names, and task/message queues against the selected SDK.
- Keep CI130X, CI13LC, CI13XX, CI230X, and CI23LC SDKs separate unless a shared SDK explicitly covers the chip and feature.
- For ASR features, show where messages enter `sys_msg_queue` and where application handling belongs.

## Minimum Answer Shape

When generating firmware changes, include:

- Target chip/family and SDK version assumed.
- Example or source files used as evidence.
- Required `user_config.h`, `source_file.prj`, packing, model, or flash-layout changes.
- Runtime path for messages/tasks/callbacks.
- Hardware validation notes for audio, wake word, UART, BLE/Wi-Fi, or IR behavior.
