#交叉编译c程序
cd cantomqtt
cmake . 
docker run -it -v .:/root/tbox/ tbox-compiler:v4
chmod -R 777 can_mqtt


#重新生成ext4，将编译后文件放入
cd ..
mount ./qemu/docker/files/qemufiles/initrd_32le_v2.ext4 ~/tmp
rm -rf  ~/tmp/iov/
mkdir ~/tmp/iov/
cp -rf ./cantomqtt/can* ~/tmp/iov/
chmod -R 777 ~/tmp/iov/
umount ~/tmp

