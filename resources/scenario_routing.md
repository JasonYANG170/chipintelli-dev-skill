# Chipintelli Scenario Routing

Use this quick map after selecting the chip family and SDK version.

| Scenario | Where to Look |
|---|---|
| New project | `chips/<family>/recipes/new_project.md`, closest SDK `projects/` sample, and `resources/sdk_index.md` |
| Offline ASR | Family ASR recipe, SDK `projects/*/firmware/asr/`, model files, and `user_config.h` |
| Wake word and command words | ASR recipe plus `DEFAULT_MODEL_GROUP_ID`, confidence, timeout, and model package settings |
| CWSL, NL ASR, multi-intent | CI13LC or CI13XX recipes and exact SDK docs/examples |
| AEC, beamforming, DOA, denoise | CI130X/CI13XX audio algorithm recipes and headers |
| BLE voice or BLE broadcast | `chips/ci23lc/recipes/` and BLE app/config folders |
| Wi-Fi voice combo or OTA | `chips/ci230x/recipes/`, CMake project, partition config, and LN882H flashing docs |
| IR, UART, GPIO, PWM, timer, codec, player | Family recipe first, then SDK headers/source and closest sample |

## Answer Discipline

For generated code, include the selected chip, SDK version, source/example evidence, `user_config.h` changes, `source_file.prj` additions, and firmware packing/model requirements.
