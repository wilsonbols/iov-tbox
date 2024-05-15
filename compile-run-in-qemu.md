#Environment Preparation

Three running environments are used here: host, docker container, and qemu. The host machine is best an x86-64 Ubuntu Linux host. The environment I tested is Ubuntu 22.04, kernel 5.15.

```
# Note: If you are using an Azure server, you need to execute the following command:
sudo apt-get install -y linux-modules-extra-$(uname -r)

# Install and configure vcan device
modprobe vcan
ip link add dev vcan0 type vcan
ip link set vcan0 up

# Add network card settings so that qemu can access host ports via ip
ip tuntap add dev tap0 mode tap
ip link set dev tap0 up
ip address add dev tap0 192.168.181.128/24

```


#Compile the sample program and generate the image, push it to the image repository

Download the code repository IoV-TBox and IoV-Deploy

```
git clone --recursive [IoV-TBox ssh URL]
git clone [IoV-Deploy ssh URL]
```


```
cd IoV-TBox

# Compile the cpp program, only do qemu image, push to swr
./build_qemu.sh

cd ../IoV-Deploy
# Modify the image version of can_mqtt
vim docker-compose-all.yml

docker-compose -f docker-compose-all.yml up -d

```
After starting, you can access the safeplatform page via the following address:
http://[vmip]:8088

Log in to the vm to simulate the device sending can signals

```
# If can-utils is not installed
apt-get update
apt-get install can-utils

cansend vcan0 123#0D54024AE0F0
cansend vcan0 123#06C40127E0F0
cansend vcan0 123#1A4E0023E0F0
cansend vcan0 123#11430101E0F0
cansend vcan0 123#0DB60102E0F0
cansend vcan0 123#0F710119E0F0
```

After sending simulated signals, you should be able to see accident information on the front-end page.