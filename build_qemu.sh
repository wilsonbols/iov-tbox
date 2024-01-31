#交叉编译c程序，编译脚本在./cantomqtt/make.sh 中。也可以使用cmake脚本
docker login -u cn-north-4@XVLLZV9P2LDL1Q47R2R5 -p 2206da3a9b99abd4bfaae36f42f590512f638e4d3125bb0c5be0e35816704941 swr.cn-north-4.myhuaweicloud.com
docker run -it -v ./cantomqtt/:/root/tbox/ swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4

#重新生成ext4，将编译后文件放入
mkdir ~/tmp
mount ./QEMU/docker/files/qemufiles/initrd_32le_v2.ext4 ~/tmp
rm -rf  ~/tmp/iov/
mkdir ~/tmp/iov/
cp -rf ./cantomqtt/can* ~/tmp/iov/
chmod -R 777 ~/tmp/iov/
umount ~/tmp


#生成qemu镜像
docker build -t swr.cn-north-4.myhuaweicloud.com/iov-workshop/can_qemu:latest ./QEMU/docker
docker login -u cn-north-4@XVLLZV9P2LDL1Q47R2R5 -p 2206da3a9b99abd4bfaae36f42f590512f638e4d3125bb0c5be0e35816704941 swr.cn-north-4.myhuaweicloud.com
docker push swr.cn-north-4.myhuaweicloud.com/iov-workshop/can_qemu:latest

#删除编译后的容器
#docker rm $(docker ps -a -q --filter ancestor=$(docker images -a -q tbox-compiler:v4))