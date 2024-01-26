// conversion.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <jansson.h>

// 函数声明
char* decimalToHexadecimal(int decimalValue);
char* getBeijingTime();
char* write_can_temperature(char *json_string);
int gettemperature(char *json);
char* getcollision(int index);
char* getlocation(int index);
int getRand2();
int getRand99();
// 函数定义
char* decimalToHexadecimal(int decimalValue) {
    // 计算所需的字符串长度
    int len = snprintf(NULL, 0, "%X", decimalValue);

    // 分配足够的内存来存储十六进制字符串
    char* hexString = (char*)malloc(len + 1);

    // 将十进制数转换为十六进制字符串
    snprintf(hexString, len + 1, "%X", decimalValue);

    return hexString;
}





char* getBeijingTime() {
    // 获取当前时间
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);

    // 将时间调整为北京时区
    localTime = gmtime(&currentTime);
    localTime->tm_hour += 8;

    // 分配足够的内存来存储日期和时间字符串
    char *dateTimeString = (char *)malloc(20); // yyyy/MM/dd HH:MM:SS\0 共 20 个字符
    if (dateTimeString == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // 格式化日期和时间字符串
    snprintf(dateTimeString, 20, "%04d/%02d/%02d %02d:%02d:%02d",
             localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
             localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    return dateTimeString;
}



// 写入温度数据到can总线
char* write_can_temperature(char *json_string) {


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

    /* 设置过滤规则：不接受任何报文、仅发送数据 */
    setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);


    // -------------------------------解析json字符串
    json_error_t error;
    json_t *root = json_loads(json_string, 0, &error);

    if (root) {
        const char *canid = json_string_value(json_object_get(root, "canid"));
        if (strcasecmp(canid, "0x200") == 0) {

            int temp= gettemperature(json_string);
            /* 发送数据 */
            frame.data[0] = (temp >> 8) & 0xFF;  // 温度的高字节
            frame.data[1] = temp & 0xFF;         // 温度的低字节
            frame.data[2] = 0xC0;
            frame.data[3] = 0xD0;
//        frame.data[4] = 0xE0;
//        frame.data[5] = 0xF0;
            frame.can_dlc = 4;	//一次发送6个字节数据
            frame.can_id = 0x200;//帧ID为 0x200,标准帧 表示温度


            ret = write(sockfd, &frame, sizeof(frame)); //发送数据
            if(sizeof(frame) != ret) { //如果ret不等于帧长度，就说明发送失败
                perror("write error");
                goto out;
            }

            // 调用函数获取时间字符串
            char *beijingTime = getBeijingTime();
            // 输出时间字符串
            printf("%s ------发送温度数据到can总线\n", beijingTime);

        }

        json_decref(root);
    } else {
        fprintf(stderr, "JSON parsing error: %s\n", error.text);
    }

    out:
    /* 关闭套接字 */
    close(sockfd);
    //exit(EXIT_SUCCESS);
}

// 解析获得温度
int gettemperature(char *json_string) {
    //const char *json_string = "{\"canid\": \"0x200\",\"temperature\": \"25\"}";

    json_error_t error;
    json_t *root = json_loads(json_string, 0, &error);

    if (root) {
        const char *canid = json_string_value(json_object_get(root, "canid"));

            // 获取 "temperature" 字段的值
            json_t *temperature_json = json_object_get(root, "temperature");

            // 检查 "temperature" 字段的值类型是否是字符串
            if (json_is_string(temperature_json)) {
                // 将字符串转换为整数
                int temperature = atoi(json_string_value(temperature_json));

                printf("canid: %s\n", canid);
                printf("temperature: %d\n", temperature);
                return temperature;

            } else {
                fprintf(stderr, "\"temperature\" field is not a string.\n");
            }

        json_decref(root);
    } else {
        fprintf(stderr, "JSON parsing error: %s\n", error.text);
    }
    return 0;
}



//
char* getcollision(int index) {
    // 三个可能的事件
     char* events[] = {"气囊弹出", "车辆侧翻", "车辆涉水"};
    return events[index];
}


//
char* getlocation(int index) {

     char* events[] = {"北京市市辖区丰台区东铁匠营街道",
                            "贵州省六盘水市六枝特区梭戛苗族彝族回族乡",
                            "山西省晋城市陵川县马圪当乡",
                            "浙江省湖州市吴兴区仁皇山街道",
                            "海南省海口市秀英区海秀街道办",
                            "安徽省黄山市屯溪区屯光镇",
                            "湖南省郴州市北湖区月峰瑶族乡",
                            "上海市市辖区金山区吕巷镇",
                            "山西省晋中市左权县麻田镇",
                            "上海市市辖区徐汇区田林街道",
                            "湖南省株洲市茶陵县界首镇",
                            "四川省内江市威远县向义镇",
                            "黑龙江省黑河市北安市海星镇",
                            "浙江省衢州市江山市长台镇",
                            "广西壮族自治区贺州市八步区桂岭镇",
                            "黑龙江省大庆市林甸县林甸县长青林场",
                            "北京市市辖区朝阳区双井街道",
                            "上海市市辖区嘉定区新成路街道",
                            "四川省甘孜藏族自治州得荣县徐龙乡",
                            "安徽省淮北市烈山区烈山区工业园",
                            "河南省鹤壁市鹤山区中北街道",
                            "河南省安阳市汤阴县任固镇",
                            "河南省开封市杞县葛岗镇",
                            "黑龙江省哈尔滨市道里区经纬街道",
                            "江苏省淮安市淮安区林集镇",
                            "黑龙江省大兴安岭地区塔河县十八站林业局",
                            "山东省聊城市冠县斜店乡",
                            "湖南省长沙市开福区湘雅路街道",
                            "吉林省白城市洮北区三合乡",
                            "黑龙江省鹤岗市工农区团结街道",
                            "贵州省黔西南布依族苗族自治州兴仁县鲁础营乡",
                            "浙江省台州市天台县南屏乡",
                            "天津市市辖区东丽区军粮城街道",
                            "江西省九江市武宁县万福经济技术开发区管委会",
                            "黑龙江省双鸭山市尖山区富安街道",
                            "重庆市市辖区北碚区柳荫镇",
                            "浙江省金华市武义县王宅镇",
                            "宁夏回族自治区银川市永宁县望洪镇",
                            "辽宁省葫芦岛市南票区邱皮沟街道",
                            "山东省威海市环翠区竹岛街道",
                            "内蒙古自治区阿拉善盟阿拉善左旗温都尔勒图镇",
                            "河南省新乡市牧野区和平路街道",
                            "黑龙江省大兴安岭地区呼玛县鸥浦乡",
                            "河北省邢台市柏乡县王家庄乡",
                            "宁夏回族自治区吴忠市同心县王团镇",
                            "黑龙江省伊春市伊春区东升街道",
                            "吉林省通化市辉南县辉发城镇",
                            "青海省黄南藏族自治州泽库县宁秀乡",
                            "山东省青岛市市南区珠海路街道",
                            "陕西省安康市石泉县云雾山镇",
                            "重庆市市辖区江北区复盛镇人民政府",
                            "湖北省武汉市江夏区郑店街道",
                            "青海省果洛藏族自治州玛多县扎陵湖乡",
                            "四川省成都市青羊区黄田坝街道",
                            "北京市市辖区石景山区广宁街道",
                            "内蒙古自治区呼伦贝尔市根河市阿龙山镇",
                            "吉林省白山市抚松县兴参镇",
                            "安徽省安庆市潜山县源潭镇",
                            "湖北省孝感市云梦县道桥镇",
                            "西藏自治区阿里地区札达县底雅乡",
                            "辽宁省锦州市北镇市闾阳镇",
                            "吉林省松原市乾安县大遐畜牧场",
                            "宁夏回族自治区中卫市中宁县喊叫水乡",
                            "陕西省安康市石泉县迎丰镇",
                            "黑龙江省牡丹江市东安区七星街道",
                            "辽宁省锦州市黑山县常兴镇",
                            "广东省江门市江海区礼乐街道",
                            "四川省泸州市古蔺县桂花乡",
                            "河南省许昌市鄢陵县南坞乡",
                            "贵州省黔西南布依族苗族自治州普安县窝沿乡",
                            "广东省茂名市茂南区山阁镇",
                            "河南省许昌市魏都区七里店街道",
                            "上海市市辖区浦东新区东海农场",
                            "云南省曲靖市陆良县中枢镇",
                            "福建省福州市罗源县霍口畲族乡",
                            "浙江省嘉兴市海盐县秦山街道",
                            "四川省达州市大竹县张家乡",
                            "甘肃省陇南市礼县石桥镇",
                            "河北省廊坊市固安县固安镇",
                            "江苏省泰州市高港区泰州市高港区科技创业园管理委员会",
                            "福建省泉州市惠安县惠南工业区",
                            "四川省德阳市旌阳区城北街道",
                            "黑龙江省牡丹江市穆棱市八面通镇",
                            "宁夏回族自治区中卫市中宁县白马乡",
                            "海南省省直辖县级行政区划万宁市长丰镇",
                            "贵州省贵阳市乌当区逸景社区服务中心",
                            "贵州省铜仁市印江土家族苗族自治县新寨乡",
                            "青海省玉树藏族自治州治多县治渠乡",
                            "山东省潍坊市安丘市金冢子镇",
                            "黑龙江省大兴安岭地区呼玛县北疆乡",
                            "吉林省吉林市舒兰市法特镇",
                            "湖北省荆门市京山县罗店镇",
                            "重庆市县彭水苗族土家族自治县万足镇",
                            "青海省西宁市湟源县巴燕乡",
                            "河南省省直辖县级行政区划济源市济源市轵城镇",
                            "陕西省宝鸡市麟游县常丰镇",
                            "广西壮族自治区北海市合浦县乌家镇",
                            "湖北省恩施土家族苗族自治州利川市都亭街道",
                            "四川省达州市渠县东安乡",
                            "山东省菏泽市成武县成武县九女集镇"};


    return events[index];
}

//
int getRand2() {
    srand(time(NULL));
    int randomValue = rand();
    // 将随机数映射到0、1、2范围
    int result = randomValue % 3;
    return result;
}


//
int getRand99() {
    srand(time(NULL));
    int min=0;
    int max=99;
    // 生成随机数（0到RAND_MAX之间）
    int randomValue = rand();
    // 计算范围内的随机数
    int result = min + randomValue % (max - min + 1);
    return result;
}
