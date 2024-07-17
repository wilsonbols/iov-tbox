

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

    /* Open socket */
    sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(0 > sockfd) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* Specify can0 device */
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sockfd, SIOCGIFINDEX, &ifr);
    can_addr.can_family = AF_CAN;
    can_addr.can_ifindex = ifr.ifr_ifindex;

    /* Bind can0 to the socket */
    ret = bind(sockfd, (struct sockaddr *)&can_addr, sizeof(can_addr));
    if (0 > ret) {
        perror("bind error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* Set filter rules */
    //setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    /* Receive data */
    for ( ; ; ) {
        if (0 > read(sockfd, &frame, sizeof(struct can_frame))) {
            perror("read error");
            break;
        }

        /* Check if an error frame is received */
        if (frame.can_id & CAN_ERR_FLAG) {
            printf("Error frame!\n");
            break;
        }

        /* Check frame format */
        if (frame.can_id & CAN_EFF_FLAG)    //Extended frame
            printf("Extended frame <0x%08x> ", frame.can_id & CAN_EFF_MASK);
        else        //Standard frame
            printf("Standard frame <0x%03x> ", frame.can_id & CAN_SFF_MASK);

        /* Check frame type: data frame or remote frame */
        if (frame.can_id & CAN_RTR_FLAG) {
            printf("remote request\n");
            continue;
        }

        /* Print data length */
        printf("[%d] ", frame.can_dlc);

        /* Print data */
                for (i = 0; i < frame.can_dlc; i++){
            printf("%02x ", frame.data[i]);
        }
        printf("\n");

        // Call function to get time string
        char *beijingTime = getBeijingTime();

        if (frame.can_id == 0x123) {

            // Parse
            int engineRPM = (frame.data[0] << 8) | frame.data[1];
            char prmstr[50];
            // Use sprintf function to convert integer to string
            sprintf(prmstr, "Vehicle engine RPM:%d", engineRPM);
            // Create JSON object
            json_t *root = json_object();
            // Add key-value pairs to JSON object
            json_object_set_new(root, "vin", json_string("NH2FX13D223ES1340"));
            json_object_set_new(root, "collisioninfo", json_string(getcollision(frame.data[2])));
            json_object_set_new(root, "locationinfo", json_string(getlocation(frame.data[3])));
            json_object_set_new(root, "descriptioninfo", json_string(prmstr));

            // Convert JSON object to string
            char *jsonString = json_dumps(root, JSON_ENCODE_ANY);

            // Output JSON string
            //printf("%s\n", jsonString);

            mqtt_publish(client, topic, jsonString);
            // Release memory
            free(jsonString);
            json_decref(root);
        }

        // Check if CAN ID is 0x200
        if (frame.can_id == 0x200) {
            // Parse data
            int action = frame.data[1];
            printf("%s------Received switch door information from CAN bus: %d \n",beijingTime, action);
        }
        printf("\n");
        sleep(20);   //Delay

    }
    /* Close socket */
    close(sockfd);
}



// Publish message
void mqtt_publish(MQTTClient client, char *topic, char *payload) {
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = strlen(payload);
    message.qos = 0;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, 10000L);
    // Call function to get time string
    char *beijingTime = getBeijingTime();
    // Output
    printf("%s------Send \n`%s`\n to topic `%s` \n",beijingTime, payload, topic);
}