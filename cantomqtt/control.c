
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

// Function declarations
char* getEnvVariable(char* envVarName);
char* decimalToHexadecimal(int decimalValue);
char* getBeijingTime();
char* cansend_cardoor(char* json_string);
int gettemperature(char* json);
char* getcollision(int index);
char* getGPRSLocaltion(int index);
char* sendGPRSRequest(const char* requestType, int index);
char* parseLocation(const char* gprsResponse);
char* getlocation(int index);
int getRand2();
int getRand99();

// Function definitions
char* getEnvVariable(char* envVarName) {
    // Use getenv to get the value of the environment variable
    char *envVarValue = getenv(envVarName);
    // Check if the environment variable exists
    if (envVarValue != NULL) {
        printf("Env Value of %s: %s\n", envVarName, envVarValue);
    } else {
        printf("%s is not defined.\n", envVarName);
    }
    return envVarValue;
}

char* decimalToHexadecimal(int decimalValue) {
    // Calculate the required string length
    int len = snprintf(NULL, 0, "%X", decimalValue);

    // Allocate enough memory to store the hexadecimal string
    char* hexString = (char*)malloc(len + 1);

    // Convert the decimal number to a hexadecimal string
    snprintf(hexString, len + 1, "%X", decimalValue);

    return hexString;
}

char* getBeijingTime() {
    // Get the current time
    time_t currentTime;
    struct tm *localTime;
    time(&currentTime);

    // Adjust the time to Beijing timezone
    localTime = gmtime(&currentTime);
    localTime->tm_hour += 8;

    // Allocate enough memory to store the date and time string
    char *dateTimeString = (char *)malloc(20); // yyyy/MM/dd HH:MM:SS\0 total 20 characters
    if (dateTimeString == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Format the date and time string
    snprintf(dateTimeString, 20, "%04d/%02d/%02d %02d:%02d:%02d",
             localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
             localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    return dateTimeString;
}

// Write data to the CAN bus
char* cansend_cardoor(char* json_string) {
    
    perror("cansend_cardoor Not Implementation !");
    fprintf(stderr, "Recive JSON : %s\n", json_string);
    
}

// Parse
int gettemperature(char* json_string) {
    //const char *json_string = "{\"canid\": \"0x200\",\"temperature\": \"25\"}";

    json_error_t error;
    json_t *root = json_loads(json_string, 0, &error);

    if (root) {
        const char *canid = json_string_value(json_object_get(root, "canid"));

        // Get the value of the "temperature" field
        json_t *temperature_json = json_object_get(root, "temperature");

        // Check if the value of the "temperature" field is a string
        if (json_is_string(temperature_json)) {
            // Convert the string to an integer
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
    // Three possible events
    char* events[] = {"Airbag deployed", "Vehicle rollover", "Vehicle water immersion"};
    return events[index];
}

char* getGPRSLocaltion(int index) {
   // Check if the index is valid
   if (index < 0 || index >= 117) {
      return "Invalid index"; // Return an error message if the index is invalid
   }

   // Simulate GPRS module initialization
   initializeGPRSModule();

   // Simulate sending a request to the GPRS module to obtain location information
   char* gprsResponse = sendGPRSRequest("GET_LOCATION", index);

   // Check the response from the GPRS module
   if (gprsResponse == NULL || strcmp(gprsResponse, "ERROR") == 0) {
      return "GPRS module error"; // Return an error message if there is an error with the GPRS module
   }

   // Parse the location information returned by the GPRS module
   char* location = parseLocation(gprsResponse);
   
   // Check if the location information was successfully parsed
   if (location == NULL) {
      return "Location parse error"; // Return an error message if there is a parsing error
   }

   // Return the parsed location information
   return getlocation(index);
}

// Simulate GPRS module initialization function
void initializeGPRSModule() {
   // Initialize the GPRS module
}

// Simulate sending a GPRS request function
char* sendGPRSRequest(const char* requestType, int index) {
   //  Send a request to the GPRS module and get a response
   return "Mocked GPRS Response";
}

// Simulate parsing location information function
char* parseLocation(const char* gprsResponse) {
   // Parse the location information from the GPRS response
   return "Mocked Location Info";
}

char* getlocation(int index) {

     char* events[] = {"Fengtai District, Beijing City",
                            "Suojia Miao, Yi and Hui Ethnic Township, Liuzhi Special District, Liupanshui City, Guizhou Province",
                            "Magangdang Township, Lingchuan County, Jincheng City, Shanxi Province",
                            "Renhuangshan Subdistrict, Wuxing District, Huzhou City, Zhejiang Province",
                            "Haixiu Subdistrict Office, Xiuying District, Haikou City, Hainan Province",
                            "Tunguang Town, Tunxi District, Huangshan City, Anhui Province",
                            "Yuefeng Yao Ethnic Township, Beihu District, Chenzhou City, Hunan Province",
                            "Luxiang Town, Jinshan District, Shanghai City",
                            "Matian Town, Zuoquan County, Jinzhong City, Shanxi Province",
                            "Tianlin Subdistrict, Xuhui District, Shanghai City",
                            "Jieshou Town, Chaling County, Zhuzhou City, Hunan Province",
                            "Xiangyi Town, Weiyuan County, Neijiang City, Sichuan Province",
                            "Haixing Town, Bei'an City, Heihe City, Heilongjiang Province",
                            "Changtai Town, Jiangshan City, Quzhou City, Zhejiang Province",
                            "Guiling Town, Babu District, Hezhou City, Guangxi Zhuang Autonomous Region",
                            "Changqing Forestry Farm, Lindian County, Daqing City, Heilongjiang Province",
                            "Shuangjing Subdistrict, Chaoyang District, Beijing City",
                            "Xincheng Road Subdistrict, Jiading District, Shanghai City",
                            "Xulong Township, Derong County, Garze Tibetan Autonomous Prefecture, Sichuan Province",
                            "Industrial Park, Lishan District, Huaibei City, Anhui Province",
                            "Zhongbei Subdistrict, Heshan District, Hebi City, Henan Province",
                            "Rengu Town, Tangyin County, Anyang City, Henan Province",
                            "Gegang Town, Qi County, Kaifeng City, Henan Province",
                            "Jingwei Subdistrict, Daoli District, Harbin City, Heilongjiang Province",
                            "Linji Town, Huai'an District, Huai'an City, Jiangsu Province",
                            "Eighteen Station Forestry Bureau, Tahe County, Daxing'anling Prefecture, Heilongjiang Province",
                            "Xiedian Township, Guan County, Liaocheng City, Shandong Province",
                            "Xiangya Road Subdistrict, Kaifu District, Changsha City, Hunan Province",
                            "Sanhe Township, Taobei District, Baicheng City, Jilin Province",
                            "Tuanjie Subdistrict, Gongnong District, Hegang City, Heilongjiang Province",
                            "Luchu Camp Township, Xingren County, Qianxinan Buyi and Miao Autonomous Prefecture, Guizhou Province",
                            "Nanping Township, Tiantai County, Taizhou City, Zhejiang Province",
                            "Junliangcheng Subdistrict, Dongli District, Tianjin City",
                            "Wanfu Economic and Technological Development Zone Management Committee, Wuning County, Jiujiang City, Jiangxi Province",
                            "Fu'an Subdistrict, Jianshan District, Shuangyashan City, Heilongjiang Province",
                            "Liuyin Town, Beibei District, Chongqing City",
                            "Wangzai Town, Wuyi County, Jinhua City, Zhejiang Province",
                            "Wanghong Town, Yongning County, Yinchuan City, Ningxia Hui Autonomous Region",
                            "Qiupigou Subdistrict, Nanpiao District, Huludao City, Liaoning Province",
                            "Zhudao Subdistrict, Huancui District, Weihai City, Shandong Province",
                            "Wendouerletu Town, Alxa Left Banner, Alxa League, Inner Mongolia Autonomous Region",
                            "Peace Road Subdistrict, Muye District, Xinxiang City, Henan Province",
                            "Oupu Township, Huma County, Daxing'anling Prefecture, Heilongjiang Province",
                            "Wangjiazhuang Township, Baixiang County, Xingtai City, Hebei Province",
                            "Wangtuan Town, Tongxin County, Wuzhong City, Ningxia Hui Autonomous Region",
                            "Dongsheng Subdistrict, Yichun District, Yichun City, Heilongjiang Province",
                            "Huifacheng Town, Huinan County, Tonghua City, Jilin Province",
                            "Ningxiu Township, Zeku County, Huangnan Tibetan Autonomous Prefecture, Qinghai Province",
                            "Zhuhai Road Subdistrict, Shinan District, Qingdao City, Shandong Province",
                            "Yunwu Mountain Town, Shiquan County, Ankang City, Shaanxi Province",
                            "Fusheng Town People's Government, Jiangbei District, Chongqing City",
                            "Zhengdian Subdistrict, Jiangxia District, Wuhan City, Hubei Province",
                            "Zhalin Lake Township, Maduo County, Guoluo Tibetan Autonomous Prefecture, Qinghai Province",
                            "Huangtianba Subdistrict, Qingyang District, Chengdu City, Sichuan Province",
                            "Guangning Subdistrict, Shijingshan District, Beijing City",
                            "Alongshan Town, Genhe City, Hulunbuir City, Inner Mongolia Autonomous Region",
                            "Xingcan Town, Fushun County, Baishan City, Jilin Province",
                            "Yuantan Town, Qianshan County, Anqing City, Anhui Province",
                            "Daoqiao Town, Yunmeng County, Xiaogan City, Hubei Province",
                            "Diyaxiang Township, Zada County, Ali Region, Tibet Autonomous Region",
                            "Luyang Town, Beizhen City, Jinzhou City, Liaoning Province",
                            "Daxia Livestock Farm, Qian'an County, Songyuan City, Jilin Province",
                            "Hanjiashui Township, Zhongning County, Zhongwei City, Ningxia Hui Autonomous Region",
                            "Yingfeng Town, Shiquan County, Ankang City, Shaanxi Province",
                            "Qixing Subdistrict, Dong'an District, Mudanjiang City, Heilongjiang Province",
                            "Changxing Town, Heishan County, Jinzhou City, Liaoning Province",
                            "Liyue Subdistrict, Jianghai District, Jiangmen City, Guangdong Province",
                            "Guihua Township, Gulin County, Luzhou City, Sichuan Province",
                            "Nanwu Township, Yanling County, Xuchang City, Henan Province",
                            "Woyan Township, Puan County, Qianxinan Buyi and Miao Autonomous Prefecture, Guizhou Province",
                            "Shange Town, Maonan District, Maoming City, Guangdong Province",
                            "Qilidian Subdistrict, Weidu District, Xuchang City, Henan Province",
                            "Donghai Farm, Pudong New Area, Shanghai City",
                            "Zhushu Town, Luliang County, Qujing City, Yunnan Province",
                            "Huokou She Ethnic Township, Luoyuan County, Fuzhou City, Fujian Province",
                            "Qinshan Subdistrict, Haiyan County, Jiaxing City, Zhejiang Province",
                            "Zhangjia Township, Dazhu County, Dazhou City, Sichuan Province",
                            "Shiqiao Town, Li County, Longnan City, Gansu Province",
                            "Gu'an Town, Gu'an County, Langfang City, Hebei Province",
                            "Science and Technology Entrepreneurship Park Management Committee, Gaogang District, Taizhou City, Jiangsu Province",
                            "Huinan Industrial Zone, Huian County, Quanzhou City, Fujian Province",
                            "Chengbei Subdistrict, Jingyang District, Deyang City, Sichuan Province",
                            "Bamiantong Town, Muling City, Mudanjiang City, Heilongjiang Province",
                            "Baima Township, Zhongning County, Zhongwei City, Ningxia Hui Autonomous Region",
                            "Changfeng Town, Wanning City, Hainan Province",
                            "Yijing Community Service Center, Wudang District, Guiyang City, Guizhou Province",
                            "Xinzhai Township, Yinjiang Tujia and Miao Autonomous County, Tongren City, Guizhou Province",
                            "Zhiqu Township, Zhiduo County, Yushu Tibetan Autonomous Prefecture, Qinghai Province",
                            "Jindizai Town, Anqiu City, Weifang City, Shandong Province",
                            "Beijiang Township, Huma County, Daxing'anling Prefecture, Heilongjiang Province",
                            "Fate Town, Shulan City, Jilin City, Jilin Province",
                            "Luodian Town, Jingshan County, Jingmen City, Hube Province",
                            "Wanbu Town, Pengshui Miao and Tujia Autonomous County, Chongqing City",
                            "Baiyan Township, Huangyuan County, Xining City, Qinghai Province",
                            "Zucheng Town, Jiyuan City, Henan Province",
                            "Yingfeng Town, Shiquan County, Ankang City, Shaanxi Province",
                            "Wujia Town, Hepu County, Beihai City, Guangxi Zhuang Autonomous Region",
                            "Renwang Subdistrict, Enshi Tujia and Miao Autonomous Prefecture, Hubei Province",
                            "Dongan Township, Qu County, Dazhou City, Sichuan Province",
                            "Wangjiazhuang Township, Baixiang County, Xingtai City, Hebei Province",
                            "Baima Township, Zhongning County, Zhongwei City, Ningxia Hui Autonomous Region",
                            "Changfeng Town, Wanning City, Hainan Province",
                            "Yijing Community Service Center, Wudang District, Guiyang City, Guizhou Province",
                            "Xinzhai Township, Yinjiang Tujia and Miao Autonomous County, Tongren City, Guizhou Province",
                            "Zhiqu Township, Zhiduo County, Yushu Tibetan Autonomous Prefecture, Qinghai Province",
                            "Jindizai Town, Anqiu City, Weifang City, Shandong Province",
                            "Beijiang Township, Huma County, Daxing'anling Prefecture, Heilongjiang Province",
                            "Fate Town, Shulan City, Jilin City, Jilin Province",
                            "Luodian Town, Jingshan County, Jingmen City, Hubei Province",
                            "Wanbu Town, Pengshui Miao and Tujia Autonomous County, Chongqing City",
                            "Baiyan Township, Huangyuan County, Xining City, Qinghai Province",
                            "Zucheng Town, Jiyuan City, Henan Province",
                            "Yingfeng Town, Shiquan County, Ankang City, Shaanxi Province",
                            "Wujia Town, Hepu County, Beihai City, Guangxi Zhuang Autonomous Region",
                            "Renwang Subdistrict, Enshi Tujia and Miao Autonomous Prefecture, Hubei Province",
                            "Dongan Township, Qu County, Dazhou City, Sichuan Province"};

    return events[index];
}


//
int getRand2() {
    srand(time(NULL));
    int randomValue = rand();
    // Map the random number to the range 0, 1, 2
    int result = randomValue % 3;
    return result;
}

//
int getRand99() {
    srand(time(NULL));
    int min=0;
    int max=99;
    // Generate a random number (0 to RAND_MAX)
    int randomValue = rand();
    // Calculate a random number within the range
    int result = min + randomValue % (max - min + 1);
    return result;
}