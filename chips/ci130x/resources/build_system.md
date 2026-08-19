# CI130X Build System

> SDK: `CI130X_SDK_Offline_V2.1.14`  
> The CI130X SDK uses a **Make + Lua** build system. Lua parses a project file and generates Makefile rules.

---

## Overview

```
source_file.prj  ──►  generate_makefile.lua  ──►  build/source_file.mk
                                                        │
Makefile  ──►  common_head.mk  ──►  common_flags.mk  ──►  common_tail.mk
     │              │                   │                    │
     └── project-specific flags ────────┘                    │
                                                             ▼
                                                   Final firmware binary
```

### Build Flow

1. `make` reads the project `Makefile`
2. `Makefile` includes `common_head.mk` (compiler paths, variables)
3. `Makefile` sets project-specific flags (C_FLAGS, LD_FLAGS)
4. `common_tail.mk` rule triggers `generate_makefile.lua` to parse `source_file.prj`
5. Lua generates `build/source_file.mk` with compilation rules for each source file
6. `Makefile` includes `build/source_file.mk`
7. Compiler (`riscv-nuclei-elf-gcc`) compiles each `.c`/`.S` file
8. Linker (`riscv-nuclei-elf-g++`) links all objects + libraries into `.elf`
9. `objcopy` converts `.elf` to `.bin`
10. `ci-tool-kit merge user-file` packs binary with firmware files

---

## source_file.prj Format

**Location**: `projects/<project_name>/project_file/source_file.prj`

This is the central project configuration file. It lists all source files, libraries, include paths, and build macros.

### Syntax

```
// Comment line (starts with //)

// Build configuration macros
define-macro: MACRO_NAME=VALUE

// Build config options
build-config: OPTION_NAME=VALUE

// Source files (relative to SDK root)
source-file: path/to/source.c
source-file: path/to/assembly.S

// Library files
library-file: $(LIBS_PATH)/libname.a

// Include paths (relative to SDK root)
include-path: path/to/headers
```

### Example (from offline_asr_pro_sample)

```
//config
define-macro: ASR_CODE_VERSION=2

//compile options
build-config: USE_MORE_WORDS_LIBRARY=0

//source files.
source-file: startup/ci130x_init.c
source-file: startup/ci130x_vtable.S
source-file: startup/ci130x_startup.S

source-file: system/ci130x_it.c
source-file: system/ci130x_system.c
source-file: system/platform_config.c
source-file: system/ci130x_handlers.c
source-file: system/baudrate_calibrate.c

source-file: components/freertos/croutine.c
source-file: components/freertos/event_groups.c
source-file: components/freertos/list.c
source-file: components/freertos/queue.c
source-file: components/freertos/stream_buffer.c
source-file: components/freertos/tasks.c
source-file: components/freertos/timers.c
source-file: components/freertos/portable/MemMang/heap_4.c

source-file: components/log/ci_log.c
source-file: components/player/audio_play/audio_play_api.c
source-file: components/asr/asr_process_callback.c
source-file: components/codec_manager/codec_manager.c

source-file: driver/ci130x_chip_driver/src/ci130x_gpio.c
source-file: driver/ci130x_chip_driver/src/ci130x_uart.c
// ... more driver source files

source-file: projects/offline_asr_pro_sample/src/main.c
source-file: projects/offline_asr_pro_sample/src/user_msg_deal.c

//library files
library-file: $(LIBS_PATH)/libasr_v2.a
library-file: $(LIBS_PATH)/libnewlib_port.a
library-file: $(LIBS_PATH)/libfreertos_port.a
library-file: $(LIBS_PATH)/libdsu.a
library-file: $(LIBS_PATH)/libflash_encrypt.a

//header file paths
include-path: driver/ci130x_chip_driver/inc
include-path: driver/boards
include-path: components
include-path: components/asr
include-path: components/player/audio_play
// ... more include paths
```

### Commenting Out Files

Prefix a line with `//` to exclude a source file:

```
//source-file: components/example/iic_test.c
//source-file: components/player/sonic/sonic.c
```

---

## generate_makefile.lua

**Location**: `utils/generate_makefile.lua`

This Lua script parses `source_file.prj` and generates `build/source_file.mk`.

### How It Works

1. Reads `source_file.prj` line by line
2. For each `source-file:` line:
   - Extracts file extension (`.c`, `.S`, `.a`)
   - For `.c` files: generates `OBJS += build/objs/<name>.o` and a compilation rule using `$(CC)`
   - For `.S` files: generates compilation rule using `$(AS)`
   - For `.a` files: generates `LIB_FILES +=` and `-L` / `-l` flags
3. For each `include-path:` line: generates `C_FLAGS += -I$(ROOT_DIR)/<path>`
4. Writes all rules to `build/source_file.mk`

### Generated source_file.mk Example

```makefile
# This file is maked by run generate_makefile.lua
OBJS += build/objs/main.o
-include build/objs/main.d
build/objs/main.o : $(ROOT_DIR)/projects/offline_asr_pro_sample/src/main.c
	$(CC_PREFIX)$(CC) $(C_FLAGS) -c -o "$@" "$<"

OBJS += build/objs/ci130x_gpio.o
-include build/objs/ci130x_gpio.d
build/objs/ci130x_gpio.o : $(ROOT_DIR)/driver/ci130x_chip_driver/src/ci130x_gpio.c
	$(CC_PREFIX)$(CC) $(C_FLAGS) -c -o "$@" "$<"
```

### Trigger

The regeneration is triggered by this rule in `common_tail.mk`:

```makefile
build/source_file.mk: source_file.prj
	$(LUA) $(SDK_PATH)/utils/generate_makefile.lua source_file.prj $(SDK_PATH)
```

---

## Makefile Structure

**Location**: `projects/<project_name>/project_file/Makefile`

### Project Makefile

```makefile
SDK_PATH ?= ../../..

include $(SDK_PATH)/utils/common_head.mk

SECONDARY_FLASH := ../firmware/user_code/[0]code.bin
SECONDARY_LIST := $(BUILD_DIR)/$(PROJECT_NAME).lst
LTO_OPTION = -flto
LIBS := -lm

C_FLAGS += -march=rv32imafc -mabi=ilp32f -mcmodel=medlow ...
C_FLAGS += -DCI_CONFIG_FILE=\"user_config.h\" -DCORE_ID=0
C_FLAGS += -fshort-enums

S_FLAGS := -march=rv32imafc -mabi=ilp32f ...
LD_FLAGS := -march=rv32imafc -mabi=ilp32f ...
LD_FLAGS += -g -T "../src/ci130x.lds" -nostartfiles -Xlinker --gc-sections
LD_FLAGS += -Wl,--wrap,memcmp -Wl,--wrap,memcpy ...  # Wrap libc functions

all: secondary-outputs

ifneq ($(MAKECMDGOALS), clean)
-include $(BUILD_DIR)/source_file.mk

ifeq ($(USE_MORE_WORDS_LIBRARY), 0)
LIB_FILES += $(SDK_PATH)/$(LIBS_PATH)/libcikd_pro.a
LIBS += -lcikd_pro
else
LIB_FILES += $(SDK_PATH)/$(LIBS_PATH)/libcikd_pro_more_words.a
LIBS += -lcikd_pro_more_words
endif
endif

$(BUILD_DIR)/$(PROJECT_NAME).elf: $(OBJS) $(USER_OBJS) build/source_file.mk $(LIB_FILES) ../src/ci130x.lds makefile
	$(CC_PREFIX)$(LD) $(LD_FLAGS) -o "$(BUILD_DIR)/$(PROJECT_NAME).elf" $(OBJS) $(USER_OBJS) $(LIBS)

$(SECONDARY_FLASH): $(BUILD_DIR)/$(PROJECT_NAME).elf
	@mkdir -p ../firmware/user_code
	$(CC_PREFIX)$(OC) -O binary "$(BUILD_DIR)/$(PROJECT_NAME).elf" $(SECONDARY_FLASH)
	@cp $(SDK_PATH)/$(LIBS_PATH)/libfbin_pro.a ../firmware/user_code/[1]code.bin
	@$(SDK_PATH)/tools/ci-tool-kit merge user-file -i ../firmware/user_code
	@rm ../firmware/user_code/[1]code.bin

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST)

include $(SDK_PATH)/utils/common_tail.mk
```

---

## common_head.mk

**Location**: `utils/common_head.mk`

Sets up global variables and toolchain paths.

| Variable | Value | Description |
|----------|-------|-------------|
| `BUILD_DIR` | `build` | Build output directory |
| `PROJECT_NAME` | Auto from directory name | Project name |
| `LIBS_PATH` | `libs` (Windows) / `libs/linux` (Linux) | Library path |
| `LUA` | `$(SDK_PATH)/tools/build-tools/bin/lua.exe` | Lua interpreter |
| `CC_PREFIX` | `riscv-nuclei-elf-` | Toolchain prefix |
| `CC` | `gcc` | C compiler |
| `AS` | `gcc` | Assembler |
| `LD` | `g++` | Linker |
| `OD` | `objdump` | Object dump |
| `OC` | `objcopy` | Object copy |
| `SIZE` | `size` | Size tool |
| `O_OPTION` | `-Os` | Optimization level |

---

## common_flags.mk

**Location**: `utils/common_flags.mk`

Sets default compiler and assembler flags. Included by projects that need standard flags without overriding everything.

```makefile
C_FLAGS += -march=rv32imafc -mabi=ilp32f -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mfdiv
C_FLAGS += -fsigned-char -ffunction-sections -fdata-sections -fno-common -fshort-enums -std=gnu11
S_FLAGS += -march=rv32imafc -mabi=ilp32f ... -x assembler-with-cpp

# Debug info only when not release
ifneq ($(RELEASE), TRUE)
C_FLAGS += -g
S_FLAGS += -g
endif
```

### Key Compiler Flags

| Flag | Description |
|------|-------------|
| `-march=rv32imafc` | RISC-V RV32IMAFC architecture (Integer + Mul + Atomic + Float + Compressed) |
| `-mabi=ilp32f` | ABI: 32-bit integer, 32-bit float |
| `-mcmodel=medlow` | Medium-low code model |
| `-msave-restore` | Use save/restore library for smaller code |
| `-mfdiv` | Hardware float divide |
| `-fshort-enums` | Enums use minimum bytes (1 byte for small enums) |
| `-ffunction-sections` | Each function in its own section (for --gc-sections) |
| `-fdata-sections` | Each data item in its own section |
| `-DCI_CONFIG_FILE=\"user_config.h\"` | User configuration file path |
| `-DCORE_ID=0` | Host core identifier |

---

## common_tail.mk

**Location**: `utils/common_tail.mk`

Defines pattern rules, the source_file.mk generation rule, and the clean target.

```makefile
%.o : %.c
	$(CC_PREFIX)$(CC) $(C_FLAGS) -c -o "$@" "$<"

%.o : %.S
	$(CC_PREFIX)$(AS) $(S_FLAGS) -c -o "$@" "$<"

build/source_file.mk: source_file.prj
	$(LUA) $(SDK_PATH)/utils/generate_makefile.lua source_file.prj $(SDK_PATH)

clean:
	-$(RM) -rf build/*
	-$(RM) -rf *.a

.PHONY: all clean dependents
```

---

## How to Add a New Source File

### Step 1: Create the source file

```bash
# Create new file in project src directory
# projects/offline_asr_pro_sample/src/my_custom_code.c
```

### Step 2: Add to source_file.prj

```
# In projects/offline_asr_pro_sample/project_file/source_file.prj
# Add after existing source files:
source-file: projects/offline_asr_pro_sample/src/my_custom_code.c
```

### Step 3: Add include path (if new directory)

```
# In source_file.prj, add:
include-path: projects/offline_asr_pro_sample/src/my_include_dir
```

### Step 4: Rebuild

```bash
cd projects/offline_asr_pro_sample/project_file
make clean
make -j4
```

### Step 5: Add library file (if needed)

```
# In source_file.prj:
library-file: $(LIBS_PATH)/libmy_library.a
```

The Lua script automatically adds `-L` and `-l` flags for library files.

---

## Build Commands

### Environment Setup

```bash
# Set PATH to include build tools and compiler
export SDK_ROOT="/d/启英泰伦/CI130X_SDK_Offline_V2.1.14/ci130x_sdk"
export PATH="$SDK_ROOT/tools/build-tools/bin:$PATH"
# Add RISC-V GCC to PATH (adjust path as needed)
export PATH="/path/to/riscv-nuclei-elf-gcc/bin:$PATH"
```

### Build

```bash
cd $SDK_ROOT/projects/offline_asr_pro_sample/project_file
make -j4
```

### Clean Build

```bash
make clean
make -j4
```

### Release Build (no debug info)

```bash
make RELEASE=TRUE -j4
```

### Build with More Words Library

```
# In source_file.prj:
build-config: USE_MORE_WORDS_LIBRARY=1
```

```bash
make -j4
```

---

## Build Output

| File | Location | Description |
|------|----------|-------------|
| `build/<project>.elf` | `project_file/build/` | ELF binary (for debugging) |
| `build/<project>.map` | `project_file/build/` | Memory map |
| `build/<project>.lst` | `project_file/build/` | Disassembly listing |
| `build/objs/*.o` | `project_file/build/objs/` | Object files |
| `build/source_file.mk` | `project_file/build/` | Generated Makefile rules |
| `firmware/user_code/[0]code.bin` | `firmware/user_code/` | Final packed firmware binary |

---

## Firmware Packaging

The final firmware binary is created by `ci-tool-kit merge user-file`:

```bash
# This happens automatically during build:
mkdir -p ../firmware/user_code
objcopy -O binary build/project.elf ../firmware/user_code/[0]code.bin
cp $(SDK_PATH)/libs/libfbin_pro.a ../firmware/user_code/[1]code.bin
$(SDK_PATH)/tools/ci-tool-kit merge user-file -i ../firmware/user_code
rm ../firmware/user_code/[1]code.bin
```

The merge tool combines:
- `[0]code.bin` - User application code
- `firmware/asr/` - ASR model files
- `firmware/dnn/` - DNN model files
- `firmware/voice/` - Voice prompt files
- `firmware/user_file/` - User data files

Into a single firmware image for flashing.
