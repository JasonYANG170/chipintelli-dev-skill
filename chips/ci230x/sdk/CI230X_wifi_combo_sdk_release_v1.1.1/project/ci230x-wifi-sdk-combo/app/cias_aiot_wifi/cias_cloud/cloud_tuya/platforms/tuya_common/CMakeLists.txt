file(GLOB_RECURSE  TUYA_COMMON_SRC  ${CMAKE_CURRENT_SOURCE_DIR}  "*.c")

set(tuya_common_static_target  "tuya_common")
add_library(${tuya_common_static_target}  STATIC  ${TUYA_COMMON_SRC})
add_library(tuya::common  ALIAS  ${tuya_common_static_target})
