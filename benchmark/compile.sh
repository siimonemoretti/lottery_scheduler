#!/bin/bash

# Actually: compile and check for errors / warnings
if ! arm-linux-gnueabi-gcc -static -I ~/my-kernel-headers/include hello_sched.c -o hello_sched; then
   echo "Compilation failed. Exiting."
   exit 1
fi

# Compile dummy_program.c
if ! arm-linux-gnueabi-gcc -static -I ~/my-kernel-headers/include dummy_program.c -o dummy; then
   echo "Compilation failed. Exiting."
   exit 1
fi

# Compile dummy_launcher.c
if ! arm-linux-gnueabi-gcc -static -I ~/my-kernel-headers/include dummy_launcher.c -o dummy_launcher; then
   echo "Compilation failed. Exiting."
   exit 1
fi

# Then: 
find . -print0 | cpio --nul -ov --format=newc | gzip -9 > rootfs.cpio.gz