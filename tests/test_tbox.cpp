#include <gtest/gtest.h>

extern "C" {
#include "../cantomqtt/control.h"
}

// gettemperature 是您要测试的函数
extern int gettemperature(char* json_string);

TEST(GetTemperatureTest, TestTemperature1) {
    // 使用一个 JSON 字符串
    char* json_string = "{\"canid\": \"0x200\",\"temperature\": \"25\"}";

    // 调用 gettemperature 函数
    int temperature = gettemperature(json_string);

    // 检查结果，这里假设正确的温度应该在一个合理的范围内
    EXPECT_GE(temperature, -40);
    EXPECT_LE(temperature, 50);
}

TEST(GetTemperatureTest, TestTemperature2) {
    // 使用另一个 JSON 字符串
    char* json_string = "{\"canid\": \"0x201\",\"temperature\": \"30\"}";

    // 调用 gettemperature 函数
    int temperature = gettemperature(json_string);

    // 检查结果
    EXPECT_GE(temperature, -40);
    EXPECT_LE(temperature, 50);
}

TEST(GetTemperatureTest, TestTemperature3) {
    // 使用另一个 JSON 字符串
    char* json_string = "{\"canid\": \"0x202\",\"temperature\": \"-5\"}";

    // 调用 gettemperature 函数
    int temperature = gettemperature(json_string);

    // 检查结果
    EXPECT_GE(temperature, -40);
    EXPECT_LE(temperature, 50);
}
