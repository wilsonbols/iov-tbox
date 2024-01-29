#交叉编译c程序
sudo docker login -u cn-north-4@XVLLZV9P2LDL1Q47R2R5 -p 2206da3a9b99abd4bfaae36f42f590512f638e4d3125bb0c5be0e35816704941 swr.cn-north-4.myhuaweicloud.com
sudo docker run -it -v ./cantomqtt/:/root/tbox/ swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4

#重新生成ext4，将编译后文件放入
sudo mkdir ~/tmp
sudo mount ./QEMU/docker/files/qemufiles/initrd_32le_v2.ext4 ~/tmp
sudo rm -rf  ~/tmp/iov/
sudo mkdir ~/tmp/iov/
sudo cp -rf ./cantomqtt/can* ~/tmp/iov/
sudo chmod -R 777 ~/tmp/iov/
sudo umount ~/tmp


#生成qemu镜像
sudo docker build -t swr.cn-north-4.myhuaweicloud.com/iov-workshop/can_qemu:latest ./QEMU/docker
sudo docker login -u cn-north-4@XVLLZV9P2LDL1Q47R2R5 -p 2206da3a9b99abd4bfaae36f42f590512f638e4d3125bb0c5be0e35816704941 swr.cn-north-4.myhuaweicloud.com
sudo docker push swr.cn-north-4.myhuaweicloud.com/iov-workshop/can_qemu:latest

#删除编译后的容器
#docker rm $(docker ps -a -q --filter ancestor=$(docker images -a -q tbox-compiler:v4))