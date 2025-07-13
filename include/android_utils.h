#ifndef ANDROID_UTILS_H
#define ANDROID_UTILS_H

#include "syscall_wrapper.h"

// 前向声明，避免循环依赖
#ifdef __LP64__
    typedef unsigned long long Elf_Addr;
#else
    typedef unsigned int Elf_Addr;
#endif

// 字符串函数
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int strcmp(const char *s1, const char *s2);
char *strcat(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
size_t strlen(const char *str);
char *strstr(const char *haystack, const char *needle);
char *strchr(const char *s, int c);

// 数字转换
char *itoa(int value, char *str, int base);
int atoi(const char *str);
long atol(const char *str);

// 简单的数字转字符串函数（用于shellcode）
void simple_itoa(int value, char *buffer, size_t buffer_size);
void simple_itoa_hex(Elf_Addr value, char *buffer, size_t buffer_size);

// 网络字节序转换
unsigned int htonl(unsigned int hostlong);
unsigned short htons(unsigned short hostshort);
unsigned int ntohl(unsigned int netlong);
unsigned short ntohs(unsigned short netshort);

// 网络连接
int create_tcp_connection(unsigned int ip, unsigned short port);

// 文件操作
int file_exists(const char *path);
ssize_t read_file(const char *path, void *buffer, size_t size);
ssize_t write_file(const char *path, const void *buffer, size_t size);

// 十六进制转换
void bytes_to_hex(const unsigned char *bytes, size_t len, char *hex_str);
int hex_to_bytes(const char *hex_str, unsigned char *bytes, size_t max_len);

// 字符串解析
int parse_hex_address(const char *hex_str, Elf_Addr *addr);
char *skip_whitespace(char *str);
char *find_next_token(char *str, char delimiter);

// 高级封装函数 - 用于简化shellcode逻辑
typedef struct {
    char so_name[64];
    char symbol_name[64];
    Elf_Addr base_addr;
    Elf_Addr symbol_addr;
    int symbol_count;
} ShellcodeResult;

// 一站式符号查找函数
int shellcode_find_symbol(const char *so_name, const char *symbol_name, ShellcodeResult *result);

// Android logcat日志函数
void android_log_info(const char *tag, const char *message);
void android_log_error(const char *tag, const char *message);
void android_log_hex(const char *tag, const char *prefix, Elf_Addr value);
void android_log_debug(const char *tag, const char *message);

#endif // ANDROID_UTILS_H 