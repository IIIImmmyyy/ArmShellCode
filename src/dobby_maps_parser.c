#include "dobby_maps_parser.h"
#include "logger.h"
#include "string_utils.h"
#include "syscall_wrapper.h"

#define TAG "DOBBY_MAPS_PARSER"
#define MAPS_FILE_PATH "/proc/self/maps"

// === 内部辅助函数 ===

static int hex_char_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static uintptr_t parse_hex(const char** p_str) {
    uintptr_t val = 0;
    const char* str = *p_str;
    while (*str) {
        int c = hex_char_to_value(*str);
        if (c < 0) break;
        val = (val << 4) | c;
        str++;
    }
    *p_str = str;
    return val;
}

static const char* skip_whitespace(const char* p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

// === API 实现 ===

int dobby_get_runtime_module(const char* name, dobby_runtime_module_t* module) {
    if (!name || !module) {
        return MAPS_PARSER_ERR_PARSE_FAILED;
    }

    int fd = sys_openat(-100, MAPS_FILE_PATH, O_RDONLY, 0);
    if (fd < 0) {
//        LOG_E_ERR(TAG, "open maps file failed", -fd);
        return MAPS_PARSER_ERR_OPEN_FAILED;
    }

    char read_buf[4096];
    char line_buf[512];
    ssize_t bytes_in_buf = 0;
    int buf_pos = 0;
    int line_pos = 0;
    
    int result_code = MAPS_PARSER_ERR_NOT_FOUND;

    while (1) {
        if (buf_pos >= bytes_in_buf) {
            bytes_in_buf = sys_read(fd, read_buf, sizeof(read_buf));
            if (bytes_in_buf <= 0) break;
            buf_pos = 0;
        }

        while (buf_pos < bytes_in_buf) {
            char c = read_buf[buf_pos++];
            if (c == '\n') {
                line_buf[line_pos] = '\0';
                line_pos = 0;

                const char* p = line_buf;
                uintptr_t start_addr = parse_hex(&p);
                if (*p != '-') continue;
                p++;
                parse_hex(&p); // end_addr, not used
                p = skip_whitespace(p);

                char perms[5];
                for(int i = 0; i < 4; i++) perms[i] = *p++;
                perms[4] = '\0';
                p = skip_whitespace(p);

                uintptr_t offset = parse_hex(&p);
                p = skip_whitespace(p); // device
                parse_hex(&p);
                if (*p != ':') continue;
                p++;
                parse_hex(&p);
                p = skip_whitespace(p);
                parse_hex(&p); // inode
                p = skip_whitespace(p);

                const char* pathname = p;

                // Dobby 的判断逻辑: r-xp or r--p 且 offset == 0
                if (offset == 0 && (str_compare(perms, "r-xp") == 0 || str_compare(perms, "r--p") == 0)) {
                    if (str_ends_with(pathname, name)) {
                        module->load_address = start_addr;
                        str_copy(module->path, pathname, MAX_PATH_LEN);
                        result_code = MAPS_PARSER_SUCCESS;
                        goto end_loop;
                    }
                }

            } else if (line_pos < sizeof(line_buf) - 1) {
                line_buf[line_pos++] = c;
            }
        }
    }

end_loop:
    sys_close(fd);
    return result_code;
} 