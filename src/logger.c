#include "logger.h"
#include "syscall_wrapper.h"
#include "string_utils.h"
#include "file_ops.h"

#define LOG_BUFFER_SIZE 256
#define FILE_LOG_BUFFER_SIZE 513

// 全局日志回调函数指针
//static file_info_t log_file = { .fd = -1, .file_size = 0, .mapped_memory = NULL };

// 注意：
// str_len, str_copy, str_append 等函数的本地实现已被移除。
// 我们现在依赖于 "string_utils.h" 中提供的统一实现，
// 以解决链接器报出的 "duplicate symbol" 错误。

// 日志级别字符串
static const char* level_strings[] = {
    "[DEBUG]",
    "[INFO] ",
    "[WARN] ",
    "[ERROR]"
};



// 十六进制转字符串
void hex_to_string(char* buffer, uintptr_t value, size_t buffer_size) {
    const char hex_chars[] = "0123456789abcdef";
    char temp[32];
    int pos = 0;
    
    // 处理0的特殊情况
    if (value == 0) {
        str_copy(buffer, "0", buffer_size);
        return;
    }
    
    // 转换为十六进制
    while (value > 0 && pos < 31) {
        temp[pos++] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    // 反转字符串
    int i = 0;
    while (pos > 0 && i < buffer_size - 1) {
        buffer[i++] = temp[--pos];
    }
    buffer[i] = '\0';
}

// 日期时间结构体
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} datetime_t;

// 判断是否为闰年
static int is_leap_year(int year) {
    if (year % 4 != 0) return 0;
    if (year % 100 != 0) return 1;
    if (year % 400 == 0) return 1;
    return 0;
}

// 将epoch时间转换为日期时间
static void epoch_to_datetime(time_t epoch, datetime_t *dt) {
    // 检查epoch是否为负数或超出合理范围(0~9999年)
    if (epoch < 0 || epoch > 253402300799LL) {
        dt->year = 1970;
        dt->month = 1;
        dt->day = 1;
        dt->hour = 0;
        dt->minute = 0;
        dt->second = 0;
        return;
    }

    // 从1970年开始计算
    dt->year = 1970;
    int days = epoch / 86400;
    int seconds = epoch % 86400;

    // 计算时分秒
    dt->hour = seconds / 3600;
    dt->minute = (seconds % 3600) / 60;
    dt->second = seconds % 60;

    // 计算年和天
    while (1) {
        int year_days = is_leap_year(dt->year) ? 366 : 365;
        if (days < year_days) break;
        days -= year_days;
        dt->year++;
    }

    // 计算月和日
    int month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (is_leap_year(dt->year)) month_days[1] = 29;

    dt->month = 0;
    while (dt->month < 12 && days >= month_days[dt->month]) {
        days -= month_days[dt->month];
        dt->month++;
    }
    dt->month++;
    dt->day = days + 1;
}

// 整数转字符串（带前导零）
static void int_to_string(int num, char *buffer, int min_digits) {
    int i = 0;
    int temp = num;
    char digits[16];

    // 处理0的情况
    if (temp == 0) {
        digits[i++] = '0';
    } else {
        // 转换数字为字符
        while (temp > 0) {
            digits[i++] = (temp % 10) + '0';
            temp /= 10;
        }
    }

    // 补前导零
    while (i < min_digits) {
        digits[i++] = '0';
    }

    // 反转字符串
    int j = 0;
    while (i > 0) {
        buffer[j++] = digits[--i];
    }
    buffer[j] = '\0';
}

// 时间戳格式化函数
static void format_timestamp(struct timespec *ts, char *buffer, size_t buffer_size) {
    datetime_t dt;
    epoch_to_datetime(ts->tv_sec, &dt);
    char temp[16];

    // YYYY-MM-DD HH:MM:SS
    str_copy(buffer, "[", buffer_size);

    int_to_string(dt.year, temp, 4);
    str_append(buffer, temp, buffer_size);
    str_append(buffer, "-", buffer_size);

    int_to_string(dt.month, temp, 2);
    str_append(buffer, temp, buffer_size);
    str_append(buffer, "-", buffer_size);

    int_to_string(dt.day, temp, 2);
    str_append(buffer, temp, buffer_size);
    str_append(buffer, " ", buffer_size);

    int_to_string(dt.hour, temp, 2);
    str_append(buffer, temp, buffer_size);
    str_append(buffer, ":", buffer_size);

    int_to_string(dt.minute, temp, 2);
    str_append(buffer, temp, buffer_size);
    str_append(buffer, ":", buffer_size);

    int_to_string(dt.second, temp, 2);
    str_append(buffer, temp, buffer_size);
    str_append(buffer, "]", buffer_size);
}

// 基础日志写入
void log_write(log_level_t level, const char* tag, const char* message) {

    // 获取当前时间戳
    struct timespec ts;
    int ret = sys_clock_gettime(CLOCK_REALTIME, &ts);
    if (ret != 0) {
        // 系统调用失败时使用默认时间
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
    }
    char timestamp[32];
    format_timestamp(&ts, timestamp, sizeof(timestamp));

    char log_buffer[512];

    // 构建日志消息: [时间戳] [LEVEL] TAG: message
    str_copy(log_buffer, timestamp, sizeof(log_buffer));
    str_append(log_buffer, " ", sizeof(log_buffer));
    str_append(log_buffer, level_strings[level], sizeof(log_buffer));
    str_append(log_buffer, " ", sizeof(log_buffer));
    str_append(log_buffer, tag, sizeof(log_buffer));
    str_append(log_buffer, ": ", sizeof(log_buffer));
    str_append(log_buffer, message, sizeof(log_buffer));



//    // 写入日志文件
//    if (log_file.fd != -1) {
//        char file_log_buffer[FILE_LOG_BUFFER_SIZE];
//        str_copy(file_log_buffer, log_buffer, sizeof(file_log_buffer));
//        str_append(file_log_buffer, "\n", sizeof(file_log_buffer));
//        file_write(&log_file, file_log_buffer, str_len(file_log_buffer));
//    }
}

// 十六进制值日志
void log_hex(log_level_t level, const char* tag, const char* prefix, uintptr_t value) {
    char message[256];
    char hex_str[32];
    
    str_copy(message, prefix, sizeof(message));
    str_append(message, "0x", sizeof(message));
    
    hex_to_string(hex_str, value, sizeof(hex_str));
    str_append(message, hex_str, sizeof(message));
    
    log_write(level, tag, message);
}

// 错误码日志
void log_error_code(log_level_t level, const char* tag, const char* operation, int error_code) {
    char message[256];
    char code_str[32];
    
    str_copy(message, operation, sizeof(message));
    str_append(message, " failed with code ", sizeof(message));
    
    hex_to_string(code_str, (uintptr_t)error_code, sizeof(code_str));
    str_append(message, code_str, sizeof(message));
    
    log_write(level, tag, message);
}

// 简单的格式化日志（支持基本的%s, %d, %x占位符）
void log_printf(log_level_t level, const char* tag, const char* format, ...) {
    // 由于shellcode环境限制，这里先实现基础版本
    // 实际项目中可以扩展支持更多格式化选项
    char message[512];
    str_copy(message, format, sizeof(message));
    log_write(level, tag, message);
}