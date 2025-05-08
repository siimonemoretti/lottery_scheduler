#!/bin/bash

# Script Name: script.bash
# Description: Launch qemu-system-arm with ...
# Author: [Simone Moretti]

# First off: try to locate the zImage, the versatile-pb.dtb and the rootfs.cpio.gz using the find command

# Find the zImage starting from root
mapfile -t zimages < <(find / -type f -name "zImage" 2>/dev/null)

# Check how many files were found
count=${#zimages[@]}

if [ $count -eq 0 ]; then
	echo "No zImage file found. Please build the kernel first."
	exit 1
elif [ $count -eq 1 ]; then
	zImage="${zimages[0]}"
	echo "Found one zImage file: $zImage"
else
	echo "Multiple zImage files found:"
	for i in "${!zimages[@]}"; do
		 echo "[$i] ${zimages[$i]}"
	done
	read -p "Enter the number of the zImage file to use: " index
	if ! [[ "$index" =~ ^[0-9]+$ ]] || [ "$index" -ge "$count" ]; then
	   echo "Invalid selection."
		exit 1
	fi
	zImage="${zimages[$index]}"
	echo "You zImage: $zImage"
fi

# Find the versatile-pb.dtb starting from root
mapfile -t dtbs < <(find / -type f -name "versatile-pb.dtb" 2>/dev/null)

# Check how many files were found
count=${#dtbs[@]}

if [ $count -eq 0 ]; then
   echo "No versatile-pb.dtb file found. Please build the kernel first."
   exit 1
elif [ $count -eq 1 ]; then
   dtbs="${dtbs[0]}"
   echo "Found one versatile-pb.dtb file: $dtbs"
else
   echo "Multiple versatile-pb.dtb files found:"
   for i in "${!dtbs[@]}"; do
       echo "[$i] ${dtbs[$i]}"
   done
   read -p "Enter the number of the versatile-pb.dtb file to use: " index
   if ! [[ "$index" =~ ^[0-9]+$ ]] || [ "$index" -ge "$count" ]; then
      echo "Invalid selection."
      exit 1
   fi
   dtbs="${dtbs[$index]}"
   echo "You dtbs: $dtbs"
fi

# Find the rootfs.cpio.gz starting from root
mapfile -t rootfs < <(find / -type f -name "rootfs.cpio.gz" 2>/dev/null)

# Check how many files were found
count=${#rootfs[@]}

if [ $count -eq 0 ]; then
   echo "No rootfs.cpio.gz file found. Please build the rootfs first."
   exit 1
elif [ $count -eq 1 ]; then
   rootfs="${rootfs[0]}"
   echo "Found one rootfs.cpio.gz file: $rootfs"
else
   echo "Multiple rootfs.cpio.gz files found:"
   for i in "${!rootfs[@]}"; do
       echo "[$i] ${rootfs[$i]}"
   done
   read -p "Enter the number of the rootfs.cpio.gz file to use: " index
   if ! [[ "$index" =~ ^[0-9]+$ ]] || [ "$index" -ge "$count" ]; then
      echo "Invalid selection."
      exit 1
   fi
   rootfs="${rootfs[$index]}"
   echo "You rootfs: $rootfs"
fi

# FINALLY ! We have all the paths we need
# Now we can run qemu-system-arm with the selected files

# Wait for the user to press Enter
read -p "Press Enter to start QEMU with the selected files..."

qemu-system-arm -M versatilepb \
	-kernel $zImage \
	-dtb $dtbs \
	-initrd $rootfs \
	-serial stdio \
	-append "root=/dev/mem console=ttyAMA0" \
	-display none
