#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <time.h>
#include "MQTTClient.h"
#include "control.h"
#include "collision.h"

#define DEBUG       0

#define ADDRESS     "tcp://vincentwei.southeastasia.cloudapp.azure.com:1883"
#define USERNAME    "admin"
#define PASSWORD    "public"
#define CLIENTID    "c-client-vehicle"
#define QOS         0
#define VEHICLE_CONTROL       "vehicle_control"
#define VEHICLE_COLLISION       "vehicle_collision"
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
    cansend_cardoor(payload);

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
    rc = MQTTClient_publish(client, VEHICLE_COLLISION, strlen(payload), payload, 1, 0, &token);
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
        rc = MQTTClient_publish(client, VEHICLE_COLLISION, read_bytes, payload, 1, 0, &token);
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
    rc = MQTTClient_publish(client, VEHICLE_COLLISION, 0, "", 1, 0, &token);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish final message, return code %d\n", rc);
        return -1;
    }
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish final message, return code %d\n", rc);
        return -1;
    }
    printf("json file message publish to topic %s completed,the content is: \n",VEHICLE_COLLISION);
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
    MQTTClient_subscribe(client, VEHICLE_CONTROL, QOS);


    // Calculate expire time
    unsigned long expire_time_s_since_epoch = time(NULL) + 5;

    char *file_name = "vehicle-info.json";
    char *file_path = "/app/code/cl/vehicle-info.json";
    char *file_id = "wenjie-m7";

    readcansimulate( client, VEHICLE_COLLISION);

    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
    return rc;
}