CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(tvscore)

SET(LIBRARY_OUTPUT_PATH ${TVS_PROJECT_LIB_DIR})

SET(tvs_sdk_path ../)

# 核心库源码
AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/tvs_sdk/src tvssdk_src)

# TVS API适配模块源码
AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/compatible/src tvscompatible_src)

# http库源码
AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/third_party/src/mongoose tvshttp_src)

# ping源码
AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/third_party/src/net_ping tvsping_src)

# 按照TVS_PLATFORM_TYPE指定的平台类型，指定os wrapper模块路径
if ("${TVS_PLATFORM_TYPE}" STREQUAL "TVS_LINUX")
	AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/os/linux tvs_os_src)
elseif ("${TVS_PLATFORM_TYPE}" STREQUAL "TVS_FREERTOS")
	AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/os/freeRTOS tvs_os_src)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_FREERTOS")
elseif ("${TVS_PLATFORM_TYPE}" STREQUAL "TVS_RTTHREAD")
	AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/os/rt-thread tvs_os_src)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTVS_CONFIG_OS_USE_DEFAULT_MALLOC=0 -DPLATFORM_RT_THREAD")
else ()
	AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/os/linux tvs_os_src)
endif()

# SDK需要的头文件库路径
INCLUDE_DIRECTORIES(${tvs_sdk_path}/tvs_sdk/inc)
INCLUDE_DIRECTORIES(${tvs_sdk_path}/tvs_common)
INCLUDE_DIRECTORIES(${tvs_sdk_path}/tvs_sdk/tvs_sdk_api)
INCLUDE_DIRECTORIES(${tvs_sdk_path}/compatible/inc)
INCLUDE_DIRECTORIES(${tvs_sdk_path}/os/inc)
INCLUDE_DIRECTORIES(${tvs_sdk_path}/third_party/inc)
INCLUDE_DIRECTORIES(${tvs_sdk_path}/third_party/inc/mongoose)
INCLUDE_DIRECTORIES(${tvs_sdk_path}/third_party/inc/net_ping)

#编译静态库libtvscore.a
ADD_LIBRARY(${PROJECT_NAME} STATIC
		${tvssdk_src}
		${tvscompatible_src} 
		${tvshttp_src}
		${tvs_os_src}
		${tvsping_src})
		
