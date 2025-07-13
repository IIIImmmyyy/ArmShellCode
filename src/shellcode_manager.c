#include "syscall_wrapper.h"

// shellcode管理相关功能实现

// 将数据写入到指定地址并设置为可执行
int make_executable(void *addr, size_t size) {
    return sys_mprotect(addr, size, PROT_READ | PROT_WRITE | PROT_EXEC);
}

// 创建可执行内存区域
void *alloc_executable_memory(size_t size) {
    void *mem = sys_mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if ((long)mem < 0) {
        return (void *)0;
    }
    
    // 设置为可执行
    if (make_executable(mem, size) < 0) {
        sys_munmap(mem, size);
        return (void *)0;
    }
    
    return mem;
}

// 释放可执行内存
int free_executable_memory(void *addr, size_t size) {
    return sys_munmap(addr, size);
}

// 复制shellcode到内存并执行
int execute_shellcode(const unsigned char *shellcode, size_t size) {
    // 分配可执行内存
    void *exec_mem = alloc_executable_memory(size);
    if (!exec_mem) {
        return -1;
    }
    
    // 复制shellcode
    unsigned char *dest = (unsigned char *)exec_mem;
    const unsigned char *src = shellcode;
    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
    }
    
    // 执行shellcode (转换为函数指针并调用)
    void (*func)(void) = (void (*)(void))exec_mem;
    func();
    
    // 清理内存
    free_executable_memory(exec_mem, size);
    
    return 0;
} 