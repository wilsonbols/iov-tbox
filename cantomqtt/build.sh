

#!/bin/bash

can_sed="./can_sed"
mqtt_client="./mqtt_client"

# 检查文件是否存在
if [ -e "$can_sed" ]; then
    rm -f "$can_sed"
    echo "can_sed deleted."
else
    echo "File does not exist."
fi


if [ -e "$mqtt_client" ]; then
    rm -f "$mqtt_client"
    echo "mqtt_client deleted."
else
    echo "File does not exist."
fi

echo "----------build start"
gcc writecan.c control.c -o can_sed -ljansson

gcc mqtt.c control.c collision.c -o mqtt_client -lpaho-mqtt3c -ljansson

echo "----------build end"