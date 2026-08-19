
SDK_PATH ?= ../../..

ROOT_DIR := $(SDK_PATH)
BUILD_DIR := build

include $(SDK_PATH)/utils/common_head.mk

USE_RTC_ENABLE_RECORD	:= 0
USE_RTC_DISABLE_RECORD	:= 1
#0: CI_RTC_TYPE = USE_RTC_ENABLE_RECORD，开启录音功能并且固件加密。
#1：CI_RTC_TYPE = USE_RTC_DISABLE_RECORD，关闭录音功能且固件未加密。

CI_RTC_TYPE	:= $(USE_RTC_DISABLE_RECORD)

ifeq ($(CI_RTC_TYPE), $(USE_RTC_ENABLE_RECORD))
C_FLAGS	+= -DUSE_FLASH_RECORD_PLAY_ENABLE=1
RTC_LINK_LDS := ../src/ci13lc_rtc_enable_record.lds
LIBS := -lrtc_enable_record
else ifeq ($(CI_RTC_TYPE), $(USE_RTC_DISABLE_RECORD))
C_FLAGS	+= -DUSE_FLASH_RECORD_PLAY_ENABLE=0
RTC_LINK_LDS := ../src/ci13lc_rtc_disable_record.lds
LIBS := -lrtc_disable_record
endif

LTO_OPTION := -flto

SECONDARY_FLASH := ../firmware/user_code/[0]code.bin 
SECONDARY_LIST := $(BUILD_DIR)/$(PROJECT_NAME).lst


LIBS += -lm

include $(SDK_PATH)/utils/common_flags.mk

C_FLAGS += -DCI_CONFIG_FILE=\"user_config.h\" 
# C_FLAGS += -DCORE_ID=0 -Werror

TS := $(shell echo $(patsubst ^+*, ,$(subst T, ,$(shell echo `date -Iseconds`))) | head -c19)
C_FLAGS += -DCOMPILE_TIME="\"$(TS)\""

# enum only cost 1 byte
C_FLAGS += -fshort-enums
S_FLAGS += -I$(SDK_PATH)/components -I$(SDK_PATH)/driver/ci13lc_chip_driver/inc
LD_FLAGS += -march=rv32imafc -mabi=ilp32f -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mfdiv $(O_OPTION) -fmessage-length=0	$(LTO_OPTION)
LD_FLAGS += -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-delete-null-pointer-checks -fno-unroll-loops
LD_FLAGS += -T "$(RTC_LINK_LDS)" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"$(BUILD_DIR)/$(PROJECT_NAME).map" --specs=nano.specs #-Wl,-no-relax

include $(SDK_PATH)/utils/romlib_api.mk

LD_FLAGS += -L$(SDK_PATH)/$(LIBS_PATH) -L$(SDK_PATH)/components/freertos/portable/GCC
LD_FLAGS += -L$(SDK_PATH)/components/asr/decoder -L$(SDK_PATH)/utils

all: secondary-outputs



ifneq ($(MAKECMDGOALS), clean)
-include $(BUILD_DIR)/source_file.mk
endif



$(BUILD_DIR)/$(PROJECT_NAME).elf: $(OBJS) $(USER_OBJS) build/source_file.mk $(LIB_FILES) $(RTC_LINK_LDS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C++ Linker'
	$(CC_PREFIX)$(LD) $(LD_FLAGS) -o "$(BUILD_DIR)/$(PROJECT_NAME).elf" $(OBJS) $(USER_OBJS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

$(SECONDARY_FLASH): $(BUILD_DIR)/$(PROJECT_NAME).elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	@mkdir -p ../firmware/user_code && $(CC_PREFIX)$(OC) -O binary "$(BUILD_DIR)/$(PROJECT_NAME).elf"  $(SECONDARY_FLASH)
	@$(SDK_PATH)/tools/ci-tool-kit.exe merge user-file -i ../firmware/user_code > /dev/null
	@echo 'Finished building: $@'
	@echo ' '

$(BUILD_DIR)/$(PROJECT_NAME).lst: $(BUILD_DIR)/$(PROJECT_NAME).elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	$(CC_PREFIX)$(OD) --demangle --disassemble --reloc --wide "$(BUILD_DIR)/$(PROJECT_NAME).elf" > "$(BUILD_DIR)/$(PROJECT_NAME).lst"
	$(CC_PREFIX)$(SIZE) --format=berkeley "$(BUILD_DIR)/$(PROJECT_NAME).elf"
	@echo 'Finished building: $@'
	@echo ' '



secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST)


include $(SDK_PATH)/utils/common_tail.mk

.PHONY: build/objs/main.o
