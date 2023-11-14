#include <dot_builder.hpp>

extern "C" void LogFunctionCall__(char* callee_name, char* caller_name, long int value_addr) {
    printf("[LOG] CALL '%s' -> '%s' {%ld}\n", callee_name, caller_name, value_addr);
}

extern "C" void LogFuncRet__(char* func_name, long int value_addr) {
    printf("[LOG] End function '%s' {%ld}\n", func_name, value_addr);
}

