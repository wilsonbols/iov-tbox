#本地运行(Ubuntu 20.04 LTS)

本文档说明tbox_compiler镜像的生成方法

## 编译依赖

- 本地环境参考这个：https://www.cnblogs.com/CltCj/articles/17661086.html
- 安装依赖：

```
apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
apt-get install libssl-dev
```

因为qemu是aarch64架构的应用，安装arm交叉编译gcC

```
apt-get install gcc-arm-linux-gnueabi
```

下载代码库

```
mkdir ~/mqtt-client-sdk && cd ~/mqtt-client-sdk
git clone https://github.com/eclipse/paho.mqtt.c.git # 也可以使用国内的镜像:https://gitee.com/mirrors/paho.mqtt.c.git
cd paho.mqtt.c
```

编辑 **cmake/toolchain.linux-arm11.cmake**，使用和qemu相同的架构编译器。

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
 
编译

```
 cmake -DCMAKE_TOOLCHAIN_FILE=~/mqtt-client-sdk/paho.mqtt.c/cmake/toolchain.linux-arm11.cmake   \
       -DPAHO_BUILD_STATIC=TRUE     \
       -DPAHO_BUILD_SAMPLES=FALSE   \
       ~/mqtt-client-sdk/paho.mqtt.c
make 
make install
```

编译安装jansson依赖

```
wget https://github.com/akheron/jansson/releases/download/v2.13/jansson-2.13.tar.bz2
bunzip2 -c jansson-2.13.tar.bz2 | tar xf -
cd jansson-2.13

./configure --host=arm-linux CC=arm-linux-gnueabi-gcc
make
make check
make install
```


将所有编译安装后的文件拷贝到当前目录，准备生成镜像

```
cd 到代码库根目录
mkdir lib
cp -rf /usr/local/lib/libpaho* ./lib/
cp -rf /usr/local/lib/ja* ./lib/

# 构建一个基础镜像
docker build -t tbox-compiler:base -f Dockerfile_compiler.base .
# 启动镜像后重新配置依赖环境
docker run -it -d -n tc tbox-compiler:base
docker exec -it tc bash
apt-get update 
#重复安装说明：安装gcc-multilib是因为不安装会提示少.h文件，但是安装后会覆盖原有的gcc-arm-linux-gnueabi，因此需要再次安装gcc-arm-linux-gnueabi。
#            如果全部写在dockerfile中，重复安装会报错。。。。不得已只能这样做。
FROM tbox-compiler:latest
apt-get install gcc-multilib
apt-get install gcc-arm-linux-gnueabi
exit
docker commit
docker build -t tbox-compiler:v4 -f Dockerfile_compiler .

```

## 编译样例程序


```
./build_qemu.sh

#容器启动后的编译脚本在./cantomqtt/make.sh中
```
