#本地运行(Ubuntu 20.04 LTS)

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

编辑 **cmake/toolchain.linux-arm11.cmake**，使用和qemu相同的架构编译器

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

记得将输出的所有文件拷贝到ext4中

```
Installing: /usr/local/lib/libpaho-mqtt3c.so.1.3.13
-- Installing: /usr/local/lib/libpaho-mqtt3c.so.1
-- Installing: /usr/local/lib/libpaho-mqtt3c.so
-- Installing: /usr/local/lib/libpaho-mqtt3a.so.1.3.13
-- Installing: /usr/local/lib/libpaho-mqtt3a.so.1
-- Installing: /usr/local/lib/libpaho-mqtt3a.so
-- Installing: /usr/local/bin/MQTTVersion
-- Set runtime path of "/usr/local/bin/MQTTVersion" to ""
-- Installing: /usr/local/lib/libpaho-mqtt3c.a
-- Installing: /usr/local/lib/libpaho-mqtt3a.a
-- Installing: /usr/local/include/MQTTAsync.h
-- Installing: /usr/local/include/MQTTClient.h
-- Installing: /usr/local/include/MQTTClientPersistence.h
-- Installing: /usr/local/include/MQTTProperties.h
-- Installing: /usr/local/include/MQTTReasonCodes.h
-- Installing: /usr/local/include/MQTTSubscribeOpts.h
-- Installing: /usr/local/include/MQTTExportDeclarations.h
-- Installing: /usr/local/lib/cmake/eclipse-paho-mqtt-c/eclipse-paho-mqtt-cConfig.cmake
-- Installing: /usr/local/lib/cmake/eclipse-paho-mqtt-c/eclipse-paho-mqtt-cConfig-noconfig.cmake
-- Installing: /usr/local/lib/cmake/eclipse-paho-mqtt-c/eclipse-paho-mqtt-cConfigVersion.cmake
```

## 编译样例程序
```
mkdir manual-build && cd manual-build
cmake .. && make  ##构建 
cd .. && chmod -R 777 manual-build ##添加执行权限
cd manual-build
./IoV-Edge-CClient  ## 运行 crtl+C 退出

- 程序会订阅主 vehicle-warning 消息
- 程序会循环发布主题 vehicle-info, 会发两个，一个是读取vehicle-info.json文件的内容，一个是同样格式的静态内容

- 使用 http://www.emqx.io/online-mqtt-client 接收和发送消息以验证上面两个主题的消息

## 本代码库根据这个代码库修改而来
https://github.com/leansoftX/code-arts-mqtt-client-demo.git


