#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <syscall_wrapper.h> // 包含 syscall 包装器, 里面已经有 <sys/stat.h>
#include <stddef.h>          // 为了 size_t
#include <stdint.h>          // 为了 uintptr_t

// 恢复被误删的错误码定义
#define FILE_SUCCESS        0
#define FILE_ERR_NOT_FOUND  -1
#define FILE_ERR_ACCESS     -2
#define FILE_ERR_SIZE       -3
#define FILE_ERR_MMAP       -4
#define FILE_ERR_READ       -5

#define FILE_PATH_MAX 256

// 修正文件信息结构体
typedef struct {
    int fd;
    size_t file_size;      // 保留文件大小
    void *mapped_memory;   // 保留映射的内存地址
    struct stat st;        // 加入标准的stat结构体，用于fstat
} file_info_t;

// 函数声明
int file_open_and_map(const char *path, file_info_t *info);
void file_close(file_info_t *info);
ssize_t file_read(file_info_t *info, void *buf, size_t count, off_t offset);

// 新增函数：用于读取 /proc/ 等大小不确定的文件
// 返回读取的字节数，或在失败时返回错误码
ssize_t file_read_all(const char *path, char *buf, size_t buf_size);

// 新增文件写入相关定义
#define FILE_WRITE_MODE  (O_WRONLY | O_CREAT | O_APPEND)
#define FILE_PERMISSIONS 0644

// 新增函数声明
int file_open_for_write(const char *path, file_info_t *info);
ssize_t file_write(file_info_t *info, const void *buf, size_t count);

#endif //FILE_OPS_H