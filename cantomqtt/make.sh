cmake . 
arm-linux-gnueabi-gcc main.c common/conversion.c -o can_mqtt -lpaho-mqtt3c -ljansson
chmod -R 777 cantomqtt