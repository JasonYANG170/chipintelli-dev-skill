file(GLOB_RECURSE  TUYA_OS_ADAPTER_SRC  ${CMAKE_CURRENT_SOURCE_DIR}  "*.c")

set(tuya_os_adapter_static_target "tuya_os_adapter")
add_library(${tuya_os_adapter_static_target} STATIC ${TUYA_OS_ADAPTER_SRC})
add_library(tuya::os_adapter ALIAS ${tuya_os_adapter_static_target})

target_link_libraries(${tuya_os_adapter_static_target}
    PUBLIC
    lwip
    ${CHIP_SERIAL}_wifi
)

target_include_directories(${tuya_os_adapter_static_target}
    PUBLIC
    ../../${PLATFORM_TYPE}-sdk/components/ble/
    ../../${PLATFORM_TYPE}-sdk/components/ble/ble_arch/
    ../../${PLATFORM_TYPE}-sdk/components/ble/ble_api/common
    ../../${PLATFORM_TYPE}-sdk/components/ble/ble_api/gap_gatt/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/ble_lib_import
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/ble/hl/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/ble/hl/inc
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/ble/ll/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/ble/ll/import
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/ble/ll/src
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/ble/ll/src/llm
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/em/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/hci/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/sch/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/mac/sch/import
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/aes/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/common/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/dbg/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/ecc_p256/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/h4tl/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/ke/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/lib_ver/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/nvds/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/rf/api
    ../../${PLATFORM_TYPE}-sdk/components/ble/modules/rwip/api
    ../../${PLATFORM_TYPE}-sdk/mcu/driver_ln882h
)
