include(ProjModuleCollect.cmake)

set(BLE_USR_APP_PATH app/ble_usr_ln)   #蓝牙配网功能
set(USR_APP_PATH app/usr_ln)
set(TUYA_LINK_LIB  null)

option(CIAS_LN_HARDWARE_TEST_ENABLE     "cias ln hardware test enable"              0)
option(CIAS_AIOT_ENABLE                 "cias aiot fuction."                        0)
option(CIAS_BLE_CONFIG_ENABLE           "cias ble config function"                  0)
option(CIAS_IOT_CLOUD_HISENSE_ENABLE    "cias iot function for hisense"              0)
option(CIAS_IOT_CLOUD_ALI_ENABLE        "cias iot function for ali"                 0)
option(CIAS_IOT_TENCENT_ENABLE          "cias iot function for tencent"             0)
option(CIAS_IOT_TVS_ENABLE              "cias iot function for tvs"                 0)
option(CIAS_IOT_CLOUD_HUAWEI            "cias iot function for huawei"              0)
option(CIAS_IOT_TUYA_ENABLE             "cias iot function for tuya"                0)
option(CIAS_TUYA_IR_CTRL_ENABLE         "cias iot function for tuya ir ctrl"        0)
option(CIAS_IOT_CLOUD_XIAOMI_ENABLE     "cias iot function for xiaomi"              0)
option(CIAS_IOT_CLOUD_CI_ENABLE         "cias iot function for chipintelli"         0)
option(CIAS_AIOT_AUDIO_OTA_ENABLE       "cias chip audio ota function"              0)
option(CIAS_AIOT_WIFI_OTA_ENABLE        "cias chip wifi ota function"               0)
option(CIAS_SYSTEM_MANAGE_ENABLE        "cias system manage function"               0)
option(CIAS_AUDIO_SPEEX_ENABLE          "cias audio speex function"                 0)
option(CIAS_LAN_NETWORK_ENABLE          "cias lan network function"                 0)


set(CIAS_AIOT_ENABLE                    1)     #AIOT云功能
set(CIAS_LN_HARDWARE_TEST_ENABLE        0)     #ln硬件测试

if (CIAS_AIOT_ENABLE)
    set(CIAS_BLE_CONFIG_ENABLE          1)     #ble配网使能-涂鸦云使能不用单独使能CIAS_BLE_CONFIG_ENABLE，代码内部默认自动开启蓝牙配网功能 
    set(CIAS_IOT_CLOUD_HISENSE_ENABLE   0)     #海信IOT使能
    set(CIAS_IOT_CLOUD_HUAWEI           0)     #华为云使能
    set(CIAS_IOT_CLOUD_XIAOMI_ENABLE    0)     #小米云使能
    set(CIAS_IOT_CLOUD_CI_ENABLE        0)     #启英云使能
    set(CIAS_IOT_TENCENT_ENABLE         0)     #腾讯IOT云使能
    set(CIAS_IOT_TVS_ENABLE             1)     #腾讯TVS使能
    set(CIAS_IOT_TUYA_ENABLE            0)     #涂鸦云使能
    set(CIAS_IOT_CLOUD_ALI_ENABLE       0)     #阿里云使能,该功暂时禁止使用
    set(CIAS_AIOT_AUDIO_OTA_ENABLE      0)     #语音芯片OTA使能
    set(CIAS_AIOT_WIFI_OTA_ENABLE       0)     #WIFI芯片OTA使能
    set(CIAS_SYSTEM_MANAGE_ENABLE       0)     #系统管理任务使能
    set(CIAS_AUDIO_SPEEX_ENABLE         0)     #语音压缩功能-目前离在线功能speex压缩放在语音端，该功能不使能
    set(CIAS_LAN_NETWORK_ENABLE         0)     #局域网测试功能-测试用，不用使能
endif()

if(CIAS_IOT_TUYA_ENABLE) 
    set(CIAS_TUYA_IR_CTRL_ENABLE    1)        #涂鸦红外遥控器功能使能
endif()
configure_file(
    "${PROJECT_SOURCE_DIR}/Config.h.in"
    "${PROJECT_SOURCE_DIR}/Config.h"
)
include_directories(${PROJECT_SOURCE_DIR})


if(CIAS_LN_HARDWARE_TEST_ENABLE)   #硬件测试
    file(GLOB_RECURSE  LN_HARDWARE_TEST_SRC  app/cias_aiot_wifi/cias_test/*.c)
    set(CIAS_LN_HARDWARE_TEST_SRC
        ${LN_HARDWARE_TEST_SRC})
endif()

##################################  mbedtls  ####################################
if (NOT CIAS_IOT_TUYA_ENABLE)   #涂鸦使用自带的mebedtls库
    file(GLOB  MBEDTLS_SRC  ${COMP_MBEDTLS_DIR}/library/*.c)
    include_directories(${COMP_MBEDTLS_DIR}/include)
    include_directories(${COMP_MBEDTLS_DIR}/include/mbedtls)
    list(APPEND MODULE_SRC ${MBEDTLS_SRC})
    ###################################  mbedtls port ####################################
    file(GLOB  MBEDTLS_PORT_SRC  ${COMP_MBEDTLS_DIR}/port_ln/library/*.c)
    include_directories(${COMP_MBEDTLS_DIR}/port_ln/include)
    add_definitions(-DMBEDTLS_CONFIG_FILE="mbedtls_config.h")
    list(APPEND MODULE_SRC ${MBEDTLS_PORT_SRC})
endif()

if(CIAS_AIOT_ENABLE)
    file(GLOB_RECURSE  CIAS_AIOT_COMMON_SRC      app/cias_aiot_wifi/cias_common/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_ADAPTER_SRC     app/cias_aiot_wifi/cias_adapter/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_AUTH_SRC        app/cias_aiot_wifi/cias_auth/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_SYSTEM_SRC      app/cias_aiot_wifi/cias_system/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_KERNEL_SRC      app/cias_aiot_wifi/cias_kernel/freertos/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_MSG_HANDLE_SRC  app/cias_aiot_wifi/cias_msg_handle/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_MEDIA_SRC       app/cias_aiot_wifi/cias_media/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_PROTOCOL_SRC    app/cias_aiot_wifi/cias_protocol/cias_olcp/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_NET_HTTP_SRC    app/cias_aiot_wifi/cias_net/cias_http_interface/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_NET_AP_SRC      app/cias_aiot_wifi/cias_net/cias_ap_interface/*.c)
    file(GLOB_RECURSE  CIAS_AIOT_WIFI_PORT_SRC   app/cias_aiot_wifi/cias_wifi_port/*.c)
    set(CIAS_AIOT_SRC
        ${CIAS_AIOT_COMMON_SRC}
        ${CIAS_AIOT_AUTH_SRC}
        ${CIAS_AIOT_SYSTEM_SRC}
        ${CIAS_AIOT_KERNEL_SRC}
        ${CIAS_AIOT_MSG_HANDLE_SRC}
        ${CIAS_AIOT_MEDIA_SRC}
        ${CIAS_AIOT_PROTOCOL_SRC}
        ${CIAS_AIOT_NET_HTTP_SRC}
        ${CIAS_AIOT_NET_AP_SRC}
        ${CIAS_AIOT_WIFI_PORT_SRC}
        ${CIAS_AIOT_ADAPTER_SRC}
    )
endif()  
#for ble config
if(CIAS_BLE_CONFIG_ENABLE)
    file(GLOB_RECURSE  CIAS_BLE_CONFIG_PORT_SRC  app/cias_aiot_wifi/cias_ble_port/cias_ble_config.c)
    if(CIAS_IOT_CLOUD_HISENSE_ENABLE)  #支持海信app 蓝牙配网
        file(GLOB_RECURSE  CIAS_BLE_CONFIG_PORT_SRC  app/cias_aiot_wifi/cias_ble_port/hisense_ble_config.c)
    endif()
    set(CIAS_BLE_CONFIG_SRC
        ${CIAS_BLE_CONFIG_PORT_SRC}
    )
    
endif()
#for audio ota
if(CIAS_AIOT_AUDIO_OTA_ENABLE)
    file(GLOB_RECURSE  CIAS_AUIDO_OTA_SRC  app/cias_aiot_wifi/cias_ota/ota_audio/*.c)
    set(CIAS_AUDIO_OTA_SRC
        ${CIAS_AUIDO_OTA_SRC}
    )
endif()
#for wifi ota
if(CIAS_AIOT_WIFI_OTA_ENABLE)
#    file(GLOB_RECURSE  CIAS_WIFI_CMD_OTA_SRC  ../../components/ln_at_cmd/ota/*.c)
    file(GLOB_RECURSE  CIAS_WIFI_OTA_SUB_SRC  app/cias_aiot_wifi/cias_ota/ota_wifi/*.c)
    file(GLOB_RECURSE  CIAS_HTTP_CLIENT_SRC  ../../components/net/httpsclient/src/*.c)
    set(CIAS_WIFI_OTA_SRC
        ${CIAS_WIFI_OTA_SUB_SRC}
        ${CIAS_HTTP_CLIENT_SRC}
    )
endif()
#for cias audio speex
if(CIAS_AUDIO_SPEEX_ENABLE)
    file(GLOB_RECURSE  CIAS_SPEEX_LIB      app/cias_aiot_wifi/cias_algorithm/libspeex/*.c)
    file(GLOB_RECURSE  CIAS_SPEEX_PORT     app/cias_aiot_wifi/cias_algorithm/port/*.c)
    file(GLOB_RECURSE  CIAS_SPEEX_SUB_SRC  app/cias_aiot_wifi/cias_algorithm/src/*.c)
    file(GLOB_RECURSE  CIAS_SPEEX_SUB_SRC  app/cias_aiot_wifi/cias_algorithm/speex/libspeex/*.c)
    set(CIAS_AUDIO_SPEEX_SRC
        ${CIAS_SPEEX_LIB}
        ${CIAS_SPEEX_PORT}
        ${CIAS_SPEEX_SUB_SRC}
    )
endif()
#for tencent tvs
if(CIAS_IOT_TVS_ENABLE)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TVS_INTERFACE_SRC       app/cias_aiot_wifi/cias_cloud/cloud_tvs/cias_interface/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TVS_COMPATIBLE_SRC      app/cias_aiot_wifi/cias_cloud/cloud_tvs/compatible/src/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TVS_THIRD_MONGOOSE_SRC  app/cias_aiot_wifi/cias_cloud/cloud_tvs/third_party/src/mongoose/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TVS_THIRD_NETPING_SRC   app/cias_aiot_wifi/cias_cloud/cloud_tvs/third_party/src/net_ping/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TVS_SDK_SRC             app/cias_aiot_wifi/cias_cloud/cloud_tvs/tvs_sdk/src/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TVS_ADAPTER_SRC         app/cias_aiot_wifi/cias_cloud/cloud_tvs/tvs_adapter/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TVS_OS_SRC              app/cias_aiot_wifi/cias_cloud/cloud_tvs/os/freeRTOS/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TETST_SRC               app/cias_aiot_wifi/cias_cloud/cias_cloud_func_test/*.c)
    
    set(CIAS_IOT_TVS_SRC
        ${CIAS_IOT_CLOUD_TVS_INTERFACE_SRC}
        ${CIAS_IOT_CLOUD_TVS_COMPATIBLE_SRC}
        ${CIAS_IOT_CLOUD_TVS_THIRD_MONGOOSE_SRC}
        ${CIAS_IOT_CLOUD_TVS_THIRD_NETPING_SRC}
        ${CIAS_IOT_CLOUD_TVS_SDK_SRC}
        ${CIAS_IOT_CLOUD_TVS_ADAPTER_SRC}
        ${CIAS_IOT_CLOUD_TVS_OS_SRC}
       
        #${CIAS_IOT_CLOUD_TETST_SRC}
    )
endif()

#for hisense iot
if(CIAS_IOT_CLOUD_HISENSE_ENABLE)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_HISENSE_SRC       app/cias_aiot_wifi/cias_cloud/cloud_iot_hisense/*.c)
    set(CIAS_IOT_HISENSE_SRC
        ${CIAS_IOT_CLOUD_HISENSE_SRC}
    )
endif()

#for lan network test (player)
if(CIAS_LAN_NETWORK_ENABLE)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_local_SRC              app/cias_aiot_wifi/cias_cloud/cias_cloud_func_test/*.c)
    set(CIAS_LAN_NETWORK_SRC
        ${CIAS_IOT_CLOUD_local_SRC}
    )
endif()

#cias_iot_ali
if(CIAS_IOT_CLOUD_ALI_ENABLE)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_COMPONENTS_SRC           app/cias_aiot_wifi/cias_cloud/cloud_ali/components/data-model/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_CORE_SRC                 app/cias_aiot_wifi/cias_cloud/cloud_ali/core/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_CORE_SYSDEP_SRC          app/cias_aiot_wifi/cias_cloud/cloud_ali/core/sysdep/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_CORE_UTILS_SRC           app/cias_aiot_wifi/cias_cloud/cloud_ali/core/utils/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_DEMOS_SRC                app/cias_aiot_wifi/cias_cloud/cloud_ali/demos/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_EXTERNAL_SRC             app/cias_aiot_wifi/cias_cloud/cloud_ali/external/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_PORTFILES_AIOT_PORT_SRC  app/cias_aiot_wifi/cias_cloud/cloud_ali/portfiles/aiot_port/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_ALI_PORTFILES_SRC            app/cias_aiot_wifi/cias_cloud/cloud_ali/portfiles/*.c)
    set(CIAS_IOT_CLOUD_ALI_SRC
        ${CIAS_IOT_CLOUD_ALI_COMPONENTS_SRC}
        ${CIAS_IOT_CLOUD_ALI_CORE_SYSDEP_SRC}
        ${CIAS_IOT_CLOUD_ALI_CORE_UTILS_SRC}
        ${CIAS_IOT_CLOUD_ALI_DEMOS_SRC}
        ${CIAS_IOT_CLOUD_ALI_EXTERNAL_SRC}
        ${CIAS_IOT_CLOUD_ALI_PORTFILES_AIOT_PORT_SRC}
    )
 #for cias iot tencent
elseif(CIAS_IOT_TENCENT_ENABLE)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TENCENT_INTERFACE_SRC  app/cias_aiot_wifi/cias_cloud/cloud_iot_tencent/cias_qcloud_interface/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TENCENT_PLATFORM_SRC   app/cias_aiot_wifi/cias_cloud/cloud_iot_tencent/platform/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TENCENT_SDK_SRC        app/cias_aiot_wifi/cias_cloud/cloud_iot_tencent/sdk_src/*.c)
    set(CIAS_IOT_CLOUD_TENCENT_SRC
        ${CIAS_IOT_CLOUD_TENCENT_INTERFACE_SRC}
        ${CIAS_IOT_CLOUD_TENCENT_PLATFORM_SRC}
        ${CIAS_IOT_CLOUD_TENCENT_SDK_SRC}
    )
elseif(CIAS_IOT_CLOUD_HISENSE_ENABLE)
    set(BLE_USR_APP_PATH app/ble_usr_hisense)
    set(USR_APP_PATH app/usr_hisense)

elseif( CIAS_IOT_TUYA_ENABLE)
    # FIXME: to be adapted...
    set(APP_NAME        "cloud_tuya")
    set(PRODECT_KEY     "cacxn7yngnggmeq9")
    set(PRODUCT_VER     "1.0.0")
    # FIXME: to be adapted...
    # set(APP_NAME        "tuya_uart_common_demo")
    # set(PRODECT_KEY     "u8om0z8xjoazy904")
    # set(PRODUCT_VER     "1.0.0")

    #################  Global parameter parsed from app config, no manual change is required  ##########
    set(APP_PATH                ${CMAKE_CURRENT_SOURCE_DIR}/app/cias_aiot_wifi/cias_cloud/${APP_NAME})
    set(BLE_USR_APP_PATH app/ble_usr_tuya)
    set(USR_APP_PATH app/usr_tuya)
    set(PRODUCT_CFG_FILENAME    product_${PRODECT_KEY}_${PRODUCT_VER})
    set(PRODUCT_CFG_FILEPATH    ${APP_PATH}/${PRODUCT_CFG_FILENAME})
    set(PRODUCT_CFG_HEADER      ${APP_PATH}/apps/src/.compile_usr_cfg.h)

    string(TIMESTAMP COMPILE_TIME %Y-%m-%d_%H:%M:%S)
    set(ln_ty_sdk_build_time   ${COMPILE_TIME})

    message(STATUS   "current dir: ${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS   "ln_ty_sdk build time: ${COMPILE_TIME}")

   
    if(NOT EXISTS  ${APP_PATH})
    message(FATAL_ERROR  "ERROR: Unknown user app: ${APP_NAME}!!!")
    endif()
    configure_file(
        "${APP_PATH}/platforms/ln_ty_sdk_version.h.in"
        "${APP_PATH}/platforms/ln_ty_sdk_version.h"
        )
    function(PARSE_CONFIG_BIN_NAME  file_path  sw_ver bin_name)
    if(NOT  EXISTS  ${file_path})
        message(FATAL_ERROR  "ERROR: user config file does NOT exist!!!")
    endif()

    file(STRINGS  ${file_path}  file_content
            REGEX  "^.*$"
            ENCODING  "UTF-8"
            LIMIT_COUNT  5
    )
    set(result_list  ${file_content})
    list(POP_BACK  result_list  val)
    list(POP_BACK  result_list  val)
    set(${bin_name}  ${val}  PARENT_SCOPE)
    string(LENGTH  ${val}  bin_name_len)
    if (${bin_name_len}  EQUAL  0)
        message(FATAL_ERROR  "ERROR: parse bin name, please check product config file!!!")
    endif()

    list(POP_BACK  result_list  val)
    set(${sw_ver}   ${val}  PARENT_SCOPE)
    string(LENGTH  ${val}  sw_ver_len)
    if (${sw_ver_len}  LESS  5)
        message(FATAL_ERROR  "ERROR: parse sw ver, please check product config file!!!")
    endif()
endfunction()

    # .compile_usr_cfg.h
    function(USR_COMPILE_CFG_PREPARE  src_path  dst_path)
        message(STATUS "prepare: ${dst_path}")
        if(EXISTS  ${src_path})
            file(GENERATE OUTPUT ${dst_path}
                    INPUT ${src_path}
            )
        else()
            message(FATAL_ERROR  "ERROR: user config file can not be found: ${src_path}")
        endif()
    endfunction()

####################################################################################################
##########################################  main flow  #############################################
####################################################################################################

## 1. Parse config file.
## ----------------------

    if(EXISTS  ${PRODUCT_CFG_FILEPATH})
    PARSE_CONFIG_BIN_NAME(${PRODUCT_CFG_FILEPATH}  USER_SW_VER APP_BIN_NAME)
    message(STATUS "------>  USER_SW_VER = ${USER_SW_VER}")
    message(STATUS "------>  APP_BIN_NAME = ${APP_BIN_NAME}")

    USR_COMPILE_CFG_PREPARE(${PRODUCT_CFG_FILEPATH}  ${PRODUCT_CFG_HEADER})
    else()
    message(FATAL_ERROR  "ERROR: unknown user config: ${PRODUCT_CFG_FILENAME}")
    endif()
    add_compile_definitions(APP_BIN_NAME=\"${APP_BIN_NAME}\")
    add_compile_definitions(USER_SW_VER=\"${USER_SW_VER}\")
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TUYA_CIAS_APP_SRC              ${APP_PATH}/apps/src/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TUYA_CIAS_TUYA_PORT_SRC        ${APP_PATH}/cias_tuya_port/*.c)
    if(CIAS_TUYA_IR_CTRL_ENABLE)
        file(GLOB_RECURSE  CIAS_IOT_CLOUD_IR_CTRL_PROTCOL_SRC        ${APP_PATH}/cias_tuya_protocol/*.c)
    endif()
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TUYA_PLATFORM_TUYA_COMMON_SRC  ${APP_PATH}/platforms/tuya_common/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TUYA_OS_ADAPTER_SRC            ${APP_PATH}/platforms/tuya_os_adapter/src/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TUYA_OS_ADAPTER_DRIVER_SRC     ${APP_PATH}/platforms/tuya_os_adapter/src/driver/*.c)
    file(GLOB_RECURSE  CIAS_IOT_CLOUD_TUYA_OS_ADAPTER_SYSTEM_SRC     ${APP_PATH}/platforms/tuya_os_adapter/src/system/*.c)
    set(CIAS_IOT_CLOUD_TUYA_SRC
        ${CIAS_IOT_CLOUD_TUYA_CIAS_APP_SRC}
        ${CIAS_IOT_CLOUD_TUYA_CIAS_TUYA_PORT_SRC}
        ${CIAS_IOT_CLOUD_TUYA_PLATFORM_TUYA_COMMON_SRC}
        ${CIAS_IOT_CLOUD_TUYA_OS_ADAPTER_SRC}
        ${CIAS_IOT_CLOUD_TUYA_OS_ADAPTER_DRIVER_SRC}
        ${CIAS_IOT_CLOUD_TUYA_OS_ADAPTER_SYSTEM_SRC}
        ${CIAS_IOT_CLOUD_IR_CTRL_PROTCOL_SRC}
        )
endif()

file(GLOB_RECURSE  USR_APP_SRC  ${USR_APP_PATH}/*.c)
file(GLOB_RECURSE  BLE_USR_APP_SRC  ${BLE_USR_APP_PATH}/*.c)
file(GLOB_RECURSE  BLE_USR_APP_CALL_BACK_SRC  ${BLE_USR_APP_PATH}/app_callback/*.c)

set(PROJ_ALL_SRC
    bsp/serial_hw.c
    bsp/drv_adc_measure.c
    app/ate/ln_ty_ate.c  #and by yjd
    startup/startup_${CHIP_SERIAL}_gcc.c
    ${USR_APP_SRC}
    ${BLE_USR_APP_SRC}
    ${CIAS_BLE_CONFIG_SRC}
    ${MODULE_SRC}
    ${CIAS_AIOT_SRC}
    ${CIAS_AUDIO_OTA_SRC}
    ${CIAS_WIFI_OTA_SRC}
    ${CIAS_IOT_CLOUD_ALI_SRC}
    ${CIAS_IOT_CLOUD_TENCENT_SRC}
    ${CIAS_IOT_HISENSE_SRC}
    ${CIAS_IOT_TVS_SRC}
    ${CIAS_IOT_CLOUD_TUYA_SRC}
    ${CIAS_LN_HARDWARE_TEST_SRC}
    ${CIAS_AUDIO_SPEEX_SRC}
    ${CIAS_LAN_NETWORK_SRC}
)

set(COMP_DHCPD_SUPPORT          ON  PARENT_SCOPE)
set(COMP_LWIP_SUPPORT           ON  PARENT_SCOPE)

#----------------------------   project output elf   ---------------------------
set(TARGET_ELF_NAME                         ${USER_PROJECT})
set(pro_executable_target                   ${TARGET_ELF_NAME}.elf)
add_executable(${pro_executable_target}     ${PROJ_ALL_SRC})

if(CIAS_IOT_TUYA_ENABLE)
target_link_libraries(${pro_executable_target}
    PUBLIC
    ${CHIP_SERIAL}_ble_full_stack
    ln::dhcpd
    lwip
    ${CHIP_SERIAL}_wifi
    -lc -lm -lnosys 
    tuya_iot
    PRIVATE
    -T${LINKER_SCRIPT}
    ${EXTRA_LINK_FLAGS}
)
target_link_directories(${pro_executable_target}
    PRIVATE
    ${LN_SDK_ROOT}/lib/gcclib
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/sdk/lib
)
else()
target_link_libraries(${pro_executable_target}
    PUBLIC
    ${CHIP_SERIAL}_ble_full_stack
    ln::dhcpd
    lwip
    ${CHIP_SERIAL}_wifi
    -lc -lm -lnosys 
    PRIVATE
    -T${LINKER_SCRIPT}
    ${EXTRA_LINK_FLAGS}
)
target_link_directories(${pro_executable_target}
    PRIVATE
    ${LN_SDK_ROOT}/lib/gcclib
)
endif()



target_include_directories(${pro_executable_target}
    PRIVATE
    app
    bsp
    cfg
    app/ate
    ${BLE_USR_APP_PATH}/app_callback/
    ${BLE_USR_APP_PATH}/
    ${USR_APP_PATH}/
if(CIAS_LN_HARDWARE_TEST_ENABLE)
    app/cias_aiot_wifi/cias_test/
endif
if(CIAS_AIOT_ENABLE)
    app/cias_aiot_wifi/cias_auth/
    app/cias_aiot_wifi/cias_common/
    app/cias_aiot_wifi/cias_system/
    app/cias_aiot_wifi/cias_adapter/
    app/cias_aiot_wifi/cias_net/cias_http_interface/
    app/cias_aiot_wifi/cias_net/cias_ap_interface/
    app/cias_aiot_wifi/cias_msg_handle/
    app/cias_aiot_wifi/cias_media/
    app/cias_aiot_wifi/cias_kernel/freertos/
    app/cias_aiot_wifi/cias_protocol/cias_olcp/  
    app/cias_aiot_wifi/cias_user_cfg/
    app/cias_aiot_wifi/cias_log/
    app/cias_aiot_wifi/cias_wifi_port/
endif()
#for cias ble config
if(CIAS_BLE_CONFIG_ENABLE)
    app/cias_aiot_wifi/cias_ble_port/
endif()
#audio ota
if(CIAS_AIOT_AUDIO_OTA_ENABLE)
    app/cias_aiot_wifi/cias_ota/ota_audio/
endif()
#wifi ota
if(CIAS_AIOT_WIFI_OTA_ENABLE)
    app/cias_aiot_wifi/cias_ota/ota_wifi
    ${LN_SDK_ROOT}/components/net/httpsclient/inc/
endif()
#audio speex
if(CIAS_AUDIO_SPEEX_ENABLE)
    app/cias_aiot_wifi/cias_algorithm/speex/include/
    app/cias_aiot_wifi/cias_algorithm/speex/include/ogg
    app/cias_aiot_wifi/cias_algorithm/speex/include/speex
    app/cias_aiot_wifi/cias_algorithm/speex/libspeex
    app/cias_aiot_wifi/cias_algorithm/speex/port
endif()
#cias iot ali
if(CIAS_IOT_CLOUD_ALI_ENABLE)
    app/cias_aiot_wifi/cias_cloud/cloud_ali/components/data-model/
    app/cias_aiot_wifi/cias_cloud/cloud_ali/core/
    app/cias_aiot_wifi/cias_cloud/cloud_ali/core/sysdep/
    app/cias_aiot_wifi/cias_cloud/cloud_ali/core/utils/
    app/cias_aiot_wifi/cias_cloud/cloud_ali/demos/
    app/cias_aiot_wifi/cias_cloud/cloud_ali/external/
    app/cias_aiot_wifi/cias_cloud/cloud_ali/portfiles/
    app/cias_aiot_wifi/cias_cloud/cloud_ali/portfiles/aiot_port/
endif()
#cias iot tencent
if(CIAS_IOT_TENCENT_ENABLE)
    app/cias_aiot_wifi/cias_cloud/cloud_iot_tencent/cias_qcloud_interface/inc
    app/cias_aiot_wifi/cias_cloud/cloud_iot_tencent/include
    app/cias_aiot_wifi/cias_cloud/cloud_iot_tencent/include/exports
    app/cias_aiot_wifi/cias_cloud/cloud_iot_tencent/sdk_src/internal_inc
endif()

#cias lan network test(player)
if(CIAS_LAN_NETWORK_ENABLE)
    app/cias_aiot_wifi/cias_cloud/cias_cloud_func_test
endif()

#cias iot hisense
if(CIAS_IOT_CLOUD_HISENSE_ENABLE)
    app/cias_aiot_wifi/cias_cloud/cloud_iot_hisense
endif()

if(CIAS_IOT_TVS_ENABLE)
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/cias_interface
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/third_party/inc/mongoose
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/third_party/inc/net_ping
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/third_party/
    app/cias_aiot_wifi/cias_cloud/cloud_tvs//tvs_sdk/inc
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/tvs_sdk/tvs_sdk_api
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/tvs_common
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/tvs_adapter
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/compatible/inc
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/compatible/inc/tvs_api
    app/cias_aiot_wifi/cias_cloud/cloud_tvs/os/inc
    
    #app/cias_aiot_wifi/cias_cloud/cias_cloud_func_test
endif()
#cias iot tuya
if(CIAS_IOT_TUYA_ENABLE)
    #app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/lwip-2.1.3/src/include
    #app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/lwip-2.1.3/src/port/ln_osal/include
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/apps/include/
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/tuya_common/include
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/tuya_common/include/driver
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/tuya_os_adapter/include
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/tuya_os_adapter/include/driver
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/tuya_os_adapter/include/system
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/platforms/tuya_adapter_test/include
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/sdk/include
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/sdk/include/mbedtls/
    app/cias_aiot_wifi/cias_cloud/cloud_tuya/cias_tuya_port
    app/usr_tuya/
    if(CIAS_TUYA_IR_CTRL_ENABLE)
        app/cias_aiot_wifi/cias_cloud/cloud_tuya/cias_tuya_protocol/
    endif()
endif()
)



include(gcc/gcc-custom-build-stage.cmake)
