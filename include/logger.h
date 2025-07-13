#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>
#include <stdint.h>

// 日志级别
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
} log_level_t;


// 日志系统初始化
//void logger_init();

// 基础日志函数
void log_write(log_level_t level, const char* tag, const char* message);

// 便利宏
#define LOG_D(tag, msg) log_write(LOG_DEBUG, tag, msg)
#define LOG_I(tag, msg) log_write(LOG_INFO, tag, msg)
#define LOG_W(tag, msg) log_write(LOG_WARN, tag, msg)
#define LOG_E(tag, msg) log_write(LOG_ERROR, tag, msg)

// 格式化日志函数
void log_printf(log_level_t level, const char* tag, const char* format, ...);
void log_hex(log_level_t level, const char* tag, const char* prefix, uintptr_t value);
void log_error_code(log_level_t level, const char* tag, const char* operation, int error_code);

// 便利格式化宏
#define LOG_DF(tag, fmt, ...) log_printf(LOG_DEBUG, tag, fmt, __VA_ARGS__)
#define LOG_IF(tag, fmt, ...) log_printf(LOG_INFO, tag, fmt, __VA_ARGS__)
#define LOG_WF(tag, fmt, ...) log_printf(LOG_WARN, tag, fmt, __VA_ARGS__)
#define LOG_EF(tag, fmt, ...) log_printf(LOG_ERROR, tag, fmt, __VA_ARGS__)

// 十六进制日志便利宏
#define LOG_HEX(level, tag, prefix, value) log_hex(level, tag, prefix, value)
#define LOG_I_HEX(tag, prefix, value) LOG_HEX(LOG_INFO, tag, prefix, value)
#define LOG_E_HEX(tag, prefix, value) LOG_HEX(LOG_ERROR, tag, prefix, value)

// 错误码日志便利宏
#define LOG_ERR(level, tag, op, code) log_error_code(level, tag, op, code)
#define LOG_I_ERR(tag, op, code) LOG_ERR(LOG_INFO, tag, op, code)
#define LOG_E_ERR(tag, op, code) LOG_ERR(LOG_ERROR, tag, op, code)

// 内部字符串操作函数
void str_copy(char* dest, const char* src, size_t max_len);
void str_append(char* dest, const char* src, size_t max_len);
size_t str_len(const char* str);
void hex_to_string(char* buffer, uintptr_t value, size_t buffer_size);

#endif // LOGGER_H 