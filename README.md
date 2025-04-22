# Group 9 OS Project - Kernel scheduler modifications and performance analysis

These are the steps followed to install the correct utilities and libraries to run the code.

NOTE: These steps were tested on a clean Ubuntu 22.04.05 (LTS) installation.

## Requirements

### 0. Setup GIT Identity

First thing to do is to set up correctly the git identity. This is only needed to push code to the repository, not to reproduce the project. 

**IMPORTANT**: Use your **own name** and **PoliTo email** address to ensure clarity.

```bash
git config user.name "Your Name"
git config user.email "Your Email"
```

### 1. Install the required packages

```bash
sudo apt update
sudo apt install gcc-arm-linux-gnueabi qemu-system-arm
```

At the time of writing, the versions of the packages are:

- arm-linux-gnueabi-gcc: 11.4.0
- qemu-system-arm: 6.2.0

To check the version of the installed packages, run the following commands:

```bash
arm-linux-gnueabi-gcc --version
qemu-system-arm --version
```

Moreover, it's possible that the following utilities are not installed:

- make
- flex 
- bison 
- libncurses5-dev 
- libncursesw5-dev

To install them:

```bash
sudo apt install make flex bison libncurses5-dev libncursesw5-dev
```

### 2. Install the kernel source code and busybox

The kernel version chosen for this project is 5.15.180. To download and extract the kernel source code, run the following commands:

```bash
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.180.tar.xz
tar xf linux-5.15.180.tar.xz
```

Next, install busybox:

```bash
wget https://busybox.net/downloads/busybox-1.36.1.tar.bz2
tar xf busybox-1.36.1.tar.bz2
```

NOTE: It's possible that "lbzip2" is not installed by default. If you get an error when extracting the busybox tarball, install it with the following command:

```bash
sudo apt update && sudo apt install lbzip2
```

### 3. Build the kernel 

Go inside the linux kernel folder:

```bash
cd linux-5.15.180/
```

Set up the configuration for building the linux kernel for the VersatilePB board:

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- versatile_defconfig
```

Finally, compile the kernel:

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
```

### 4. Configure and build BusyBox

```bash
cd ../busybox-1.36.1/
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- defconfig
make menuconfig
```

Now, go inside "Settings", then down to "Build options" and enable (with key "Y") the *build static binary* option. Then build:

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- install
```

### 5. Create the (minimal) filesystem

```bash
mkdir ~/rootfs
cd ~/rootfs
touch init
chmod +x init
```

This creates an init script inside *rootfs* folder and gives it permit to execute it.

Inside the init place the following:

```bash
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
echo -e "\\n Hello from group9!\\n"
/bin/sh
```

Copy now the busybox *_install* files:

```bash
cp -av ../busybox-1.36.1/_install/* .
```

Create the standard folder structure:

```bash
mkdir -pv {bin,sbin,etc,proc,sys,usr/{bin,sbin}}
```

Create the *initramfs* that gets passed to the Linux kernel.

```bash
find . -print0 | cpio --nul -ov --format=newc | gzip -9 > rootfs.cpio.gz
```

### 6. Launch it

Use the following command to launch qemu with the kernel just built.

```bash
qemu-system-arm -M versatilepb -kernel ~/linux-5.15.180/arch/arm/boot/zImage -dtb ~/linux-5.15.180/arch/arm/boot/dts/versatile-pb.dtb -initrd ~/rootfs/rootfs.cpio.gz -serial stdio -append "root=/dev/mem console=ttyAMA0" -display none
```