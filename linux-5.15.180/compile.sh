#!/bin/bash

# This script compiles the Linux kernel for the ARM architecture 
# using the versatile_defconfig and installs headers.

set -e  # Exit immediately if any command fails

# Step 1: Clean the kernel source tree to ensure a pristine build environment
make ARCH=arm mrproper

# Step 2: Set the default kernel configuration (for the Versatile platform)
make O=~/linux_build/ ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- versatile_defconfig

# Step 3: Build the kernel with the specified configuration
echo "Building kernel..."
if ! sudo make -j$(nproc) O=~/linux_build/ ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-; then
    echo "Kernel build failed. Exiting."
    exit 1
fi

# Step 4: Install the kernel headers (suppressing output)
echo " ==> Installing kernel headers (output is suppressed)..."
make -s ARCH=arm headers_install INSTALL_HDR_PATH=~/my-kernel-headers