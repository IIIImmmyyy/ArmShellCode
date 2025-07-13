#include "logger.h"

// 前向声明新的模块化主函数
uintptr_t shellcode_main_refactored();

/**
 * @brief 模块化shellcode的入口点 (_start)
 * 
 * 这是shellcode的真正执行起点。它负责：
 * 1. 从寄存器中获取调用者传递的日志回调函数指针。
 * 2. 将此回调函数传递给新的、重构后的主函数 `shellcode_main_refactored`。
 * 3. 安全地返回到调用者。
 */
void __attribute__((naked, noreturn, section(".text._start"))) _start(void) {
#ifdef ARCH_ARM64
    // ARM64: 回调函数指针在 x0 寄存器中
    __asm__ volatile (
        // 保存链接寄存器 (x30) 到栈，并为栈对齐预留空间
        "stp x29, x30, [sp, #-16]!\n"
        "mov x29, sp\n"

        // 调用新的主函数，x0 已经包含了回调函数指针
        "bl shellcode_main_refactored\n"

        // 恢复栈和链接寄存器
        "mov sp, x29\n"
        "ldp x29, x30, [sp], #16\n"
        
        // 返回到调用者
        "ret\n"
        :
        :
        : "memory", "x0", "x29", "x30"
    );
#elif defined(ARCH_ARM32)
    // ARM32: 回调函数指针在 r0 寄存器中
    __asm__ volatile (
        // 保存链接寄存器 (lr)
        "push {lr}\n"
        
        // 调用新的主函数，r0 已经包含了回调函数指针
        "bl shellcode_main_refactored\n"
        
        // 恢复链接寄存器并返回
        "pop {lr}\n"
        "bx lr\n"
        :
        :
        : "memory", "r0", "lr"
    );
#endif
} 