# Local Run (Ubuntu 20.04 LTS)

This document describes the method of generating the tbox_compiler image

## Compilation Dependencies

- Local environment reference this: https://www.cnblogs.com/CltCj/articles/17661086.html
- Install dependencies:

```
apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
apt-get install libssl-dev
```

Since qemu is an aarch64 architecture application, install arm cross-compiling gcC

```
apt-get install gcc-arm-linux-gnueabi
```

Download the code library

```
mkdir ~/mqtt-client-sdk && cd ~/mqtt-client-sdk
git clone https://github.com/eclipse/paho.mqtt.c.git # You can also use the domestic mirror: https://gitee.com/mirrors/paho.mqtt.c.git
cd paho.mqtt.c
```

Edit **cmake/toolchain.linux-arm11.cmake** to use the same architecture compiler as qemu.

```
# path to compiler and utilities
# specify the cross compiler
SET(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)

# Name of the target platform
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# Version of the system
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_FLAGS "-march=armv5te")
```

Compile

```
 cmake -DCMAKE_TOOLCHAIN_FILE=~/mqtt-client-sdk/paho.mqtt.c/cmake/toolchain.linux-arm11.cmake   \
       -DPAHO_BUILD_STATIC=TRUE     \
       -DPAHO_BUILD_SAMPLES=FALSE   \
       ~/mqtt-client-sdk/paho.mqtt.c
make 
make install
```

Compile and install jansson dependency

```
wget https://github.com/akheron/jansson/releases/download/v2.13/jansson-2.13.tar.bz2
bunzip2 -c jansson-2.13.tar.bz2 | tar xf -
cd jansson-2.13

./configure --host=arm-linux CC=arm-linux-gnueabi-gcc
make
make check
make install
```

Copy all compiled and installed files to the current directory to prepare for image generation

```
cd to the root directory of the code library
mkdir lib
cp -rf /usr/local/lib/libpaho* ./lib/
cp -rf /usr/local/lib/ja* ./lib/

# Build a base image
docker build -t tbox-compiler:base -f Dockerfile_compiler.base .
# Start the image and reconfigure the dependency environment
docker run -it -d -n tc tbox-compiler:base
docker exec -it tc bash
apt-get update 
# Repeat the installation note: Installing gcc-multilib is because it will prompt for missing .h files, but installing it will overwrite the original gcc-arm-linux-gnueabi, so you need to install gcc-arm-linux-gnueabi again.
# If all are written in the dockerfile, repeating the installation will cause an error. . . . So we have to do it this way.
apt-get install gcc-multilib
apt-get install gcc-arm-linux-gnueabi
exit
docker commit tc tbox-compiler:base
docker build -t tbox-compiler:v4 -f Dockerfile_compiler .

```

## Compiling Sample Programs

```bash
./build_qemu.sh

# The compilation script after the container starts is in ./cantomqtt/make.sh
```
