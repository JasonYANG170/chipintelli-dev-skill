#!/bin/bash 

mkdir -p out/xr871

cd out/xr871
rm * -rf

# 设置arm-none-eabi-gcc的路径
GCC_PATH=~/tools/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-gcc.exe

# 设置arm-none-eabi-g++的路径
GXX_PATH=~/tools/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi-g++.exe

cmake ../../ \
	-DCMAKE_TOOLCHAIN_FILE=../../toolchains/xr871/tvs.cmake \
	-DCMAKE_C_COMPILER=${GCC_PATH} \
	-DCMAKE_CXX_COMPILER=${GXX_PATH} \
	-G "Unix Makefiles"

make