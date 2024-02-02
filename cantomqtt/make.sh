

#!/bin/bash

can_send="./can_send"
tbox="./tbox"

# 检查文件是否存在
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

arm-linux-gnueabi-gcc writecan.c control.c -o can_send -ljansson -march=armv5te
arm-linux-gnueabi-gcc mqtt.c control.c collision.c -o tbox -lpaho-mqtt3c -ljansson -march=armv5te
chmod -R 777 can_send
chmod -R 777 tbox

echo "----------build end"