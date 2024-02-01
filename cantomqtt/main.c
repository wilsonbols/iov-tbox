#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <stdio.h>
#include <time.h>


#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <net/if.h>
#include <jansson.h>
#include "MQTTClient.h"
#include "common/conversion.h"  // 包含转换函数的头文件


#define DEBUG       0

#define ADDRESS     "tcp://192.168.0.128:31006"
#define USERNAME    "emqx"
#define PASSWORD    "public"
#define CLIENTID    "c-client-vehicle"
#define QOS         0
#define TOPIC_WARNING       "vehicle-warning" //emqx/c-test
#define TOPIC_INFO       "vehicle_collision" // vehicle-info
#define TIMEOUT     10000L

// 发布消息
void publish(MQTTClient client, char *topic, char *payload) {
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = strlen(payload);
    message.qos = QOS;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    // 调用函数获取时间字符串
    char *beijingTime = getBeijingTime();
    // 输出
    printf("%s------Send \n`%s`\n to topic `%s` \n",beijingTime, payload, topic);
}

// 接收消息
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char *payload = message->payload;
    // 调用函数获取时间字符串
    char *beijingTime = getBeijingTime();
    printf("%s------Received `%s` from `%s` topic \n",beijingTime, payload, topicName);

    //解析mqtt传入json，并发送到can
    write_can_temperature(payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int send_file(MQTTClient client,
              char *file_path,
              char *file_id,
              char *file_name,
              unsigned long expire_time_s_since_epoch,
              unsigned long segments_ttl_seconds) {
    FILE *fp = fopen(file_path, "rb");
    int rc;
    int qos = 1;
    const size_t buf_size = 2048;
    if (fp == NULL) {
        printf("Failed to open file %s\n", file_path);
        return -1;
    }
    // Get file size
    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    // Create payload for initial message
    char payload[buf_size];
    char expire_at_str[128];
    char segments_ttl_str[128];
    if (expire_time_s_since_epoch == -1) {
        expire_at_str[0] = '\0';
    } else {
        // No need to check return value since we know the buffer is large enough
        snprintf(expire_at_str,
                 128,
                 "  \"expire_at\": %ld,\n",
                 expire_time_s_since_epoch);
    }
    if (segments_ttl_seconds == -1) {
        segments_ttl_str[0] = '\0';
    } else {
        // No need to check return value since we know the buffer is large enough
        snprintf(segments_ttl_str,
                 128,
                 "  \"segments_ttl\": %ld,\n",
                 segments_ttl_seconds);
    }
    rc = snprintf(
            payload,
            buf_size,
            "{\n"
            "  \"name\": \"%s\",\n"
            "  \"size\": %ld,\n"
            "%s"
            "%s"
            "  \"user_data\": {}\n"
            "}",
            file_name,
            file_size,
            expire_at_str,
            segments_ttl_str);
    if (rc < 0 || rc >= buf_size) {
        printf("Failed to create payload for initial message\n");
        return -1;
    }
    // Create topic of the form $file/{file_id}/init for initial message
    char topic[buf_size];
    MQTTClient_deliveryToken token;
    rc = snprintf(topic, buf_size, "$file/%s/init", file_id);
    if (rc < 0 || rc >= buf_size) {
        printf("Failed to create topic for initial message\n");
        return -1;
    }
    // Publish initial message
    if (DEBUG) {
        printf("Publishing initial message to topic %s\n", topic);
        printf("Payload: %s\n", payload);
    }
    rc = MQTTClient_publish(client, TOPIC_INFO, strlen(payload), payload, 1, 0, &token);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        return -1;
    }
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        return -1;
    }
    // Read binary chunks of max size 1024 bytes and publish them to the broker
    // The chunks are published to the topic of the form $file/{file_id}/{offset}
    // The chunks are read into the payload
    size_t chunk_size = 1024;
    size_t offset = 0;
    size_t read_bytes;
    while ((read_bytes = fread(payload, 1, chunk_size, fp)) > 0) {
        rc = snprintf(topic, buf_size, "$file/%s/%lu", file_id, offset);
        if (rc < 0 || rc >= buf_size) {
            printf("Failed to create topic for file chunk\n");
            return -1;
        }
        if (DEBUG) {
            printf("Publishing file chunk to topic %s offset %lu\n", topic, offset);
        }
        rc = MQTTClient_publish(client, TOPIC_INFO, read_bytes, payload, 1, 0, &token);
        if (rc != MQTTCLIENT_SUCCESS) {
            printf("Failed to publish file chunk, return code %d\n", rc);
            return -1;
        }
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        if (rc != MQTTCLIENT_SUCCESS) {
            printf("Failed to publish file chunk, return code %d\n", rc);
            return -1;
        }
        offset += read_bytes;
    }
    // Check if we reached the end of the file
    if (feof(fp)) {
        if (DEBUG) {
            printf("Reached end of file\n");
        }
    } else {
        printf("Failed to read file\n");
        return -1;
    }
    fclose(fp);
    // Send final message to the topic $file/{file_id}/fin/{file_size} with an empty payload
    rc = snprintf(topic, buf_size, "$file/%s/fin/%ld", file_id, file_size);
    if (rc < 0 || rc >= buf_size) {
        printf("Failed to create topic for final message\n");
        return -1;
    }
    if (DEBUG) {
        printf("Publishing final message to topic %s\n", topic);
    }
    rc = MQTTClient_publish(client, TOPIC_INFO, 0, "", 1, 0, &token);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish final message, return code %d\n", rc);
        return -1;
    }
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish final message, return code %d\n", rc);
        return -1;
    }
    printf("json file message publish to topic %s completed,the content is: \n",TOPIC_INFO);
    printf("%s\n",payload);
    return 0;
}

int main(int argc, char *argv[]) {
    int rc;
    MQTTClient client;

    MQTTClient_create(&client, ADDRESS, CLIENTID, 0, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    } else {
        printf("Connected to MQTT Broker!\n");
    }
    // subscribe topic
    MQTTClient_subscribe(client, TOPIC_WARNING, QOS);


    // Calculate expire time
    unsigned long expire_time_s_since_epoch = time(NULL) + 5;

    char *file_name = "vehicle-info.json";
    char *file_path = "/app/code/cl/vehicle-info.json";
    char *file_id = "wenjie-m7";

    //获取can数据start
    // -------------------------------------------------------------

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
        //发动机转速
        if (frame.can_id == 0x123) {

            // 解析发动机转数数据
            int engineRPM = (frame.data[0] << 8) | frame.data[1];
            //printf("%s------Received Engine RPM: %d\n", beijingTime, engineRPM);
            //printf("%s------Received data[2]: %d\n", beijingTime, frame.data[2]);
            //printf("%s------Received data[3]: %d\n", beijingTime, frame.data[3]);
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



            //char jsonStr[300];
            // publish message to broker
            //sprintf(jsonStr, "{\"vin\":\"LSVHJ133022221761\",""\"rpm\": \"%d\"}", engineRPM);
            publish(client, TOPIC_INFO, jsonString);

            // Send file
//            int result = send_file(client,
//                                   file_path,
//                                   file_id,
//                                   file_name,
//                                   expire_time_s_since_epoch,
//                                   5);

            // 释放内存
            free(jsonString);
            json_decref(root);


        }

        // 判断 CAN ID 是否为 0x200
        if (frame.can_id == 0x200) {
            // 解析空调温度数据
            int temperature = (frame.data[0] << 8) | frame.data[1];
            printf("%s------通过can_id从can总线接收空调温度: %d°C\n",beijingTime, temperature);

        }





        printf("\n");
        sleep(20);   //延迟

    }
    /* 关闭套接字 */
    close(sockfd);

    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);

    exit(EXIT_SUCCESS);

    //---------------------------------------------------------------
    //获取can数据end

    return rc;



}