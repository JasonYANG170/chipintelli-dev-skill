# Creating a New CI13LC Project

## Prerequisites

- `riscv-nuclei-elf-gcc-9.2.0` toolchain installed
- VS Code with `ci-tool` extension installed
- CI13LC SDK (recommend `CI13LC_SDK_2.1.5` or `CI13LC_SDK_V2.0.15`)

## Steps

### 1. Copy the closest sample

```bash
cd CI13LC_SDK_2.1.5
cp -r projects/offline_asr_sample projects/my_project
```

### 2. Edit `user_config.h`

Select your board and chip:
```c
// Select board (see driver/boards/ for available options)
#define USE_CI_F16XGS02J_BOARD  1  // CI13242 module
// Set chip type
#define CI_CHIP_TYPE            13242
#define BOARD_PORT_FILE         "CI-F16XGS02J.c"
```

Configure microphone:
```c
#define MIC_DIFF_SINGLE         0  // 0=differential, 1=single-end
```

Configure UART:
```c
#define CONFIG_CI_LOG_UART          HAL_UART0_BASE
#define MSG_COM_USE_UART_EN         1
#define UART_PROTOCOL_NUMBER        (HAL_UART2_BASE)
#define UART_PROTOCOL_BAUDRATE      (UART_BaudRate9600)
```

Configure ASR:
```c
#define USE_SEPARATE_WAKEUP_EN      1
#define DEFAULT_MODEL_GROUP_ID      1
#define EXIT_WAKEUP_TIME            15000  // 15s
```

### 3. Update `source_file.prj`

If you add new source files, add them to `source_file.prj`:
```
source-file: projects/my_project/src/my_code.c
```

### 4. Place model files

- ASR model: `firmware/asr/` (from voice AI platform)
- DNN model: `firmware/dnn/`
- Voice prompts: `firmware/voice/`

### 5. Build

```bash
# Set up environment
export PATH="$SDK_ROOT/tools/build-tools/bin:$GCC_ROOT/gcc_fix_raissrc/bin:$PATH"
export SDK_PATH="$SDK_ROOT"

# Build
cd projects/my_project/project_file
make -j4
```

### 6. Pack firmware

Use `tools/PACK_UPDATE_TOOL.exe` to create the final firmware image.

### 7. Flash

Use `tools/ci-tool-kit.exe` or `tools/code_program.exe` to flash.

## Chip Variant Selection

| Chip | Flash | Package | Board File |
|---|---|---|---|
| CI1312 | 2MB | SSOP16 | CI-D12GS01J.c |
| CI13241 | 4MB | QFN32 | CI-F24XGS01J.c |
| CI13242 | 4MB | QFN32 | CI-F16XGS02J.c or CI-F24XGS01S.c |
| CI13322 | 4MB | QFN56 | CI-F322GS01S.c |
