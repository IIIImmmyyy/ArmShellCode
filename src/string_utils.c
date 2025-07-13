#include "string_utils.h"

void str_copy(char* dest, const char* src, size_t max_len) {
    if (!dest || !src || max_len == 0) return;
    size_t i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void str_append(char* dest, const char* src, size_t max_len) {
    if (!dest || !src || max_len == 0) return;
    size_t dest_len = 0;
    while (dest[dest_len] != '\0' && dest_len < max_len) {
        dest_len++;
    }
    if (dest_len >= max_len - 1) return;

    size_t i = 0;
    while (src[i] != '\0' && dest_len + i < max_len - 1) {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';
}

size_t str_len(const char* str) {
    if (!str) return 0;
    size_t i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

// 检查字符串是否以指定的后缀结尾
int str_ends_with(const char* str, const char* suffix) {
    if (!str || !suffix) {
        return 0; // false
    }

    size_t str_len = 0;
    while(str[str_len]) str_len++;

    size_t suffix_len = 0;
    while(suffix[suffix_len]) suffix_len++;

    if (suffix_len > str_len) {
        return 0; // false
    }

    // 从字符串末尾开始比较
    for (size_t i = 0; i < suffix_len; i++) {
        if (str[str_len - suffix_len + i] != suffix[i]) {
            return 0; // false
        }
    }

    return 1; // true
}

// 简单的字符串比较函数
int str_compare(const char* s1, const char* s2) {
    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// 安全的字符串比较函数，最多比较 n 个字节
int str_compare_n(const char* s1, const char* s2, size_t n) {
    if (n == 0) return 0;
    while (n-- > 0 && *s1 == *s2) {
        if (n == 0 || *s1 == '\0') break;
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void hex_to_str(uintptr_t value, char* dest, size_t dest_size) {
    if (dest_size < 3) return; // "0x" + null terminator

    const char* hex_chars = "0123456789abcdef";
    char temp_buf[sizeof(uintptr_t) * 2 + 1]; // 每个字节2个hex字符 + null
    int i = 0;

    if (value == 0) {
        temp_buf[i++] = '0';
    } else {
        uintptr_t val = value;
        while (val > 0) {
            temp_buf[i++] = hex_chars[val % 16];
            val /= 16;
        }
    }

    // "0x" 前缀
    size_t pos = 0;
    if (pos + 2 < dest_size) {
        dest[pos++] = '0';
        dest[pos++] = 'x';
    }

    // 翻转并拷贝
    while (i > 0 && pos < dest_size - 1) {
        dest[pos++] = temp_buf[--i];
    }
    dest[pos] = '\0';
}


void format_log_string_with_hex(char* buf, size_t buf_size, const char* prefix, const char* str, const char* mid, uintptr_t hex_val) {
    str_copy(buf, prefix, buf_size);
    str_append(buf, str, buf_size);
    str_append(buf, mid, buf_size);
    hex_to_str(hex_val, buf + str_len(buf), buf_size - str_len(buf));
} 