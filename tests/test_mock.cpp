#include <iostream>

#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"
#include <sys/socket.h>

extern "C" {
    #include "../cantomqtt/control.h"
}

//USING_NS_MOCKCPP;

//MOCKER(int(int), addBase);
//MOCK_METHOD(addBase, stubs().will(returnValue(100)));


void addBase(int c) {
    c = c+1;
}
int addBaseWithReturn(int c) {
    return c+1;
}

int add(int a,int b) {
    int c= a+b;

    addBase(c);
    return c;

    // int d = addBaseWithReturn(a);
    // return d;
}

//no mock test
TEST(suite1, addBase_test)
{
    int x = 0;
    addBase(x);
    EXPECT_EQ(0, x);
}

void innefunc(int var)
{
}
void testfunc()
{
    int var = 10;
    innefunc(var);
}

TEST(suite2, mock_addBase_test ) {
    MOCKER(addBase).stubs();//.will(returnValue(100));
    int c = add(1,2);
    EXPECT_EQ(3, c);
    GlobalMockObject::verify();
}

//mock inner function of function
TEST(suite3,innefunc_mock_test)
{
    int var;
    MOCKER(innefunc)
        .stubs()
        .with(eq(10));

    testfunc();
    ASSERT_EQ(var, 10);//test will fail
}

TEST(suite4, getlocation_mock_test)
{
    std::string expected = "GuangZhou Town, Maonan District, Maoming City, Guangdong Province";
    MOCKER(getlocation).stubs().will(returnValue(const_cast<char*>(expected.c_str())));
    char* adress = getlocation(0);//0:Fengtai District, Beijing City
    EXPECT_EQ("GuangZhou Town, Maonan District, Maoming City, Guangdong Province", std::string(adress));
}


TEST(suite5, getlocation_mock_test2)
{
    std::string expected = "GuangZhou Town, Maonan District, Maoming City, Guangdong Province";
    MOCKER(getlocation).stubs().will(returnValue(const_cast<char*>(expected.c_str())));
    char* adress = getGPRSLocaltion(0);//0:Fengtai District, Beijing City
    EXPECT_EQ("GuangZhou Town, Maonan District, Maoming City, Guangdong Province", std::string(adress));
}

// TEST(CanbusTest, TestCanSend) {
//     // 创建一个模拟对象
//     MOCKER(socket).stubs().will(returnValue(1));
//     //MOCKER(bind).stubs().will(returnValue(0));
//     //MOCKER(write).stubs().will(returnValue(4));//sizeof(struct can_frame)
//     //MOCKER(getenv).stubs().will(returnValue("1"));  
//     //MOCKER(json_loads).stubs().will(returnValue(nullptr));
//    // MOCKER(close).stubs().will(returnValue(0));
//      char* json_string = "{\"canid\": \"0x200\",\"temperature\": \"25\"}";
//     cansend_cardoor(json_string);//, FunctionNameOf(socket), FunctionNameOf(bind),FunctionNameOf(write), FunctionNameOf(close)
//    fprintf(stderr, "TestCanSend exec end.");
// }

