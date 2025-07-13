#include "file_ops.h"
#include "logger.h"
#include "syscall_wrapper.h"

#define TAG "FILE_OPS"

// 统一的函数，负责打开文件、获取大小并进行内存映射
int file_open_and_map(const char *path, file_info_t *info) {
    if (!path || !info) {
        return FILE_ERR_ACCESS;
    }
    
//    // 为了调试，手动构建日志字符串来打印路径
//    char log_buffer[FILE_PATH_MAX + 20];
//    str_copy(log_buffer, "Opening path: ", sizeof(log_buffer));
//    str_append(log_buffer, path, sizeof(log_buffer));
//    LOG_I(TAG, log_buffer);

    // 初始化结构体
    info->fd = -1;
    info->file_size = 0;
    info->mapped_memory = NULL;

    // 1. 打开文件
    // AT_FDCWD (-100) 表示路径是相对于当前工作目录的
    info->fd = sys_openat(-100, path, O_RDONLY, 0);
    if (info->fd < 0) {
//        LOG_E_ERR(TAG, "Failed to open file", -info->fd);
        return FILE_ERR_ACCESS;
    }
//    LOG_I_HEX(TAG, "File opened with fd: ", info->fd);


    // 2. 获取文件状态和大小
    if (sys_fstat(info->fd, &info->st) < 0) {
//        LOG_E(TAG, "fstat failed");
        file_close(info); // 使用统一的关闭函数
        return FILE_ERR_ACCESS;
    }
    info->file_size = info->st.st_size; // 从 stat 结构体获取大小
//    LOG_I_HEX(TAG, "File size: ", info->file_size);

    if (info->file_size == 0) {
//        LOG_W(TAG, "File size is zero. Continuing without mapping.");
        // 文件大小为0是有效情况，直接成功返回，但不进行映射
        return FILE_SUCCESS;
    }

    // 3. 内存映射
    info->mapped_memory = sys_mmap(NULL, info->file_size, PROT_READ, MAP_PRIVATE, info->fd, 0);
    if (info->mapped_memory == (void *)-1 || info->mapped_memory == NULL) {
//        LOG_E(TAG, "mmap failed");
        file_close(info); // 映射失败，关闭文件
        return FILE_ERR_MMAP;
    }
//    LOG_I_HEX(TAG, "File mapped at: ", (uintptr_t)info->mapped_memory);

    return FILE_SUCCESS;
}

// 统一的资源释放函数
void file_close(file_info_t *info) {
    if (!info) return;

    // 1. 解除内存映射
    if (info->mapped_memory) {
        sys_munmap(info->mapped_memory, info->file_size);
        info->mapped_memory = NULL;
    }

    // 2. 关闭文件描述符
    if (info->fd >= 0) {
        sys_close(info->fd);
        LOG_D(TAG, "Closed file descriptor");
        info->fd = -1;
    }
}

// 新函数实现：读取一个文件的全部内容到缓冲区
ssize_t file_read_all(const char *path, char *buf, size_t buf_size) {
    if (!path || !buf || buf_size == 0) {
        return FILE_ERR_ACCESS;
    }

    int fd = sys_openat(-100, path, O_RDONLY, 0); // AT_FDCWD = -100
    if (fd < 0) {
        LOG_E_ERR(TAG, "file_read_all: open failed", -fd);
        return FILE_ERR_ACCESS;
    }

    size_t total_read = 0;
    ssize_t bytes_read;

    // 循环读取，直到文件末尾(EOF)或缓冲区满
    while (total_read < buf_size) {
        bytes_read = sys_read(fd, buf + total_read, buf_size - total_read);
        if (bytes_read == 0) { // EOF
            break;
        }
        if (bytes_read < 0) { // Error
            LOG_E_ERR(TAG, "file_read_all: read failed", -bytes_read);
            sys_close(fd);
            return FILE_ERR_READ;
        }
        total_read += bytes_read;
    }

    sys_close(fd);
    
    // 确保缓冲区内容是有效的C字符串
    if (total_read < buf_size) {
        buf[total_read] = '\0';
    } else {
        buf[buf_size - 1] = '\0'; // 如果缓冲区满了，强制截断
    }

    LOG_I_HEX(TAG, "file_read_all: total bytes read: ", total_read);
    return total_read;
}


// 从指定偏移读取文件内容
ssize_t file_read(file_info_t *info, void *buf, size_t count, off_t offset) {
    if (!info || !buf || info->fd < 0) {
        return FILE_ERR_ACCESS;
    }
    
    // 使用 lseek 设置文件读取位置
    if (sys_lseek(info->fd, offset, SEEK_SET) < 0) {
        LOG_E(TAG, "lseek failed");
        return FILE_ERR_READ;
    }
    
    // 读取文件内容
    ssize_t bytes_read = sys_read(info->fd, buf, count);
    if (bytes_read < 0) {
        LOG_E_ERR(TAG, "read failed", -bytes_read);
        return FILE_ERR_READ;
    }
    
    return bytes_read;
}

// 打开文件用于写入
int file_open_for_write(const char *path, file_info_t *info) {
    if (!path || !info) {
        return FILE_ERR_ACCESS;
    }

    // 初始化结构体
    info->fd = -1;
    info->file_size = 0;
    info->mapped_memory = NULL;

    // 打开文件，使用写入模式
    info->fd = sys_openat(-100, path, FILE_WRITE_MODE, FILE_PERMISSIONS);
    if (info->fd < 0) {
        LOG_E_ERR(TAG, "Failed to open file for writing", -info->fd);
        return FILE_ERR_ACCESS;
    }
    LOG_I_HEX(TAG, "File opened for writing with fd: ", info->fd);

    return FILE_SUCCESS;
}

// 写入文件内容
ssize_t file_write(file_info_t *info, const void *buf, size_t count) {
    if (!info || !buf || info->fd < 0) {
        return FILE_ERR_ACCESS;
    }

    ssize_t bytes_written = sys_write(info->fd, buf, count);
    if (bytes_written < 0) {
        LOG_E_ERR(TAG, "Write failed", -bytes_written);
        return FILE_ERR_READ; // 复用现有错误码
    }

//    LOG_I_HEX(TAG, "Bytes written: ", bytes_written);
    return bytes_written;
}