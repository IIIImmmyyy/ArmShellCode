#ifndef DOBBY_MAPS_PARSER_H
#define DOBBY_MAPS_PARSER_H

#include <stddef.h>
#include <stdint.h>

#define MAX_PATH_LEN 256

// 错误码
#define MAPS_PARSER_SUCCESS       0
#define MAPS_PARSER_ERR_NOT_FOUND -1
#define MAPS_PARSER_ERR_OPEN_FAILED -2
#define MAPS_PARSER_ERR_READ_FAILED -3
#define MAPS_PARSER_ERR_PARSE_FAILED -4

// 模仿 Dobby 的 RuntimeModule 结构体
typedef struct {
    char path[MAX_PATH_LEN];
    uintptr_t load_address;
} dobby_runtime_module_t;

/**
 * @brief 通过流式解析 /proc/self/maps 查找库信息。
 * 
 * @param name 目标库的文件名，例如 "libGuardCore.so"。
 * @param module [out] 用于存储找到的库信息的结构体指针。
 * @return MAPS_PARSER_SUCCESS 或错误码。
 */
int dobby_get_runtime_module(const char* name, dobby_runtime_module_t* module);

#endif // DOBBY_MAPS_PARSER_H 