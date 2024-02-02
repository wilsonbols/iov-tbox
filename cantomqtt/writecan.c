/***************************************************************

***************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include "common/conversion.h"  // 包含转换函数的头文件



// 生成随机转速值（范围：1000 到 7000 RPM）
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

    /* 打开套接字 */
    sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(0 > sockfd) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /* 指定can0设备 */
    strcpy(ifr.ifr_name, "can0");
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

    /* 设置过滤规则：不接受任何报文、仅发送数据 */
    setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    // 模拟随机转速
 //   int engineRPM = generateRandomRPM();

    /* 发送数据 */
    //frame.data[0] = 0x0B;   //一个字节的范围是 0 到 255
    //frame.data[1] = 0xB8;

//    frame.data[0] = (engineRPM >> 8) & 0xFF;  // 发动机转数的高字节
//    frame.data[1] = engineRPM & 0xFF;         // 发动机转数的低字节
//    frame.data[2] = 0xC0;
//    frame.data[3] = 0xD0;
//    frame.data[4] = 0xE0;
//    frame.data[5] = 0xF0;
//    frame.can_dlc = 6;	//一次发送6个字节数据
//    frame.can_id = 0x123;//帧ID为 0x123,标准帧



    for ( ; ; ) {

        // 模拟随机转速
        int engineRPM = generateRandomRPM();
        /* 发送数据 */
        //frame.data[0] = 0x0B;   //一个字节的范围是 0 到 255
        //frame.data[1] = 0xB8;
        frame.data[0] = (engineRPM >> 8) & 0xFF;  // 发动机转数的高字节
        frame.data[1] = engineRPM & 0xFF;         // 发动机转数的低字节
        frame.data[2] = getRand2();
        frame.data[3] = getRand99();
        frame.data[4] = 0xE0;
        frame.data[5] = 0xF0;
        frame.can_dlc = 6;	//一次发送6个字节数据
        frame.can_id = 0x123;//帧ID为 0x123,标准帧





        ret = write(sockfd, &frame, sizeof(frame)); //发送数据
        if(sizeof(frame) != ret) { //如果ret不等于帧长度，就说明发送失败
            perror("write error");
            goto out;
        }

        // 调用函数获取时间字符串
        char *beijingTime = getBeijingTime();
        // 输出时间字符串
        printf("%s ------发送数据\n", beijingTime);
        // printf(" 发送数据\n");
        sleep(20);		//一秒钟发送一次
    }

    out:
    /* 关闭套接字 */
    close(sockfd);
    exit(EXIT_SUCCESS);
}
