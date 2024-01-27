cmake . 
arm-linux-gnueabi-gcc writecan.c common/conversion.c -o can_write -ljansson -march=armv5te
arm-linux-gnueabi-gcc main.c common/conversion.c -o can_mqtt -lpaho-mqtt3c -ljansson -march=armv5te
chmod -R 777 can_mqtt
chmod -R 777 can_write