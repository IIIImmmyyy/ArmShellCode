#include "logger.h"
#include "dobby_symbol_resolver.h" // <--- 引入新的 Dobby 解析器

// 标签定义
#define TAG_MAIN "SHELLCODE"
// 目标配置
static const char* TARGET_LIBRARY = "libxxx.so";
static const char* TARGET_SYMBOL = "xxxx";




// 主要shellcode逻辑
uintptr_t shellcode_main_refactored() {



    uintptr_t symbol_addr;
    int result = dobby_resolve_symbol(TARGET_LIBRARY, TARGET_SYMBOL, &symbol_addr);

    if (result != RESOLVER_SUCCESS) { // <--- 使用新的成功码
        return (uintptr_t)result;
    }

    typedef void (*ioctl)();

    ioctl fun=(ioctl)symbol_addr;
    fun();
    return symbol_addr;
}


