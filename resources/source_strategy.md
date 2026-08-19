# Chipintelli Source Strategy

The local SDK collection is large and versioned. Keep it as evidence, but never search it as one undifferentiated corpus when writing code.

## Search Narrowly

- First choose chip family and SDK version.
- Prefer `include`, `driver`, `components`, `projects`, and board config files inside that SDK.
- Search exact symbol names before generic terms like `asr`, `audio`, `uart`, or `ble`.
- Avoid relying on generated, packed, binary, third-party, or toolchain files for API behavior.

## SDK Selection

For an existing project, stay on its SDK unless the user asks to migrate. For a new project, choose the newest SDK that explicitly supports:

- Exact chip and board.
- Offline ASR or online/cloud voice mode.
- BLE/Wi-Fi/IR/OTA/peripheral requirements.
- Required voice model generation and pack tool flow.

## Common Risk Areas

- CI1312 belongs to the CI13LC routing, not CI130X-only recipes unless the selected unified SDK documents support.
- CI230X is ARM Cortex-M4/LN882H based, so do not reuse RISC-V Nuclei startup or interrupt assumptions.
- BLE combo SDKs can share ASR message queues but have different app folders and config files.
