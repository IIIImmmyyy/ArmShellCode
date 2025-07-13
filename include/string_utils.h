#ifndef SHELLCODE_STRING_UTILS_H
#define SHELLCODE_STRING_UTILS_H

#include <stddef.h> // for size_t
#include <stdint.h> // for uintptr_t

void str_copy(char *dest, const char *src, size_t dest_size);
void str_append(char* dest, const char* src, size_t max_len);
size_t str_len(const char* str);
int str_ends_with(const char* str, const char* suffix);
int str_compare(const char* s1, const char* s2);
int str_compare_n(const char* s1, const char* s2, size_t n);

// 新增函数声明
void hex_to_str(uintptr_t value, char* dest, size_t dest_size);
void format_log_string_with_hex(char* buf, size_t buf_size, const char* prefix, const char* str, const char* mid, uintptr_t hex_val);

#endif //SHELLCODE_STRING_UTILS_H 