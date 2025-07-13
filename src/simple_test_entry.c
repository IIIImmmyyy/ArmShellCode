#include "syscall_wrapper.h"
#include "elf_parser.h"
#include "android_utils.h"

// 前向声明
int simple_test_main(void);

// 简单测试入口点
void __attribute__((naked, noreturn, section(".text._start"))) _start(void) {
#ifdef ARCH_ARM64
    __asm__ volatile (
        "bl simple_test_main\n"     // 调用测试函数
        "ret\n"                     // 返回结果
        :
        :
        : "memory", "x30"
    );
#elif defined(ARCH_ARM32)
    __asm__ volatile (
        "bl simple_test_main\n"     // 调用测试函数
        "bx lr\n"                   // 返回结果
        :
        :
        : "memory", "lr"
    );
#endif
}

// 简单测试：只解析maps文件，返回映射数量
int simple_test_main(void) {
    MemoryMap maps[256];
    int map_count = parse_maps_file(maps, 256);
    
    if (map_count <= 0) {
        return -1;  // 解析失败
    }
    
    // 返回找到的映射数量（限制在合理范围内）
    return (map_count > 1000) ? 1000 : map_count;
} 