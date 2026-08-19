#
# Rules for building library
#

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------
ROOT_PATH := ../../../../..

include $(ROOT_PATH)/gcc.mk

PROJ_PATH := ../../..

# ----------------------------------------------------------------------------
# library and objects
# ----------------------------------------------------------------------------
LIBS := libtvscore.a

DIRS_IGNORE := 
FORDORS := \
		$(PROJ_PATH)/compatible/src \
		$(PROJ_PATH)/tvs_sdk/src \
		$(PROJ_PATH)/os/freeRTOS \
		$(PROJ_PATH)/third_party/src/mongoose \
		$(PROJ_PATH)/third_party/src/net_ping
		
DIRS_ALL := $(shell find $(FORDORS) -type d)
DIRS := $(filter-out $(DIRS_IGNORE),$(DIRS_ALL))

SRCS := $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS])))

OBJS := $(addsuffix .o,$(SRCS))

# extra flags:
CC_FLAGS += -I$(PROJ_PATH)/tvs_sdk/inc
CC_FLAGS += -I$(PROJ_PATH)/tvs_sdk/tvs_sdk_api
CC_FLAGS += -I$(PROJ_PATH)/os/inc 
CC_FLAGS += -I$(PROJ_PATH)/compatible/inc 
CC_FLAGS += -I$(PROJ_PATH)/third_party/inc/mongoose 
CC_FLAGS += -I$(PROJ_PATH)/third_party/inc/net_ping
CC_FLAGS += -I$(PROJ_PATH)/third_party/inc/ 
CC_FLAGS += -I$(PROJ_PATH)/tvs_common

CC_FLAGS += -I$(ROOT_PATH)/include/net
CC_FLAGS += -I$(ROOT_PATH)/include/cjson
CC_FLAGS += -DCONFIG_HAS_MBED_SSL_CONF=1 -DPLATFORM_FREERTOS

# library make rules
include $(LIB_MAKE_RULES)

