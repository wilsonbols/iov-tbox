

Here is the translated code:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include "control.h"  // Include the header file of the conversion function

// Generate a random RPM value (range: 1000 to 7000 RPM)
int generateRandomRPM() {
    return rand() % (7000 - 1000 + 1) + 1000;
}

int main(void)
{
    struct ifreq ifr = {0};
    struct sockaddr_can can_addr = {0};
    struct can_frame frame = {0};
    int sockfd = -1;
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

    /* Set filter rules: do not accept any message, only send data */
    setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    for ( ; ; ) {

        // Simulate random RPM
        int engineRPM = generateRandomRPM();
        /* Send data */
        frame.data[0] = (engineRPM >> 8) & 0xFF;  // High byte of engine RPM
        frame.data[1] = engineRPM & 0xFF;         // Low byte of engine RPM
        frame.data[2] = getRand2();
        frame.data[3] = getRand99();
        frame.data[4] = 0xE0;
        frame.data[5] = 0xF0;
        frame.can_dlc = 6;    // Send 6 bytes of data
        frame.can_id = 0x123; // Frame ID is 0x123, standard frame

        ret = write(sockfd, &frame, sizeof(frame)); // Send data
        if(sizeof(frame) != ret) { // If ret is not equal to the frame length, it means the sending failed
            perror("write error");
            goto out;
        }

        // Call the function to get the time string        char *beijingTime = getBeijingTime();
        // Output the time string
        printf("%s ------Sending data: data[0] `%d` data[1] `%d` data[2] `%d` data[3] `%d`\n", beijingTime, frame.data[0], frame.data[1], frame.data[2], frame.data[3]);
        // printf("Sending data\n");
        sleep(20);     // Send once per second
    }

    out:
    /* Close socket */
    close(sockfd);
    exit(EXIT_SUCCESS);
}
