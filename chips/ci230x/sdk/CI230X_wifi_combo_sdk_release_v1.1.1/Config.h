/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-05-10 19:17:55
 * @LastEditTime: 2022-05-24 18:18:24
 * @Description: 
 */
/**
 * This is the configure demo  
 *    - CMAKEDEFINE_VAR1 = 
 *    - CMAKEDEFINE_VAR2 = 
 *    - DEFINE_VAR1      = 
 *    - DEFINE_VAR2      = 
 */

/**
 *  cmakedefine 会根据变量的值是否为真（类似 if）来变换为 #define VAR ... 或  #undef VAR 
 */
/* #undef CMAKEDEFINE_VAR1 */
/* #undef CMAKEDEFINE_VAR2 */

/**
 * define 会直接根据规则来替换
 */

#define CIAS_LN_HARDWARE_TEST_ENABLE  0
#define CIAS_BLE_CONFIG_ENABLE        1
#define CIAS_IOT_CLOUD_HISENSE_ENABLE 0
#define CIAS_IOT_CLOUD_HUAWEI         0
#define CIAS_IOT_CLOUD_XIAOMI_ENABLE  0
#define CIAS_IOT_CLOUD_CI_ENABLE      0
#define CIAS_IOT_TENCENT_ENABLE       0
#define CIAS_IOT_TVS_ENABLE           1
#define CIAS_IOT_TUYA_ENABLE          0
#define CIAS_IOT_CLOUD_ALI_ENABLE     0
#define CIAS_AIOT_AUDIO_OTA_ENABLE    0
#define CIAS_AIOT_WIFI_OTA_ENABLE     0
#define CIAS_TUYA_IR_CTRL_ENABLE      OFF
#define CIAS_AUDIO_SPEEX_ENABLE       0
#define CIAS_SYSTEM_MANAGE_ENABLE     0


