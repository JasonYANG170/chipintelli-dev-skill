/**
 * @file   cias_freertos_debug.h
 * @author zhuo.liu@chipintelli.com
 * Copyright (C) 2020 Chipintelli Technology Co., Ltd. All rights reserved.
 */

#ifndef _CIAS_FREERTOS_DEBUG_H_
#define _CIAS_FREERTOS_DEBUG_H_

#include <stdio.h>

#include "cias_log.h"

#ifdef __cplusplus
    extern "C"{
#endif

#define CIAS_PRINT_INFO_ON  1
#define CIAS_PRINT_DEBUG_ON 1
#define CIAS_PRINT_WRAN_ON  1
#define CIAS_PRINT_ERR_ON   1

#define CIAS_LOG(flags, ...)  \
    do {                            \
        if(flags)                  \
            CIAS_LOG_HL(__VA_ARGS__);  \
            CIAS_LOG_HL("\r\n");       \
    } while (0)

#define CIAS_PRINT_INFO(...)     CIAS_LOG(CIAS_PRINT_INFO_ON,"[cias_info]"__VA_ARGS__)   
#define CIAS_PRINT_DEBUG(...)    CIAS_LOG(CIAS_PRINT_DEBUG_ON,"[cias_debug]"__VA_ARGS__)
#define CIAS_PRINT_WARN(...)     CIAS_LOG(CIAS_PRINT_WRAN_ON,"[cias_warn]"__VA_ARGS__)
#define CIAS_PRINT_ERR(...)      CIAS_LOG(CIAS_PRINT_ERR_ON,"[cias_err]%s:%d,"__VA_ARGS__,__func__, __LINE__)

#ifdef __cplusplus
    }
#endif

#endif
