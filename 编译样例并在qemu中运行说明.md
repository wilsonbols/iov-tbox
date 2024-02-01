#环境准备

这里用到了三个运行环境： host , docker container , qemu
host 机器，最好是一台 x86-64 的 ubuntu linux 主机。我测试的环境是 Ubuntu 22.04, kernel 为 5.15

```
# 注意：如果使用Azure服务器，需要执行下面命令：
sudo apt-get install -y linux-modules-extra-$(uname -r)

#安装配置vcan设备
modprobe vcan
ip link add dev vcan0 type vcan
ip link set vcan0 up

#添加网卡设置，以便qemu可以通过ip访问host端口
ip tuntap add dev tap0 mode tap
ip link set dev tap0 up
ip address add dev tap0 192.168.2.128/24

```


#编译样例程序并生成镜像，推送到镜像仓库

下载代码库 IoV-TBox 和 IoV-Deploy

```
git clone --recuresive [IoV-TBox ssh URL]
git clone [IoV-Deploy  ssh URL]
```


```
cd IoV-TBox

#编译cpp程序，只做qemu镜像，推送到swr
./build_qemu.sh

cd ../IoV-Deploy
#修改 can_mqtt的镜像版本
vim docker-compose-all.yml

docker-compose -f docker-compose-all.yml up -d

```
启动后可以通过如下地址访问safeplatform页面
http://[vmip]:8088

登录到vm中模拟设备发送can信号

```
#如果未安装 can-utils
apt-get update
apt-get install can-utils

cansend vcan0  123#0D54024AE0F0
cansend vcan0  123#06C40127E0F0
cansend vcan0  123#1A4E0023E0F0
cansend vcan0  123#11430101E0F0
cansend vcan0  123#0DB60102E0F0
cansend vcan0  123#0F710119E0F0
```

发送模拟信号后在前台页面应该可以看到事故信息