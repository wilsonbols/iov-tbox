# Cross-compile C program, build script is in ./cantomqtt/make.sh. You can also use cmake script

docker login -u cn-north-4@XVLLZV9P2LDL1Q47R2R5 -p 2206da3a9b99abd4bfaae36f42f590512f638e4d3125bb0c5be0e35816704941 swr.cn-north-4.myhuaweicloud.com
pwd
docker run -i -v .:/root/tbox/ swr.cn-north-4.myhuaweicloud.com/iov-workshop/tbox-compiler:v4

# Regenerate ext4, put the compiled files into
cd ..
mkdir ~/tmp
mount ./QEMU/docker/files/qemufiles/initrd_32le_v2.ext4 ~/tmp
rm -rf  ~/tmp/iov/
mkdir ~/tmp/iov/
cp -rf ./cantomqtt/can* ~/tmp/iov/
chmod -R 777 ~/tmp/iov/
umount ~/tmp


# Generate qemu image
docker build -t swr.cn-north-4.myhuaweicloud.com/iov-workshop/can_qemu:latest ./QEMU/docker
docker push swr.cn-north-4.myhuaweicloud.com/iov-workshop/can_qemu:latest

# Delete compiled container
#docker rm $(docker ps -a -q --filter ancestor=$(docker images -a -q tbox-compiler:v4))