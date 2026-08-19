set(COMP_TUYA_COMMON_TOP_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/tuya_common)
set(COMP_TUYA_OS_ADAPTER_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/tuya_os_adapter)
# set(COMP_TUYA_ADAPTER_TEST_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/tuya_adapter_test)
set(COMP_LWIP_DIR               ${CMAKE_CURRENT_SOURCE_DIR}/lwip-2.1.3/src)

add_subdirectory(lwip-2.1.3/src)
add_subdirectory(tuya_common)
add_subdirectory(tuya_os_adapter)
# add_subdirectory(tuya_adapter_test)