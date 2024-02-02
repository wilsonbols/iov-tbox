

#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <stdio.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <jansson.h>
#include "MQTTClient.h"
#include "control.h"

void readcansimulate(MQTTClient client, char *topic);
void mqtt_publish(MQTTClient client, char *topic, char *payload);


void readcansimulate(MQTTClient client, char *topic)
{

    struct ifreq ifr = {0};
    struct sockaddr_can can_addr = {0};
    struct can_frame frame = {0};
    int sockfd = -1;
    int i;
    int ret;

    /* 打开套接字 */
    sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(0 > sockfd) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* 指定can0设备 */
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sockfd, SIOCGIFINDEX, &ifr);
    can_addr.can_family = AF_CAN;
    can_addr.can_ifindex = ifr.ifr_ifindex;

    /* 将can0与套接字进行绑定 */
    ret = bind(sockfd, (struct sockaddr *)&can_addr, sizeof(can_addr));
    if (0 > ret) {
        perror("bind error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 设置过滤规则 */
    //setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    /* 接收数据 */
    for ( ; ; ) {
        if (0 > read(sockfd, &frame, sizeof(struct can_frame))) {
            perror("read error");
            break;
        }

        /* 校验是否接收到错误帧 */
        if (frame.can_id & CAN_ERR_FLAG) {
            printf("Error frame!\n");
            break;
        }

        /* 校验帧格式 */
        if (frame.can_id & CAN_EFF_FLAG)	//扩展帧
            printf("扩展帧 <0x%08x> ", frame.can_id & CAN_EFF_MASK);
        else		//标准帧
            printf("标准帧 <0x%03x> ", frame.can_id & CAN_SFF_MASK);

        /* 校验帧类型：数据帧还是远程帧 */
        if (frame.can_id & CAN_RTR_FLAG) {
            printf("remote request\n");
            continue;
        }

        /* 打印数据长度 */
        printf("[%d] ", frame.can_dlc);

        /* 打印数据 */
        for (i = 0; i < frame.can_dlc; i++){
            printf("%02x ", frame.data[i]);
        }
        printf("\n");

        // 调用函数获取时间字符串
        char *beijingTime = getBeijingTime();

        if (frame.can_id == 0x123) {

            // 解析
            int engineRPM = (frame.data[0] << 8) | frame.data[1];
            char prmstr[50];
            // 使用 sprintf 函数将整数转换为字符串
            sprintf(prmstr, "车辆发动机转速:%d", engineRPM);
            // 创建 JSON 对象
            json_t *root = json_object();
            // 添加键值对到 JSON 对象
            json_object_set_new(root, "vin", json_string("NH2FX13D223ES1340"));
            json_object_set_new(root, "collisioninfo", json_string(getcollision(frame.data[2])));
            json_object_set_new(root, "locationinfo", json_string(getlocation(frame.data[3])));
            json_object_set_new(root, "descriptioninfo", json_string(prmstr));

            // 将 JSON 对象转换为字符串
            char *jsonString = json_dumps(root, JSON_ENCODE_ANY);

            // 输出 JSON 字符串
            //printf("%s\n", jsonString);

            mqtt_publish(client, topic, jsonString);
            // 释放内存
            free(jsonString);
            json_decref(root);
        }

        // 判断 CAN ID 是否为 0x200
        if (frame.can_id == 0x200) {
            // 解析数据
            int action = frame.data[1];
            printf("%s------从can总线获取开关门信息: %d \n",beijingTime, action);
        }
        printf("\n");
        sleep(20);   //延迟

    }
    /* 关闭套接字 */
    close(sockfd);
}



// 发布消息
void mqtt_publish(MQTTClient client, char *topic, char *payload) {
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = strlen(payload);
    message.qos = 0;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, 10000L);
    // 调用函数获取时间字符串
    char *beijingTime = getBeijingTime();
    // 输出
    printf("%s------Send \n`%s`\n to topic `%s` \n",beijingTime, payload, topic);
}