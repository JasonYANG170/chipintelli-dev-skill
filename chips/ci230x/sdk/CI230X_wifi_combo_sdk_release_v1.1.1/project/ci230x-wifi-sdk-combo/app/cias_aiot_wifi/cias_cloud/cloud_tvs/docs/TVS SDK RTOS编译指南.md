## TVS SDK RTOS编译指南

以XR871为例：

### 1.1 开发环境选择 

Windows 环境：MinW + cmake 
Linux 环境：Ubuntu14.2 以上 + cmake



### 1.2 选择 Toolchain 

Toolchain: gcc-arm-none-eabi-4_9-2015q2 
- Windows 版本 
https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q2-update/+download/gcc-arm-none-eabi-4_9-2015q2-20150609-win32.zip 
- Linux 版本 
https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q2-update/+download/gcc-arm-none-eabi-4_9-2015q2-20150609-linux.tar.bz2 



### 1.3 下载XR871 SDK

XR871 SDK在github上：

https://github.com/XradioTech/XR871SDK.git



### 1.4 下载TVS SDK RTOS源码

下载TVS SDK RTOS源码，并按照如下路径存放XR871 SDK和TVS SDK RTOS源码：

```
|-- XR871SDK
|-- tvs_sdk_rtos
```



### 1.5 编写toolchain file

编写xr871平台对应的toolchain file，创建tvs_sdk_rtos/toolchains/xr871/tvs.cmake，在文件中加入：

```
SET(CMAKE_SYSTEM_NAME Generic)

# 设置平台类型为freeRTOS
SET(TVS_PLATFORM_TYPE "TVS_FREERTOS")

set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

# 设置XR871 SDK的头文件路径
set(PLATFORM_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../XR871SDK/include)

INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE_PATH})

# lwip和mbedtls等库的头文件路径
INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE_PATH}/net)
INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE_PATH}/net/lwip-1.4.1)
INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE_PATH}/net/lwip-1.4.1/ipv4)

# cjson库的头文件
INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE_PATH}/cjson)

# XR871 SDK中没有speex的源码，引用SDK自带的speex库
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/third_party/inc/speex)

# freeRTOS的头文件路径
INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE_PATH}/kernel/FreeRTOS)
INCLUDE_DIRECTORIES(${PLATFORM_INCLUDE_PATH}/kernel/FreeRTOS/portable/GCC/ARM_CM4F)

# xr871 SDK需要的编译参数，从SDK编译脚本中获取
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -gdwarf-2")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -MMD -MP")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -DNDEBUG -DCONFIG_HAS_MBED_SSL_CONF=1 -D__CONFIG_OS_FREERTOS")

```



### 1.6 CMakefiles.txt

TVS SDK RTOS的cmake脚本在tvs_sdk_rtos/tvs_sdk目录中：
```
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(tvscore)

SET(LIBRARY_OUTPUT_PATH ${TVS_PROJECT_LIB_DIR})

SET(tvs_sdk_path ../)

# 核心库源码
AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/tvs_sdk/src tvssdk_src)

# TVS API适配模块源码
AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/compatible/src tvscompatible_src)

# http库源码
AUX_SOURCE_DIRECTORY(${tvs_sdk_path}/third_party/src/mongoose tvsthird_party_src)

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

#编译静态库libtvscore.a
ADD_LIBRARY(${PROJECT_NAME} STATIC
		${tvssdk_src}
		${tvscompatible_src} 
		${tvsthird_party_src}
		${tvs_os_src
		${tvsping_src})

```



### 1.7 编译源码

以windows为例，解压gcc-arm-none-eabi到d:\gcc-arm-none-eabi-4_9-2015q2 目录；

指定arm-none-eabi-gcc和arm-none-eabi-g++的路径；

执行cmake命令成功后，执行make命令进行编译；

可以编写build_xr871.sh：
```
#!/bin/bash 

mkdir -p out/xr871

cd out/xr871
rm * -rf

# 设置arm-none-eabi-gcc的路径
GCC_PATH=D:/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-gcc.exe

# 设置arm-none-eabi-g++的路径
GXX_PATH=D:/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-g++.exe

cmake ../../ \
	-DCMAKE_TOOLCHAIN_FILE=../../toolchains/xr871/tvs.cmake \
	-DCMAKE_C_COMPILER=${GCC_PATH} \
	-DCMAKE_CXX_COMPILER=${GXX_PATH} \
	-G "Unix Makefiles"

make

```

最终生成的静态库为out/xr871/libs/libtvscore.a



### 1.8 集成TVS SDK RTOS

创建一个访问腾讯云小微语音服务的工程；

将tvs_sdk_core/compatible/inc下的tvs_api目录，以及tvs_sdk_core/tvs_common目录拷贝到目标工程中，并在工程的Makefile中指定依赖这两个目录下的所有头文件；

将libtvscore.a拷贝到目标工程中，并在工程的Makefile中指定依赖这个静态库；

下面是一个项目文件夹的示例：

```
|-- tvs_sdk
|		|-- tvs_api
|				|-- tvs_api.h
|				|-- tvs_alert_adapter.h
|				|-- tvs_authorize.h
|				|-- tvs_platform_adapter.h
|				|-- tvs_media_player_adapter.h
|		|-- tvs_common
|				|-- tvs_common_def.h
|		|-- lib |      
|				|-- libtvscore.a 
|-- tvs_adapter
|		|-- tvs_api_impl.c
|		|-- tvs_mediaplayer_impl.c
|		|-- ...
|-- tvs_app
|		|-- main.c
|		|-- ...
|-- Makefile
|-- ...
```

TVS SDK需要依赖cjson、mbedtls、lwip和speex库，XR871 SDK中没有speex库，可以自行编写编译脚本，将tvs_sdk_core/third_party/libspeex编译为静态库，加入到目标项目的Makefile中。

为了节省内存，speex库在编译的时候可以增加如下参数：

```
CC_FLAGS += -DFIXED_POINT -DEXPORT="" -DMAX_CHARS_PER_FRAME=100 -DNB_ENC_STACK=10000 -UHAVE_CONFIG_H -Wno-error=unused-but-set-variable
```

