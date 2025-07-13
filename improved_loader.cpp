#include <android/log.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "LOADER", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "LOADER", __VA_ARGS__)

// 用于异常处理的全局变量
static jmp_buf crash_jmp;
static volatile bool crashed = false;

// 信号处理函数
void crash_handler(int sig) {
    LOGE("Shellcode crashed with signal %d (%s)", sig, strerror(errno));
    crashed = true;
    longjmp(crash_jmp, 1);
}

void hexDump(const void* data, size_t size) {
    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t i = 0; i < size; i += 16) {
        LOGI("%04lx: ", i);
        // 打印16个字节的十六进制
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            LOGI("%02x ", bytes[i + j]);
        }
        LOGI("\n");
    }
}

void LoadShellCode() {
    std::string shellcodePath = "/data/data/com.gxooxzk.nyhakg/shellcode.bin";
    
    LOGI("=== Starting shellcode loading process ===");
    LOGI("Shellcode path: %s", shellcodePath.c_str());
    
    // 检查文件是否存在
    if (access(shellcodePath.c_str(), F_OK) != 0) {
        LOGE("Shellcode file does not exist: %s", shellcodePath.c_str());
        return;
    }
    
    if (access(shellcodePath.c_str(), R_OK) != 0) {
        LOGE("No read permission for shellcode file: %s", shellcodePath.c_str());
        return;
    }
    
    // 打开文件
    int fd = open(shellcodePath.c_str(), O_RDONLY);
    if (fd < 0) {
        LOGE("Failed to open shellcode file: %s (errno: %d)", shellcodePath.c_str(), errno);
        return;
    }
    
    // 获取文件大小
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    
    LOGI("Shellcode file size: %ld bytes", file_size);
    
    if (file_size <= 0 || file_size > 1024 * 1024) { // 限制最大1MB
        LOGE("Invalid file size: %ld", file_size);
        close(fd);
        return;
    }
    
    // 分配可执行内存
    void* shellcode_memory = mmap(NULL, file_size,
                                  PROT_READ | PROT_WRITE | PROT_EXEC,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (shellcode_memory == MAP_FAILED) {
        LOGE("Failed to allocate executable memory: %s (errno: %d)", strerror(errno), errno);
        close(fd);
        return;
    }
    
    LOGI("Allocated executable memory at: %p", shellcode_memory);
    
    // 读取shellcode到内存
    ssize_t bytes_read = read(fd, shellcode_memory, file_size);
    close(fd);
    
    if (bytes_read != file_size) {
        LOGE("Failed to read complete file. Read %ld/%ld bytes", bytes_read, file_size);
        munmap(shellcode_memory, file_size);
        return;
    }
    
    LOGI("Shellcode loaded successfully");
    LOGI("First 64 bytes of shellcode:");
    hexDump(shellcode_memory, 64);
    
    // 检查shellcode开头是否合理（ARM64魔数检查）
    const unsigned char* bytes = (const unsigned char*)shellcode_memory;
    LOGI("First 4 bytes: %02x %02x %02x %02x", bytes[0], bytes[1], bytes[2], bytes[3]);
    
    // 确保指令缓存同步
    __builtin___clear_cache((char*)shellcode_memory, (char*)shellcode_memory + file_size);
    LOGI("Instruction cache cleared");
    
    // 设置信号处理器捕获崩溃
    struct sigaction old_sigsegv, old_sigbus, old_sigill;
    struct sigaction sa;
    sa.sa_handler = crash_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGSEGV, &sa, &old_sigsegv);
    sigaction(SIGBUS, &sa, &old_sigbus);
    sigaction(SIGILL, &sa, &old_sigill);
    
    crashed = false;
    
    // 使用setjmp/longjmp进行异常处理
    if (setjmp(crash_jmp) == 0) {
        LOGI("About to execute shellcode at address: %p", shellcode_memory);
        
        // 保存当前上下文信息
        void* current_sp;
        #ifdef __aarch64__
        __asm__ volatile ("mov %0, sp" : "=r" (current_sp));
        #elif defined(__arm__)
        __asm__ volatile ("mov %0, sp" : "=r" (current_sp));
        #endif
        
        LOGI("Current stack pointer: %p", current_sp);
        LOGI("Executing shellcode NOW...");
        
        typedef void (*shellcode_func_t)(void);
        auto shellcode_func = (shellcode_func_t)shellcode_memory;
        
        // 调用shellcode
        shellcode_func();
        
        LOGI("*** SHELLCODE RETURNED SUCCESSFULLY ***");
        
    } else {
        LOGE("*** SHELLCODE CRASHED - RECOVERED ***");
    }
    
    // 恢复信号处理器
    sigaction(SIGSEGV, &old_sigsegv, nullptr);
    sigaction(SIGBUS, &old_sigbus, nullptr);
    sigaction(SIGILL, &old_sigill, nullptr);
    
    if (!crashed) {
        LOGI("Shellcode execution completed normally");
    } else {
        LOGE("Shellcode execution failed with crash");
    }
    
    // 释放内存
    if (munmap(shellcode_memory, file_size) == -1) {
        LOGE("Failed to unmap memory: %s", strerror(errno));
    } else {
        LOGI("Memory unmapped successfully");
    }
    
    LOGI("=== Shellcode loading process completed ===");
} 