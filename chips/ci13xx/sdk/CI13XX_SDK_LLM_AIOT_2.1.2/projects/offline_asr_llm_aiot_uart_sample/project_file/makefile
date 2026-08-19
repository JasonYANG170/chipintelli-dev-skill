
SDK_PATH ?= ../../..

include $(SDK_PATH)/utils/common_head.mk

########单一算法定义(注意:下面算法变量的值用户不可修改，值后更不能有空格)######## 
#AEC+NN DENOISE
USE_AEC_DENOISE_NN    := 0
#ANY+AEC+DENOISE  需要外部挂7243e codec，-双mic算法，仅双mic可用
USE_ANY_MIC_AEC_DENOISE_NN = 1
#自学习+AEC+DENOISE
USE_CWSL_AEC_DENOISE_NN    := 2
#DOA+AEC+DENOISE 该方案只支持上下行PCM
USE_AI_DOA_AEC_DENOISE_NN  := 3
#######################################################################   
CI_ALG_TYPE := $(USE_AEC_DENOISE_NN)


########可组合算法使能(注意:设置变量值后面不能有空格，只能是0/1/2...)########
#---AIOT 音频压缩类型  	#0-null  1-speex  2-opus 3-g722
AIOT_AUDIO_COMPRESS_TYPE       := 1
#---音频上传云端接口类型: #1-串口输出  2-hpout输出  3-iis输出 (不可修改)
AUDIO_UPLOAD_TYPE              := 1
#---播放器类型: 0-精简播放器(相比老播放器节约内存15KB,功能简单) 1-老播放器(占内存较多，功能完善)
USE_AUDIO_PLAYER_TYPE          := 0
########################################################################

#算法功能编译配置
ifeq ($(CI_ALG_TYPE), $(USE_NULL))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_null.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_null.a
C_FLAGS += -DNO_ASR_FLOW=0 -DUSE_NULL=1
LIBS := -lasr_dis_deepse_v2 
else ifeq ($(CI_ALG_TYPE), $(USE_DENOISE_NN))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_denoise.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_denoise.a
C_FLAGS += -DUSE_DENOISE_NN=1
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_VPR))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_vpr.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_vpr.a
C_FLAGS += -DUSE_VPR=1 
LIBS := -lasr_dis_deepse_v2
USE_DECODE_FRM := 1
else ifeq ($(CI_ALG_TYPE), $(USE_WMAN_VPR))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_wman_vpr.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_wman_vpr.a
C_FLAGS += -DUSE_WMAN_VPR=1
LIBS := -lasr_dis_deepse_v2
USE_DECODE_FRM := 1
else ifeq ($(CI_ALG_TYPE), $(USE_AI_DOA))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_doa.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_doa.a
C_FLAGS += -DUSE_AI_DOA=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2
LIBS := -lasr_dis_deepse_v2
USE_DECODE_FRM := 1
else ifeq ($(CI_ALG_TYPE), $(USE_SED_CRY))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_sed.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_sed_cry.a
C_FLAGS += -DUSE_SED=1 -DUSE_SED_CRY=1 -DNO_ASR_FLOW=1 
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_SED_SNORE))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_sed.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_sed_snore.a
C_FLAGS += -DUSE_SED=1 -DUSE_SED_SNORE=1 -DNO_ASR_FLOW=1 
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_CWSL))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_cwsl.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_cwsl.a
C_FLAGS += -DUSE_CWSL=1
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_aec.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_aec.a  
C_FLAGS += -DUSE_AEC_MODULE=1  -DHOST_MIC_USE_NUMBER=1 -DHOST_CODEC_CHA_NUM=2
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_DEREVERB))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_dereverb.lds
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_dereverb.a
C_FLAGS += -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DUSE_DEREVERB_MODULE=1 -DHOST_MIC_USE_NUMBER=2
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_CWSL_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_cwsl_aec.lds
C_FLAGS += -DUSE_CWSL=1 -DUSE_AEC_MODULE=1 -DHOST_MIC_USE_NUMBER=1 -DHOST_CODEC_CHA_NUM=2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_cwsl_aec.a
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_TTS))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_tts.lds
C_FLAGS += -DUSE_TTS=1 -DNO_ASR_FLOW=1
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_tts.a
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_BF))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_bf.lds
C_FLAGS += -DDEEP_SEPARATE_ENABLE=1 -DUSE_BEAMFORMING_MODULE=1  -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_bf.a
LIBS := -lasr_use_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_AI_DOA_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_doa_aec.lds
C_FLAGS += -DUSE_AI_DOA=1 -DUSE_AEC_MODULE=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2  -DHOST_MIC_USE_NUMBER=2 -DUSE_IIS1_OUT_PRE_RSLT_AUDIO=1 
LIBS := -lasr_dis_deepse_v2
USE_DECODE_FRM := 1
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_doa_aec.a
else ifeq ($(CI_ALG_TYPE), $(USE_DEREVERB_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_dereverb_aec.lds
C_FLAGS += -DUSE_DEREVERB_MODULE=1 -DUSE_AEC_MODULE=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1  -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 -DUSE_IIS1_OUT_PRE_RSLT_AUDIO=1 
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_dereverb_aec.a
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_BF_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_bf_aec.lds
C_FLAGS += -DDEEP_SEPARATE_ENABLE=1  -DUSE_BEAMFORMING_MODULE=1 -DUSE_AEC_MODULE=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1  -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 -DUSE_IIS1_OUT_PRE_RSLT_AUDIO=1
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_bf_aec.a
LIBS := -lasr_use_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_DOA_DEREVERB))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_doa_dereverb.lds
C_FLAGS += -DUSE_AI_DOA=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DUSE_DEREVERB_MODULE=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 
LIBS := -lasr_dis_deepse_v2
USE_DECODE_FRM := 1
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_doa_dereverb.a
else ifeq ($(CI_ALG_TYPE), $(USE_BF_DEREVERB))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_bf_dereverb.lds
C_FLAGS += -DDEEP_SEPARATE_ENABLE=1 -DUSE_BEAMFORMING_MODULE=1 -DUSE_DEREVERB_MODULE=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2  -DHOST_MIC_USE_NUMBER=2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_bf_dereverb.a
LIBS := -lasr_use_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_CWSL_DOA))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_cwsl_doa.lds
C_FLAGS += -DUSE_CWSL=1 -DUSE_AI_DOA=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_cwsl_doa.a
LIBS := -lasr_dis_deepse_v2
USE_DECODE_FRM := 1
else ifeq ($(CI_ALG_TYPE), $(USE_ANY_MIC_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_any_mic_aec.lds
C_FLAGS += -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DUSE_DUAL_MIC_ANY=1 -DUSE_AEC_MODULE=1  -DHOST_CODEC_CHA_NUM=2  -DHOST_MIC_USE_NUMBER=2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_any_mic_aec.a
LIBS := -lasr_dis_deepse_v2
else ifeq ($(CI_ALG_TYPE), $(USE_CWSL_DOA_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_cwsl_doa_aec.lds
C_FLAGS +=-DUSE_CWSL=1 -DUSE_AEC_MODULE=1 -DUSE_AI_DOA=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 -DUSE_IIS1_OUT_PRE_RSLT_AUDIO=1  
LIBS := -lasr_dis_deepse_v2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_cwsl_doa_aec.a
USE_DECODE_FRM := 1
else ifeq ($(CI_ALG_TYPE), $(USE_CSWL_ANY_MIC_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_cwsl_any_mic_aec.lds
C_FLAGS += -DUSE_CWSL=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DUSE_DUAL_MIC_ANY=1 -DUSE_AEC_MODULE=1  -DHOST_CODEC_CHA_NUM=2  -DHOST_MIC_USE_NUMBER=2
LIBS := -lasr_dis_deepse_v2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_cwsl_any_mic_aec.a
else ifeq ($(CI_ALG_TYPE), $(USE_DOA_DEREVERB_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_doa_dereverb_aec.lds
C_FLAGS += -DUSE_AI_DOA=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DUSE_DEREVERB_MODULE=1 -DUSE_AEC_MODULE=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 
LIBS := -lasr_dis_deepse_v2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_doa_dereverb_aec.a
USE_DECODE_FRM := 1
else ifeq ($(CI_ALG_TYPE), $(USE_CWSL_DOA_DEREVERB_AEC))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_cwsl_doa_dereverb_aec.lds
C_FLAGS +=  -DUSE_CWSL=1 -DUSE_AI_DOA=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DUSE_DEREVERB_MODULE=1 -DUSE_AEC_MODULE=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 
LIBS := -lasr_dis_deepse_v2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_cwsl_doa_dereverb_aec.a
USE_DECODE_FRM := 1
else ifeq ($(CI_ALG_TYPE), $(USE_AI_DOA_AEC_DENOISE_NN))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_doa_aec_denoise_nn.lds
C_FLAGS += -DUSE_DENOISE_NN=1  -DUSE_VOX_MODULE=1  -DUSE_AI_DOA=1 -DUSE_AEC_MODULE=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2  -DHOST_MIC_USE_NUMBER=2 -DUSE_IIS1_OUT_PRE_RSLT_AUDIO=1 
LIBS := -lasr_dis_deepse_v2
USE_DECODE_FRM := 1
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_doa_aec_denoise_nn.a
else ifeq ($(CI_ALG_TYPE), $(USE_AEC_DENOISE_NN))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_aec_denoise_nn.lds
C_FLAGS += -DUSE_DENOISE_NN=1  -DUSE_VOX_MODULE=1 -DUSE_AEC_MODULE=1 -DHOST_MIC_USE_NUMBER=1 -DHOST_CODEC_CHA_NUM=2 
LIBS := -lasr_dis_deepse_v2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_aec_denoise_nn.a

else ifeq ($(CI_ALG_TYPE), $(USE_CWSL_AEC_DENOISE_NN))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_cwsl_aec_denoise_nn.lds
C_FLAGS += -DUSE_CWSL=1 -DUSE_DENOISE_NN=1  -DUSE_VOX_MODULE=1 -DUSE_AEC_MODULE=1 -DHOST_MIC_USE_NUMBER=1 -DHOST_CODEC_CHA_NUM=2
LIBS := -lasr_dis_deepse_v2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_cwsl_aec_denoise_nn.a

else ifeq ($(CI_ALG_TYPE), $(USE_ANY_MIC_AEC_DENOISE_NN))
ALG_PRO_LINK_LDS := ../lds/ci130x_alg_pro_any_mic_aec_denoise_nn.lds
C_FLAGS += -DUSE_DUAL_MIC_ANY=1 -DUSE_DENOISE_NN=1  -DUSE_VOX_MODULE=1 -DUSE_AEC_MODULE=1 -DOFFLINE_DUAL_MIC_ALG_SUPPORT=1 -DHOST_CODEC_CHA_NUM=2 -DHOST_MIC_USE_NUMBER=2 
LIBS := -lasr_dis_deepse_v2
LIB_BNPU_CORE_ALG_PRO := libbnpu_core_alg_pro_any_mic_aec_denoise_nn.a
else
$(error "error:请配置正确的CI_ALG_TYPE参数!!!")
endif


ifeq ($(USE_AUDIO_PLAYER_TYPE), 0)
	C_FLAGS += -DSIMPLE_AUDIO_PLAYER_ENABLE=1
else
	C_FLAGS += -DSIMPLE_AUDIO_PLAYER_ENABLE=0
endif
# ifeq ($(USE_PWK_ENABLE), 1)
# 	USE_DECODE_FRM := 1
# 	C_FLAGS += -DUSE_PWK=1
# else
	C_FLAGS += -DUSE_PWK=0
# endif
# ifeq ($(USE_ALC_AUTO_SWITCH_MODULE), 1)
# 	C_FLAGS += -DUSE_ALC_AUTO_SWITCH_MODULE=1
# else
	C_FLAGS += -DUSE_ALC_AUTO_SWITCH_MODULE=0
# endif
# ifeq ($(MULT_INTENT), 1)
	C_FLAGS += -DMULT_INTENT=1
# else
# 	C_FLAGS += -DMULT_INTENT=$(MULT_INTENT) -DDEFAULT_STOP_SILCNT=15 -DASR_FE_REDUCE_MEM=1 -DMAX_STOP_VAD_FRM=60
# #多意图需要使用帧长统计功能
# 	USE_DECODE_FRM := 1
# endif

ifeq ($(AIOT_AUDIO_COMPRESS_TYPE), 1)
LIBS += -lspeex
C_FLAGS += -I$(SDK_PATH)/components/cias_speex/include/speex -I$(SDK_PATH)/components/cias_speex/include/ogg -I$(SDK_PATH)/components/cias_speex/include -I$(SDK_PATH)/components/cias_speex/libspeex -I$(SDK_PATH)/components/cias_speex/port	   
else ifeq ($(AIOT_AUDIO_COMPRESS_TYPE), 2)    
LIBS += -lopus
C_FLAGS += -I$(SDK_PATH)/components/cias_opus/src/ -I$(SDK_PATH)/components/cias_opus/silk/float  -I$(SDK_PATH)/components/cias_opus/celt -I$(SDK_PATH)/components/cias_opus/include -I$(SDK_PATH)/projects/libopus/src -I$(SDK_PATH)/components/cias_opus/silk -I$(SDK_PATH)/components/cias_opus/silk/fixed/ 		   
else ifeq ($(AIOT_AUDIO_COMPRESS_TYPE), 3)
C_FLAGS += -DAIOT_AUDIO_COMPRESS_BY_G722=1
else ifeq ($(AIOT_AUDIO_COMPRESS_TYPE), 0)
else
$(error "error:请配置正确的AIOT_AUDIO_COMPRESS_TYPE参数!!!")
endif

ifeq ($(AUDIO_UPLOAD_TYPE), 1)
C_FLAGS += -DUSE_AUDIO_UPLOAD_BY_UART=1 -DUSE_AUDIO_UPLOAD_BY_HPOUT=0 -DUSE_AUDIO_UPLOAD_BY_IIS=0
else ifeq ($(AUDIO_UPLOAD_TYPE), 2)
C_FLAGS += -DUSE_AUDIO_UPLOAD_BY_HPOUT=1 -DUSE_AUDIO_UPLOAD_BY_UART=0 -DUSE_AUDIO_UPLOAD_BY_IIS=0
else ifeq ($(AUDIO_UPLOAD_TYPE), 3)
C_FLAGS += -DUSE_AUDIO_UPLOAD_BY_IIS=1 -DUSE_AUDIO_UPLOAD_BY_UART=0 -DUSE_AUDIO_UPLOAD_BY_HPOUT=0
else
$(error "error:请配置正确的AUDIO_UPLOAD_TYPE参数!!!")
endif

SECONDARY_FLASH := ../firmware/user_code/[0]code.bin 
SECONDARY_LIST := $(BUILD_DIR)/$(PROJECT_NAME).lst
LTO_OPTION = -flto

LIBS += -lm

C_FLAGS += -march=rv32imafc -mabi=ilp32f -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mfdiv $(O_OPTION) -fmessage-length=0 $(LTO_OPTION)
C_FLAGS += -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-delete-null-pointer-checks -fno-unroll-loops  -g -std=gnu11 -MMD -MP
C_FLAGS += -DCI_CONFIG_FILE=\"user_config.h\" -DCORE_ID=0
# enum only cost 1 byte
C_FLAGS += -fshort-enums
S_FLAGS := -march=rv32imafc -mabi=ilp32f -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mfdiv $(O_OPTION) -fmessage-length=0 $(LTO_OPTION)
S_FLAGS += -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-delete-null-pointer-checks -fno-unroll-loops  -g -x assembler-with-cpp -MMD -MP
S_FLAGS += -I$(SDK_PATH)/components -I$(SDK_PATH)/driver/ci130x_chip_driver/inc
LD_FLAGS := -march=rv32imafc -mabi=ilp32f -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mfdiv $(O_OPTION) -fmessage-length=0	$(LTO_OPTION)
LD_FLAGS += -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-delete-null-pointer-checks -fno-unroll-loops
LD_FLAGS += -g -T "$(ALG_PRO_LINK_LDS)"  -nostartfiles -Xlinker  --gc-sections -Wl,-Map,"$(BUILD_DIR)/$(PROJECT_NAME).map" --specs=nano.specs

LD_FLAGS += -Wl,--wrap,memcmp
LD_FLAGS += -Wl,--wrap,memcpy
LD_FLAGS += -Wl,--wrap,memmove
LD_FLAGS += -Wl,--wrap,memset
LD_FLAGS += -Wl,--wrap,strcat
LD_FLAGS += -Wl,--wrap,strcmp
LD_FLAGS += -Wl,--wrap,strcpy
LD_FLAGS += -Wl,--wrap,strlen
LD_FLAGS += -Wl,--wrap,strncat
LD_FLAGS += -Wl,--wrap,strncmp
LD_FLAGS += -Wl,--wrap,strncpy
LD_FLAGS += -Wl,--wrap,strstr
LD_FLAGS += -Wl,--wrap,stpcpy
LD_FLAGS += -Wl,--wrap,stpncpy
LD_FLAGS += -Wl,--wrap,strnlen
LD_FLAGS += -Wl,--wrap,strtoul
LD_FLAGS += -Wl,--wrap,atol
LD_FLAGS += -Wl,--wrap,qsort
LD_FLAGS += -Wl,--wrap,malloc
LD_FLAGS += -Wl,--wrap,free
LD_FLAGS += -Wl,--wrap,vsnprintf
LD_FLAGS += -Wl,--wrap,sprintf
LD_FLAGS += -Wl,--wrap,sscanf
LD_FLAGS += -Wl,--wrap,abs
LD_FLAGS += -Wl,--wrap,isnan
LD_FLAGS += -Wl,--wrap,isinf
LD_FLAGS += -Wl,--wrap,cosf
LD_FLAGS += -Wl,--wrap,sinf
LD_FLAGS += -Wl,--wrap,tanf
LD_FLAGS += -Wl,--wrap,expf
LD_FLAGS += -Wl,--wrap,sqrtf
LD_FLAGS += -Wl,--wrap,fabsf
LD_FLAGS += -Wl,--wrap,logf
LD_FLAGS += -Wl,--wrap,log10f
LD_FLAGS += -Wl,--wrap,atoi

LD_FLAGS += -L$(SDK_PATH)/$(LIBS_PATH) -L$(SDK_PATH)/components/freertos/portable/GCC
LD_FLAGS += -L$(SDK_PATH)/components/asr/decoder -L$(SDK_PATH)/utils

all: secondary-outputs

ifneq ($(MAKECMDGOALS), clean)
-include $(BUILD_DIR)/source_file.mk

ifeq ($(USE_DECODE_FRM), 1)
LIB_FILES += $(SDK_PATH)/$(LIBS_PATH)/libcikd_pro_cwsl_use_frm.a
LIBS += -lcikd_pro_cwsl_use_frm
else
LIB_FILES += $(SDK_PATH)/$(LIBS_PATH)/libcikd_pro_cwsl_dis_frm.a
LIBS += -lcikd_pro_cwsl_dis_frm
endif

endif




$(BUILD_DIR)/$(PROJECT_NAME).elf: $(OBJS) $(USER_OBJS) build/source_file.mk $(LIB_FILES) $(ALG_PRO_LINK_LDS)  makefile 
	@echo 'Building target: $@'
	@echo 'Inving: GNU RISC-V Cross C++ Linker'
	$(CC_PREFIX)$(LD) $(LD_FLAGS) -o "$(BUILD_DIR)/$(PROJECT_NAME).elf" $(OBJS) $(USER_OBJS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

$(SECONDARY_FLASH): $(BUILD_DIR)/$(PROJECT_NAME).elf
	@echo 'Inving: GNU RISC-V Cross Create Flash Image'
	@mkdir -p ../firmware/user_code && $(CC_PREFIX)$(OC) -O binary "$(BUILD_DIR)/$(PROJECT_NAME).elf"  $(SECONDARY_FLASH)
	@cp $(SDK_PATH)/$(LIBS_PATH)/$(LIB_BNPU_CORE_ALG_PRO) ../firmware/user_code/[1]code.bin
	@$(SDK_PATH)/tools/ci-tool-kit merge user-file -i ../firmware/user_code > /dev/null
	@rm ../firmware/user_code/[1]code.bin
	@echo 'Finished building: $@'
	@echo ' '
	
$(BUILD_DIR)/$(PROJECT_NAME).lst: $(BUILD_DIR)/$(PROJECT_NAME).elf
	@echo 'Inving: GNU RISC-V Cross Create Listing'
	$(CC_PREFIX)$(OD) --demangle --disassemble --reloc --wide "$(BUILD_DIR)/$(PROJECT_NAME).elf" > "$(BUILD_DIR)/$(PROJECT_NAME).lst"
	$(CC_PREFIX)$(SIZE) --format=berkeley "$(BUILD_DIR)/$(PROJECT_NAME).elf"
	@echo 'Finished building: $@'
	@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST)
	
	

include $(SDK_PATH)/utils/common_tail.mk

