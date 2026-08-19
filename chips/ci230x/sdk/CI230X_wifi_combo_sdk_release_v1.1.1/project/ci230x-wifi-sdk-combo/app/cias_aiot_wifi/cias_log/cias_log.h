/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-05-10 19:17:12
 * @LastEditTime: 2022-05-23 14:23:32
 * @Description: 
 */
/**
 * @file cias_log.h
 * @brief 打印日志
 * @author JasonChan (x@chencc.cc)
 * @date 2021-04-18
 * @copyright Copyright (c) 2021
 */
#ifndef _CIAS_LOG_H_
#define _CIAS_LOG_H_
#include "FreeRTOS.h"

#include "cias_user_config.h"
#include "log.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define     SOCKET_LOG      "[SOCKET]"
#define     HTTP_LOG        "[ HTTP ]"
#define     OTA_LOG         "[ OTA  ]"

typedef enum {
    eCIAS_LOG_DISABLE = 0,
    eCIAS_LOG_ERROR,
    eCIAS_LOG_WARN,
    eCIAS_LOG_HIGHLIGHT,
    eCIAS_LOG_INFO,
    eCIAS_LOG_DEBUG
} CIAS_LOG_LEVEL;


/**
 * 配置适配接口
*/
#define CIAS_LOG_LEVEL_DEFINE eCIAS_LOG_DEBUG
#define CIAS_LOG_AGENT(fmt,...)      LOG(LOG_LVL_INFO, fmt, ##__VA_ARGS__);



#define CIAS_LOG_HL(fmt, ...)         if(CIAS_LOG_LEVEL_DEFINE >= eCIAS_LOG_HIGHLIGHT){CIAS_LOG_AGENT("\033[1;32m"fmt"\033[0;39m\r\n", ##__VA_ARGS__);}
#define CIAS_LOG_ERR(fmt, ...)        if(CIAS_LOG_LEVEL_DEFINE >= eCIAS_LOG_ERROR    ){CIAS_LOG_AGENT("\033[31m"fmt"\033[0m\r\n", ##__VA_ARGS__);}
#define CIAS_LOG_WARN(fmt, ...)       if(CIAS_LOG_LEVEL_DEFINE >= eCIAS_LOG_WARN     ){CIAS_LOG_AGENT("\033[33m"fmt"\033[0m\r\n", ##__VA_ARGS__);}
#define CIAS_LOG_INFO(fmt, ...)       if(CIAS_LOG_LEVEL_DEFINE >= eCIAS_LOG_INFO     ){CIAS_LOG_AGENT(fmt"\r\n", ##__VA_ARGS__);}
#define CIAS_LOG_DEBUG(fmt, ...)      if(CIAS_LOG_LEVEL_DEFINE >= eCIAS_LOG_DEBUG    ){CIAS_LOG_AGENT("\033[36m"fmt"\033[0m\r\n", ##__VA_ARGS__);}
#define CIAS_LOG_DEBUG2(fmt, ...)     if(CIAS_LOG_LEVEL_DEFINE >= eCIAS_LOG_DEBUG    ){CIAS_LOG_AGENT(fmt, ##__VA_ARGS__);}

#define CIAS_ASSERT(EX)               if(!(EX)){CIAS_LOG_AGENT("\r\n\033[31m[%s, %d]: ("#EX") assert failed\033[0m\n\r\n",__FILE__,__LINE__);}

#define CIAS_ASSERT_GOFAIL(EX)        do{\
                                        if (!(EX)){\
                                            CIAS_LOG_AGENT("\r\n\033[31m[%s, %d]: ("#EX")failed\033[0m\n\r\n",__FILE__,__LINE__);\
                                            goto __fail;\
                                        }\
                                    }while(0)       

#define CIAS_ASSERT_RETURN(EX)        do{\
                                        if (!(EX)){\
                                            CIAS_LOG_AGENT("\r\n\033[31m[%s, %d]: ("#EX")failed\033[0m\n\r\n",__FILE__,__LINE__);\
                                            return -1;\
                                        }\
                                    }while(0)       




#ifdef __cplusplus
}
#endif

#endif


