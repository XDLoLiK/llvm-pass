#include <stdio.h>

extern "C" void funcStartLogger(char* funcName)  {
    printf("[LOG] Start function '%s'\n", funcName);
}

extern "C" void funcEndLogger(char* funcName, long int valID) {
    printf("[LOG] End function '%s' {%ld}\n", funcName, valID);
}

extern "C" void binOptLogger(int val, int arg0, int arg1, char* opName, char* funcName, long int valID) {
    printf("[LOG] In function '%s': %d = %d %s %d {%ld}\n", funcName, val, arg0, opName, arg1, valID);
}

extern "C" void callLogger(char* calleeName, char* callerName, long int valID) {
    printf("[LOG] CALL '%s' -> '%s' {%ld}\n", calleeName, callerName, valID);
}
