#include "android_utils.h"

// 简单的测试函数
int shellcode_main(void) {
    android_log_info("SHELLCODE", "=== SIMPLE TEST START ===");
    android_log_info("SHELLCODE", "Line 1: Hello from shellcode!");
    android_log_info("SHELLCODE", "Line 2: Testing logging...");
    android_log_info("SHELLCODE", "Line 3: About to return...");
    android_log_info("SHELLCODE", "=== SIMPLE TEST END ===");
    return 42; // 返回一个特定值用于验证
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