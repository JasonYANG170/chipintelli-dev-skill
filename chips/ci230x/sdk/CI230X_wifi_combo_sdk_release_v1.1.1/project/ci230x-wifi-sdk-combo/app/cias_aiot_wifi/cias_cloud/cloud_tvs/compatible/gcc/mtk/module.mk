TVS_SDK_PATH   =      ../../..

TVS_SDK_FORDER = ../../../tvs_sdk/src \
		../../../compatible/src \
		../../../os/freeRTOS \
		../../../third_party/src/mongoose \
		../../../third_party/src/net_ping
		
DIRS_ALL := $(shell find $(TVS_SDK_FORDER) -type d)

SRCS := $(foreach dir,$(DIRS_ALL),$(wildcard $(dir)/*.[csS]))

SRCS1 := $(subst ../../../, /, $(SRCS))

TVS_SRC_FILES := $(addprefix $(TVS_SDK_PATH),$(SRCS1))

C_FILES  += $(TVS_SRC_FILES)

CFLAGS         +=     -DPLATFORM_FREERTOS

CFLAGS         +=     -I$(SOURCE_DIR)/../../tvs_rtos_sdk/tvs_sdk/inc
CFLAGS         +=     -I$(SOURCE_DIR)/../../tvs_rtos_sdk/compatible/inc
CFLAGS         +=     -I$(SOURCE_DIR)/../../tvs_rtos_sdk/os/inc
CFLAGS         +=     -I$(SOURCE_DIR)/../../tvs_rtos_sdk/tvs_sdk/tvs_sdk_api
CFLAGS         +=     -I$(SOURCE_DIR)/../../tvs_rtos_sdk/third_party/inc/mongoose 
CFLAGS         +=     -I$(SOURCE_DIR)/../../tvs_rtos_sdk/third_party/inc/net_ping 
CFLAGS         +=     -I$(SOURCE_DIR)/../../tvs_rtos_sdk/tvs_common

CFLAGS         +=     -I$(SOURCE_DIR)/middleware/third_party/speex/include
