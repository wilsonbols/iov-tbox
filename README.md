#本地运行(Ubuntu 20.04 LTS)

## 编译依赖

说明：目前所有编译依赖已经被打入镜像swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4。


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
 
编译

```
 cmake -DCMAKE_TOOLCHAIN_FILE=~/mqtt-client-sdk/paho.mqtt.c/cmake/toolchain.linux-arm11.cmake   \
       -DPAHO_BUILD_STATIC=TRUE     \
       -DPAHO_BUILD_SAMPLES=FALSE   \
       ~/mqtt-client-sdk/paho.mqtt.c
make 
make install
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

mkdir build
cd build
cmake ..
make

#重新生成ext4，将编译后文件放入
cd ~/qemu/
mount ./docker/files/qemufiles/initrd_32le_v2.ext4 ~/tmp
rm -rf  ~/tmp/iov/
mkdir ~/tmp/iov/
cp -rf ~/IoV-Edge-CClient/cantomqtt/can* ~/tmp/iov/
chmod -R 777 ~/tmp/iov/
umount ~/tmp


#在host虚拟机上添加虚拟can设备
modprobe vcan
ip link add dev vcan0 type vcan
ip link set vcan0 up

#在host虚拟机上添加与qemu通信的网卡设置
ip tuntap add dev tap0 mode tap
ip link set dev tap0 up
ip address add dev tap0 192.168.181.128/24

#在qemu中添加网卡设置，以便和host通信
ip addr add 192.168.181.129/24 dev eth0
ip link set eth0 up

ping 192.168.181.128


#交叉式c程序编译容器生成，（在root账户的 ~/images下有生成镜像初始文件）
docker build -t tbox-compiler:v4 .
#在源代码所在目录 交叉式编译c程序
docker run -it -v .:/root/tbox/ tbox-compiler:v4


docker pull swr.cn-north-4.myhuaweicloud.com/iov-workshop/

#生成qemu运行
docker build -t can_qemu:v3 ./docker
docker run -d --network=host --privileged can_qemu:v3


cansend vcan0  123#0D54024AE0F0
cansend vcan0  123#06C40127E0F0
cansend vcan0  123#1A4E0023E0F0
cansend vcan0  123#11430129E0F0
cansend vcan0  123#0F710019E0F0

