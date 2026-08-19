#!/bin/bash 

BUILD_START_TIME=$(date +%y%m%d)

SOURCE_DIR=$(pwd)

BUILD_MAJ_VERSION=$1
BUILD_Min_VERSION=$2
BUILD_FIX_VERSION=$3
BUILD_NUM=$4
BUILD_TYPE=$5

if [ ! -n "$BUILD_NUM" ] 
then
echo do not set build number!
exit 1;
fi

if [ ! -n "$BUILD_TYPE" ]
then
echo do not set build type!
exit 1;
fi

if [ ! -n "BUILD_MAJ_VERSION" ]
then
echo do not set maj version!
exit 1;
fi

if [ ! -n "BUILD_Min_VERSION" ]
then
echo do not set min version!
exit 1;
fi

if [ ! -n "BUILD_FIX_VERSION" ]
then
echo do not set fix version!
exit 1;
fi

TVS_CORE_PATH=${SOURCE_DIR}

SDK_VERSION_INC_FILE=${TVS_CORE_PATH}/tvs_sdk/inc/tvs_version.h
rm -rf ${SDK_VERSION_INC_FILE}
echo "#ifndef __TVS_VERSION_H__" >> ${SDK_VERSION_INC_FILE}
echo "#define __TVS_VERSION_H__" >> ${SDK_VERSION_INC_FILE}
echo "#define TVS_BUILD_DATE  \"${BUILD_START_TIME}\"" >> ${SDK_VERSION_INC_FILE}
echo "#define TVS_BUILD_NUMBER  \"${BUILD_NUM}\"" >> ${SDK_VERSION_INC_FILE}
echo "#define TVS_BUILD_TYPE    \"${BUILD_TYPE}\"" >> ${SDK_VERSION_INC_FILE}
echo "#endif" >> ${SDK_VERSION_INC_FILE}

TVS_VERSION_STR=${BUILD_MAJ_VERSION}.${BUILD_Min_VERSION}.${BUILD_FIX_VERSION}.${BUILD_NUM}.${BUILD_TYPE}
echo "TVS Version: ${TVS_VERSION_STR}"

# Build TVS SDK

# 设置arm-none-eabi-gcc的路径
GCC_PATH=D:/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-gcc.exe

# 设置arm-none-eabi-g++的路径
GXX_PATH=D:/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-g++.exe

rm -rf ./build
mkdir -p ./build/xr871

cd ./build/xr871

cmake ../../ \
	-DCMAKE_TOOLCHAIN_FILE=../../toolchains/xr871/tvs.cmake \
	-DCMAKE_C_COMPILER=${GCC_PATH} \
	-DCMAKE_CXX_COMPILER=${GXX_PATH} \
	-G "Unix Makefiles"
make


echo "build finish"

# 拷贝so、头文件、adapter代码，生成压缩包
TARGET_FOLDER=${TVS_CORE_PATH}/bin
TARGET_DIR_NAME=tvs_sdk_${TVS_VERSION_STR}
TARGET_DIR=${TARGET_FOLDER}/${TARGET_DIR_NAME}

rm -rf ${TARGET_FOLDER}

mkdir ${TARGET_DIR}/tvs_sdk -p

mkdir ${TARGET_DIR}/tvs_sdk/tvs_libs -p

cp -R ${TVS_CORE_PATH}/tvs_adapter ${TARGET_DIR}/tvs_sdk/
cp -R ${TVS_CORE_PATH}/compatible/inc/tvs_api ${TARGET_DIR}/tvs_sdk/
cp -R ${TVS_CORE_PATH}/tvs_common/tvs_common_def.h ${TARGET_DIR}/tvs_sdk/tvs_api
cp -R ${TVS_CORE_PATH}/docs ${TARGET_DIR}/tvs_sdk/
cp -R ${TVS_CORE_PATH}/README.md ${TARGET_DIR}/tvs_sdk

mkdir ${TARGET_DIR}/tvs_sdk/tvs_libs/xr871 -p
cp ${TVS_CORE_PATH}/build/xr871/lib/* ${TARGET_DIR}/tvs_sdk/tvs_libs/xr871

cd ${TARGET_FOLDER}
tar -jcf ${TARGET_DIR_NAME}.tar.bz2 ${TARGET_DIR_NAME}
cd -

echo "create ${TARGET_DIR_NAME}.tar.bz2 finish"

