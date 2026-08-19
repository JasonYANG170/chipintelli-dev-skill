#!/usr/bin/env bash
rm -rf "${HOME}/Downloads/openocd-xpack.git"
git clone --branch xpack_nuclei_next --recurse-submodules https://github.com/riscv-mcu/openocd-xpack.git "${HOME}/Downloads/openocd-xpack.git"
