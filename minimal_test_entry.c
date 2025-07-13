#include "syscall_wrapper.h"

// 最简单的测试函数 - 只使用基本系统调用
int shellcode_main(void) {
    // 使用基本的write系统调用输出到stdout
    char msg1[] = "SIMPLE: Shellcode started\n";
    char msg2[] = "SIMPLE: About to return\n";
    
    sys_write(1, msg1, sizeof(msg1) - 1);
    sys_write(1, msg2, sizeof(msg2) - 1);
    
    return 123; // 返回一个特定值
}

// 入口点
void __attribute__((naked, noreturn, section(".text._start"))) _start(void) {
#ifdef ARCH_ARM64
    __asm__ volatile (
        "mov x0, sp\n"              
        "and x0, x0, #0xFFFFFFFFFFFFFFF0\n"  
        "mov sp, x0\n"              
        "bl shellcode_main\n"       
        "ret\n"                     
        :
        :
        : "memory", "x0"
    );
#elif defined(ARCH_ARM32)
    __asm__ volatile (
        "bic sp, sp, #7\n"          
        "bl shellcode_main\n"       
        "bx lr\n"                   
        :
        :
        : "memory", "lr"
    );
#endif
} 