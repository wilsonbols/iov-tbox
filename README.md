# Running Locally (Ubuntu 20.04 LTS)

## Compilation Dependencies

Note: All compilation dependencies have been built into the image swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4.

- For a local environment, refer to this: https://www.cnblogs.com/CltCj/articles/17661086.html
- Install dependencies:

```
apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
apt-get install libssl-dev
```

Since qemu is an aarch64 architecture application, install arm cross-compiling gcC

```
apt-get install gcc-arm-linux-gnueabi
```

Download the code repository

```
mkdir ~/mqtt-client-sdk && cd ~/mqtt-client-sdk
git clone https://github.com/eclipse/paho.mqtt.c.git # Alternatively, use the domestic mirror: https://gitee.com/mirrors/paho.mqtt.c.git
cd paho.mqtt.c
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

## Compile the sample program
```
mkdir manual-build && cd manual-build
cmake .. && make  ##Build 
cd .. && chmod -R 777 manual-build ##Add execution permissions
cd manual-build
./IoV-Edge-CClient  ## Run crtl+C to exit

- The program will subscribe to the main vehicle-warning message
- The program will loop to publish the topic vehicle-info, one will be read from the vehicle-info.json file, and the other will be in the same format of static content

- Use http://www.emqx.io/online-mqtt-client to receive and send messages to verify the messages of the two topics above

## This code library is modified based on this code library
https://github.com/leansoftX/code-arts-mqtt-client-demo.git

mkdir build
cd build
cmake ..
make

```
## Rebuild ext4 and put the compiled files into it
```
cd ~/qemu/
mount ./docker/files/qemufiles/initrd_32le_v2.ext4 ~/tmp
rm -rf  ~/tmp/iov/
mkdir ~/tmp/iov/
cp -rf ~/IoV-Edge-CClient/cantomqtt/can* ~/tmp/iov/
chmod -R 777 ~/tmp/iov/
umount ~/tmp
```

## Add a virtual can device to the host virtual machine
```
modprobe vcan
ip link add dev vcan0 type vcan
ip link set vcan0 up
```

## Add network settings for communication with the host in the host virtual machine
```
ip tuntap add dev tap0 mode tap
ip link set dev tap0 up
ip address add dev tap0 192.168.181.128/24
```

## Add network settings for communication with the host in qemu
```
ip addr add 192.168.181.129/24 dev eth0
ip link set eth0 up

ping 192.168.181.128
```

## Cross-compiled C program container generation (the initial file for generating the image is in the root account's ~/images)

docker build -t tbox-compiler:v4 .

## Cross-compiled C program in the source code directory

docker run -it -v .:/root/tbox/ tbox-compiler:v4

docker pull swr.cn-north-4.myhuaweicloud.com/iov-workshop/

## Generate qemu run
```
docker build -t can_qemu:v3 ./docker
docker run -d --network=host --privileged can_qemu:v3
```

cansend vcan0  123#0D54024AE0F0

cansend vcan0  123#06C40127E0F0

cansend vcan0  123#1A4E0023E0F0

cansend vcan0  123#11430129E0F0

cansend vcan0  123#0F710019E0F0

