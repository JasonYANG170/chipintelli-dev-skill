

ifndef PROJECT_NAME
PROJECT_NAME := $(word $(words $(subst /, ,$(dir $(abspath .)))), $(subst /, ,$(dir $(abspath .))))
endif

PROJECT_PATH := ..

ifeq ($(OS),Windows_NT)
CMDBOX := busybox
LIBS_PATH := libs
else
CMDBOX :=
LIBS_PATH := libs/linux
endif

USER_OBJS := 
C_SRCS :=

# CC_PREFIX := riscv-none-embed-
CC_PREFIX := riscv-nuclei-elf-
CC = gcc
AS = gcc
AR = gcc-ar
LD = g++
OD = objdump
OC = objcopy
LUA = $(SDK_PATH)/tools/build-tools/bin/lua
SIZE = size
OBJS := 

C_FLAGS := -DPROJECT_NAME=\"$(PROJECT_NAME)\"
S_FLAGS := -I../src
LD_FLAGS := 

LTO_OPTION :=
O_OPTION := -Os

C_FLAGS += -D_XIF_="__attribute__((section(\".text_in_flash\")))"
C_FLAGS += -D_DIF_="__attribute__((section(\".rodata_in_flash\")))"
C_FLAGS += -D_NOINLINE_="__attribute__((noinline))"





