

#!/bin/bash

can_send="./can_send"
tbox="./tbox"

# check file exist
if [ -e "$can_send" ]; then
    rm -f "$can_send"
    echo "can_send deleted."
else
    echo "File does not exist."
fi


if [ -e "$tbox" ]; then
    rm -f "$tbox"
    echo "tbox deleted."
else
    echo "File does not exist."
fi

echo "----------build start"

# local debug
arm-linux-gnueabi-gcc -g -o can_send writecan.c control.c -ljansson -march=armv5te

# -g for gdb remote debug 
arm-linux-gnueabi-gcc -g -o tbox mqtt.c control.c collision.c -lpaho-mqtt3c -ljansson -march=armv5te
chmod -R 777 can_send
chmod -R 777 tbox

echo "----------build end"