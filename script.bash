#!/bin/bash

# Script Name: script.bash
# Description: Launch qemu-system-arm with cached or selected paths
# Author: Simone Moretti

CONFIG_FILE="$HOME/.qemu_config.sh"
REFRESH=false
SEARCH_DIR="$HOME"  # Change this to a more likely directory

# Check for refresh flag
if [[ "$1" == "--refresh" ]]; then
	REFRESH=true
fi

# Load config if available and not refreshing
if [[ -f "$CONFIG_FILE" && "$REFRESH" == false ]]; then
	source "$CONFIG_FILE"
	echo "Using cached config:"
	echo "zImage: $zImage"
	echo "dtbs: $dtbs"
	echo "rootfs: $rootfs"
else
	echo "Searching for required files..."

	find_and_select() {
		local name="$1"
		local varname="$2"

		echo "Searching for $name..."
		mapfile -t matches < <(find "$SEARCH_DIR" -type f -name "$name" 2>/dev/null)

		local count=${#matches[@]}
		if [ $count -eq 0 ]; then
			echo "No $name found in $SEARCH_DIR. Trying root filesystem..."
			mapfile -t matches < <(find / -type f -name "$name" 2>/dev/null)
			count=${#matches[@]}
		fi

		if [ $count -eq 0 ]; then
			echo "No $name found. Please build or provide it."
			exit 1
		elif [ $count -eq 1 ]; then
			eval "$varname=\"${matches[0]}\""
			echo "Found $name: ${matches[0]}"
		else
			echo "Multiple $name files found:"
			for i in "${!matches[@]}"; do
				echo "[$i] ${matches[$i]}"
			done
			read -p "Enter the number of the $name to use: " index
			if ! [[ "$index" =~ ^[0-9]+$ ]] || [ "$index" -ge "$count" ]; then
				echo "Invalid selection."
				exit 1
			fi
			eval "$varname=\"${matches[$index]}\""
		fi
	}

	find_and_select "zImage" zImage
	find_and_select "versatile-pb.dtb" dtbs
	find_and_select "rootfs.cpio.gz" rootfs

	# Save config
	cat > "$CONFIG_FILE" <<EOF
zImage="$zImage"
dtbs="$dtbs"
rootfs="$rootfs"
EOF

	echo "Saved configuration to $CONFIG_FILE"
fi

# Confirm before running QEMU
read -p "Press Enter to start QEMU with the selected files..."

qemu-system-arm -M versatilepb \
	-kernel "$zImage" \
	-dtb "$dtbs" \
	-initrd "$rootfs" \
	-serial stdio \
	-append "root=/dev/mem console=ttyAMA0" \
	-display none
