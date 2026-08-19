---
name: chipintelli-dev-skill
description: >-
  Use when developing, configuring, building, debugging, or reviewing firmware for
  Chipintelli (启英泰伦) CI110X, CI112X, CI130X, CI13LC, CI13XX, CI230X, or CI23LC
  voice MCUs. Routes work to local SDK recipes, headers, source, and examples for
  ASR, audio algorithms, BLE/Wi-Fi combinations, IR control, OTA, and peripherals.
license: MIT
metadata:
  author: JasonYANG170
  version: "1.2.0"
  platforms: [windows]
  hermes:
    tags: [embedded, chipintelli, riscv, voice, asr, ble, wifi, firmware]
    related_skills: [systematic-debugging, test-driven-development, requesting-code-review]
---

# chipintelli-dev-skill

Use this skill for Chipintelli voice MCU firmware. The local SDK collection is large, so route to the exact chip family and SDK version before reading examples or writing code.

## Route First

1. Identify the exact chip marking, board, SDK version, algorithm package, toolchain, and requested feature.
2. Read `resources/chip_matrix.md` to choose `chips/<family>/`.
3. Read `resources/sdk_index.md` to choose a compatible SDK. Existing project SDK wins; for new projects, use the newest SDK that explicitly supports the chip, board, model flow, and connectivity needs.
4. Read `resources/scenario_routing.md` if the feature-to-recipe path is not obvious.
5. For implementation, read the selected family recipe and the closest SDK `projects/` example.

## Evidence Order

Before emitting code or build steps, verify names in this order:

1. User's existing project files: `user_config.h`, `source_file.prj`, board config, flash layout, and application code.
2. Exact SDK headers/source for the selected chip family and version.
3. Closest SDK `projects/` example.
4. Family resources and recipes.
5. Generic FreeRTOS, RISC-V, or ARM knowledge only after SDK symbols are verified.

If sources conflict, follow the user's selected SDK and state the mismatch.

## Coding Rules

- Treat `user_config.h`, `source_file.prj`, ASR assets, voice prompt files, flash layout, and firmware packing as part of the implementation.
- Do not blindly use the latest SDK. Use the project SDK unless migration is requested.
- Do not mix CI130X, CI13LC, CI13XX, CI230X, and CI23LC examples unless the same SDK explicitly documents support.
- CI1312 routes through CI13LC unless a selected unified CI13XX SDK proves otherwise.
- CI230X is ARM Cortex-M4/LN882H based; do not reuse RISC-V startup, interrupt, or toolchain assumptions.
- For ASR changes, show where recognition messages enter `sys_msg_queue` and where application handling belongs.

## Key References

- `resources/chip_matrix.md` - chip-to-family routing.
- `resources/sdk_index.md` - SDK version selection.
- `resources/scenario_routing.md` - feature-to-recipe routing.
- `resources/development_workflow.md` - shared implementation workflow.
- `resources/source_strategy.md` - how to search SDK trees without mixing versions.
- `resources/recipe_quality.md` - required metadata and evidence standard for recipes.
- `scripts/validate_mcu_skill.py` - structural and recipe-quality smoke check.
- `scripts/check_recipe_symbols.py` - optional symbol-evidence scan for a selected family or recipe.
- `resources/recipe_index.md` - cross-family recipe index.
- `chips/<family>/recipes/*.md` and `chips/<family>/resources/` - scenario and API evidence.

## Stop Conditions

Ask for the exact chip and SDK version when they materially change the answer. If a symbol, config, or feature cannot be verified in the selected SDK or examples, mark it unverified instead of inventing a generic MCU implementation.
