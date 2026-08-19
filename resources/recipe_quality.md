# Chipintelli Recipe Quality Standard

Use this standard when creating or revising Chipintelli recipes. The goal is to keep code tied to the selected SDK, voice model flow, and board configuration.

## Required Metadata

Every recipe should state near the top:

- Applicable chips and excluded chips.
- SDK version and whether the recipe is family-specific or unified-SDK based.
- Board/config assumptions.
- Example project and source files used as evidence.
- Required ASR/model/voice/packing assets.
- Validation level: `compiled`, `source-matched`, `example-derived`, or `draft`.

## Required Implementation Evidence

For each nontrivial code block, cite or name:

- Header/source file containing each API.
- `user_config.h` options touched.
- `source_file.prj` additions for new source files.
- Message queue, task, or callback path for runtime behavior.
- Flash partition or pack-tool requirements.
- Toolchain/build command for the selected SDK.

## Review Checklist

- No SDK-version mixing.
- No CI130X/CI13LC/CI13XX/CI230X/CI23LC example mixing without explicit SDK support.
- No RISC-V assumptions in CI230X ARM/LN882H code.
- No ASR code path without model files and flash packing notes.
- No new source file omitted from `source_file.prj`.

## Optional Symbol Scan

Run `scripts/check_recipe_symbols.py --scope-glob "chips/<family>/recipes/*.md"` when revising a family, or `--recipe-glob "chips/<family>/recipes/<recipe>.md"` for one file. Treat findings as review leads; project-local callbacks and voice-platform-generated handlers often need manual classification.
