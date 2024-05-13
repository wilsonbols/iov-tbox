

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>

int main(void)
{
    struct ifreq ifr = {0};
    struct sockaddr_can can_addr = {0};
    struct can_frame frame = {0};
    int sockfd = -1;
    int i;
    int ret;

    /* open socket */
    sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(0 > sockfd) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* set can device */
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sockfd, SIOCGIFINDEX, &ifr);
    can_addr.can_family = AF_CAN;
    can_addr.can_ifindex = ifr.ifr_ifindex;

    /*  bind can0 to the socket */ 
    ret = bind(sockfd, (struct sockaddr *)&can_addr, sizeof(can_addr));
    if (0 > ret) {
        perror("bind error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* 设置过滤规则 */
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
        if (frame.can_id & CAN_EFF_FLAG) //Extended frame
            printf("Extended frame <0x%08x> ", frame.can_id & CAN_EFF_MASK);
        else //Standard frame
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
        // Parse engine RPM data
        int engineRPM = (frame.data[0] << 8) | frame.data[1];
        printf("Received Engine RPM: %d\n", engineRPM);
        //printf("\n");
    }

    /* Close socket */
    close(sockfd);
    exit(EXIT_SUCCESS);
}
