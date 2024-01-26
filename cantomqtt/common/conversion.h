// conversion.h

#ifndef CONVERSION_H
#define CONVERSION_H

// 函数声明
char* decimalToHexadecimal(int decimalValue);

char* getBeijingTime();

char* write_can_temperature(char *json_string);

int gettemperature(char *json_string);

char* getcollision(int index);
char* getlocation(int index);

int getRand2();

int getRand99();

#endif  // CONVERSION_H