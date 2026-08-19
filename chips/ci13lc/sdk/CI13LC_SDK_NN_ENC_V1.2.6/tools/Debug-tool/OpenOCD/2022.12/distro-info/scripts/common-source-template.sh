# -----------------------------------------------------------------------------
# This file is part of the xPack distribution.
#   (https://xpack.github.io)
# Copyright (c) 2019 Liviu Ionescu.
#
# Permission to use, copy, modify, and/or distribute this software 
# for any purpose is hereby granted, under the terms of the MIT license.
# -----------------------------------------------------------------------------

# Helper script used in the second edition of the xPack build 
# scripts. As the name implies, it should contain only functions and 
# should be included with 'source' by the build scripts (both native
# and container).

# -----------------------------------------------------------------------------

# Use this as openocd source code definition template
# Create your own scripts/common-source.sh
# -----------------------------------------------------------------------------
OPENOCD_PROJECT_NAME="openocd"
OPENOCD_VERSION="0.11.0"
OPENOCD_GIT_BRANCH="nuclei-develop"
OPENOCD_GIT_COMMIT=""
README_OUT_FILE_NAME="README-Changelog.md"
OPENOCD_GIT_URL="https://github.com/riscv-mcu/riscv-openocd"
OPENOCD_SRC_FOLDER_NAME="${OPENOCD_PROJECT_NAME}.git"
