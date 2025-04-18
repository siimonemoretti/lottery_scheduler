# Group 9 OS Project - Kernel scheduler modifications and performance analysis

These are the steps followed to install the correct utilities and libraries to run the code.

NOTE: These steps were tested on a clean Ubuntu 22.04.05 (LTS) installation.

## 0. Setup GIT Identity

First thing to do is to set up correctly the git identity. This is only needed to push code to the repository, not to reproduce the project. 

**IMPORTANT**: Use your **own name** and **PoliTo email** address to ensure clarity.

```bash
git config user.name "Your Name"
git config user.email "Your Email"
```

## 1. Install the required packages

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

## 2. Install the kernel source code and busybox

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