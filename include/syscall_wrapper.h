#ifndef SYSCALL_WRAPPER_H
#define SYSCALL_WRAPPER_H
#include <sys/stat.h>
#include "asm-generic/unistd.h"
// Android系统调用号定义
#ifdef ARCH_ARM64
    // ARM64系统调用号 (正确的现代Linux值)
    #define SYS_read            63
    #define SYS_write           64
    #define SYS_openat          56  // 现代Linux使用openat而不是open
    #define SYS_close           57
    #define SYS_lseek           62  // lseek系统调用
    #define SYS_fstat           80  // fstat系统调用
    #define SYS_exit            93
    #define SYS_execve          221
    #define SYS_mmap            222
    #define SYS_munmap          215
    #define SYS_mprotect        226
    #define SYS_socket          198
    #define SYS_connect         203
    #define SYS_bind            200
    #define SYS_listen          201
    #define SYS_accept          202
    #define SYS_clock_gettime   283 // 修正为Android ARM64正确的clock_gettime系统调用号
#elif defined(ARCH_ARM32)
    // ARM32系统调用号
    #define SYS_read            3
    #define SYS_write           4
    #define SYS_open            5
    #define SYS_close           6
    #define SYS_lseek           19  // lseek系统调用
    #define SYS_fstat           108 // fstat系统调用
    #define SYS_exit            1
    #define SYS_execve          11
    #define SYS_mmap            90
    #define SYS_munmap          91
    #define SYS_mprotect        125
    #define SYS_socket          281
    #define SYS_connect         283
    #define SYS_bind            282
    #define SYS_listen          284
    #define SYS_accept          285
    #define SYS_clock_gettime   307 // 添加ARM32的clock_gettime系统调用号
#endif

// 文件操作标志
#define O_RDONLY            0
#define O_WRONLY            1
#define O_RDWR              2
#define O_CREAT             0100
#define O_EXCL              0200
#define O_TRUNC             01000
#define O_APPEND            02000

// lseek whence 常量
#define SEEK_SET            0   // 文件开头
#define SEEK_CUR            1   // 当前位置
#define SEEK_END            2   // 文件结尾

// 内存保护标志
#define PROT_READ           0x1
#define PROT_WRITE          0x2
#define PROT_EXEC           0x4
#define PROT_NONE           0x0

// 内存映射标志
#define MAP_SHARED          0x01
#define MAP_PRIVATE         0x02
#define MAP_ANONYMOUS       0x20
#define MAP_FIXED           0x10

// 网络相关常量
#define AF_INET             2
#define SOCK_STREAM         1
#define SOCK_DGRAM          2

// 基本数据类型
typedef long ssize_t;
typedef unsigned long size_t;
typedef int pid_t;
typedef unsigned int mode_t;
typedef long off_t;


// 内联汇编系统调用实现
#ifdef ARCH_ARM64
    // ARM64内联汇编系统调用
    static inline long syscall0(long number) {
        register long x8 __asm__("x8") = number;
        register long x0 __asm__("x0");
        __asm__ volatile("svc #0" : "=r"(x0) : "r"(x8) : "memory");
        return x0;
    }
    
    static inline long syscall1(long number, long arg1) {
        register long x8 __asm__("x8") = number;
        register long x0 __asm__("x0") = arg1;
        __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8) : "memory");
        return x0;
    }
    
    static inline long syscall2(long number, long arg1, long arg2) {
        register long x8 __asm__("x8") = number;
        register long x0 __asm__("x0") = arg1;
        register long x1 __asm__("x1") = arg2;
        __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1) : "memory");
        return x0;
    }
    
    static inline long syscall3(long number, long arg1, long arg2, long arg3) {
        register long x8 __asm__("x8") = number;
        register long x0 __asm__("x0") = arg1;
        register long x1 __asm__("x1") = arg2;
        register long x2 __asm__("x2") = arg3;
        __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2) : "memory");
        return x0;
    }
    
    static inline long syscall4(long number, long arg1, long arg2, long arg3, long arg4) {
        register long x8 __asm__("x8") = number;
        register long x0 __asm__("x0") = arg1;
        register long x1 __asm__("x1") = arg2;
        register long x2 __asm__("x2") = arg3;
        register long x3 __asm__("x3") = arg4;
        __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2), "r"(x3) : "memory");
        return x0;
    }
    
    static inline long syscall5(long number, long arg1, long arg2, long arg3, long arg4, long arg5) {
        register long x8 __asm__("x8") = number;
        register long x0 __asm__("x0") = arg1;
        register long x1 __asm__("x1") = arg2;
        register long x2 __asm__("x2") = arg3;
        register long x3 __asm__("x3") = arg4;
        register long x4 __asm__("x4") = arg5;
        __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4) : "memory");
        return x0;
    }
    
    static inline long syscall6(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
        register long x8 __asm__("x8") = number;
        register long x0 __asm__("x0") = arg1;
        register long x1 __asm__("x1") = arg2;
        register long x2 __asm__("x2") = arg3;
        register long x3 __asm__("x3") = arg4;
        register long x4 __asm__("x4") = arg5;
        register long x5 __asm__("x5") = arg6;
        __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5) : "memory");
        return x0;
    }

#elif defined(ARCH_ARM32)
    // ARM32内联汇编系统调用
    static inline long syscall0(long number) {
        register long r7 __asm__("r7") = number;
        register long r0 __asm__("r0");
        __asm__ volatile("swi #0" : "=r"(r0) : "r"(r7) : "memory");
        return r0;
    }
    
    static inline long syscall1(long number, long arg1) {
        register long r7 __asm__("r7") = number;
        register long r0 __asm__("r0") = arg1;
        __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7) : "memory");
        return r0;
    }
    
    static inline long syscall2(long number, long arg1, long arg2) {
        register long r7 __asm__("r7") = number;
        register long r0 __asm__("r0") = arg1;
        register long r1 __asm__("r1") = arg2;
        __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1) : "memory");
        return r0;
    }
    
    static inline long syscall3(long number, long arg1, long arg2, long arg3) {
        register long r7 __asm__("r7") = number;
        register long r0 __asm__("r0") = arg1;
        register long r1 __asm__("r1") = arg2;
        register long r2 __asm__("r2") = arg3;
        __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2) : "memory");
        return r0;
    }
    
    static inline long syscall4(long number, long arg1, long arg2, long arg3, long arg4) {
        register long r7 __asm__("r7") = number;
        register long r0 __asm__("r0") = arg1;
        register long r1 __asm__("r1") = arg2;
        register long r2 __asm__("r2") = arg3;
        register long r3 __asm__("r3") = arg4;
        __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2), "r"(r3) : "memory");
        return r0;
    }
    
    static inline long syscall5(long number, long arg1, long arg2, long arg3, long arg4, long arg5) {
        register long r7 __asm__("r7") = number;
        register long r0 __asm__("r0") = arg1;
        register long r1 __asm__("r1") = arg2;
        register long r2 __asm__("r2") = arg3;
        register long r3 __asm__("r3") = arg4;
        register long r4 __asm__("r4") = arg5;
        __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2), "r"(r3), "r"(r4) : "memory");
        return r0;
    }
    
    static inline long syscall6(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
        register long r7 __asm__("r7") = number;
        register long r0 __asm__("r0") = arg1;
        register long r1 __asm__("r1") = arg2;
        register long r2 __asm__("r2") = arg3;
        register long r3 __asm__("r3") = arg4;
        register long r4 __asm__("r4") = arg5;
        register long r5 __asm__("r5") = arg6;
        __asm__ volatile("swi #0" : "+r"(r0) : "r"(r7), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) : "memory");
        return r0;
    }
#endif

// 高级系统调用包装函数
ssize_t sys_read(int fd, void *buf, size_t count);
ssize_t sys_write(int fd, const void *buf, size_t count);
int sys_openat(int dirfd, const char *pathname, int flags, mode_t mode);
int sys_open(const char *pathname, int flags, mode_t mode);  // 便利函数，调用openat
int sys_close(int fd);
off_t sys_lseek(int fd, off_t offset, int whence);
int sys_fstat(int fd, struct stat *statbuf);
void sys_exit(int status);
int sys_execve(const char *filename, char *const argv[], char *const envp[]);
void *sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int sys_munmap(void *addr, size_t length);
int sys_mprotect(void *addr, size_t len, int prot);
int sys_socket(int domain, int type, int protocol);
int sys_connect(int sockfd, const void *addr, unsigned int addrlen);
int sys_bind(int sockfd, const void *addr, unsigned int addrlen);
int sys_listen(int sockfd, int backlog);
int sys_accept(int sockfd, void *addr, unsigned int *addrlen);


#define CLOCK_REALTIME 0

// 时间系统调用封装
static inline int sys_clock_gettime(int clock_id, struct timespec *ts) {
#ifdef ARCH_ARM64
    return syscall2(__NR_clock_gettime, clock_id, (long)ts);
#elif defined(ARCH_ARM32)
    return syscall2(SYS_clock_gettime, clock_id, (long)ts);
#else
    return -1;
#endif
}

#endif // SYSCALL_WRAPPER_H