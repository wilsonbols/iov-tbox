// control.h

#ifndef CONTROL_H
#define CONTROL_H

// Function declarations
char* getEnvVariable(char* envVarName);

char* decimalToHexadecimal(int decimalValue);

char* getBeijingTime();

char* cansend_cardoor(char* json_string);

int gettemperature(char* json_string);

char* getcollision(int index);
char* getlocation(int index);
char* getGPRSLocaltion(int index);

int getRand2();

int getRand99();

#endif  // CONTROL_H