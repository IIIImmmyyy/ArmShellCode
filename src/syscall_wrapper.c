#include "syscall_wrapper.h"

ssize_t sys_read(int fd, void *buf, size_t count) {
    return syscall3(SYS_read, fd, (long)buf, count);
}

ssize_t sys_write(int fd, const void *buf, size_t count) {
    return syscall3(SYS_write, fd, (long)buf, count);
}

int sys_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    return syscall4(SYS_openat, dirfd, (long)pathname, flags, mode);
}

// 便利函数，调用 openat
int sys_open(const char *pathname, int flags, mode_t mode) {
    return sys_openat(-100, pathname, flags, mode); // AT_FDCWD = -100
}

int sys_close(int fd) {
    return syscall1(SYS_close, fd);
}

off_t sys_lseek(int fd, off_t offset, int whence) {
    return syscall3(SYS_lseek, fd, offset, whence);
}

int sys_fstat(int fd, struct stat *statbuf) {
    return syscall2(SYS_fstat, fd, (long)statbuf);
}

void sys_exit(int status) {
    syscall1(SYS_exit, status);
}

int sys_execve(const char *filename, char *const argv[], char *const envp[]) {
    return syscall3(SYS_execve, (long)filename, (long)argv, (long)envp);
}

void *sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return (void*)syscall6(SYS_mmap, (long)addr, length, prot, flags, fd, offset);
}

int sys_munmap(void *addr, size_t length) {
    return syscall2(SYS_munmap, (long)addr, length);
}

int sys_mprotect(void *addr, size_t len, int prot) {
    return syscall3(SYS_mprotect, (long)addr, len, prot);
}

int sys_socket(int domain, int type, int protocol) {
    return syscall3(SYS_socket, domain, type, protocol);
}

int sys_connect(int sockfd, const void *addr, unsigned int addrlen) {
    return syscall3(SYS_connect, sockfd, (long)addr, addrlen);
}

int sys_bind(int sockfd, const void *addr, unsigned int addrlen) {
    return syscall3(SYS_bind, sockfd, (long)addr, addrlen);
}

int sys_listen(int sockfd, int backlog) {
    return syscall2(SYS_listen, sockfd, backlog);
}

int sys_accept(int sockfd, void *addr, unsigned int *addrlen) {
    return syscall3(SYS_accept, sockfd, (long)addr, (long)addrlen);
} 