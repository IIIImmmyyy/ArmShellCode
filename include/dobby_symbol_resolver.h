#ifndef DOBBY_SYMBOL_RESOLVER_H
#define DOBBY_SYMBOL_RESOLVER_H

#include <elf_common.h>
#include <stddef.h>
#include <stdint.h>

// 错误码
#define RESOLVER_SUCCESS         0
#define RESOLVER_ERR_NOT_FOUND   -1
#define RESOLVER_ERR_FILE_ACCESS -2
#define RESOLVER_ERR_INVALID_ELF -3

// dlsym相关定义
#define RTLD_DEFAULT    ((void*)0)
#define RTLD_NEXT      ((void*)-1)

// __loader_dlsym函数指针类型
typedef void* (*loader_dlsym_t)(void* handle, const char* symbol, const void* caller_addr);

// 全局resolver上下文
typedef struct {
    loader_dlsym_t dlsym_func;      // __loader_dlsym函数指针
    int initialized;                 // 是否已初始化
} dobby_resolver_ctx_t;



/**
 * @brief 移植自 Dobby 的符号解析器。
 *
 * @param lib_name 目标库的文件名, e.g., "libGuardCore.so".
 * @param symbol_name 要查找的符号名.
 * @param symbol_address [out] 用于存储找到的符号地址的指针.
 * @return RESOLVER_SUCCESS 或错误码.
 */
int dobby_resolve_symbol(const char* lib_name, const char* symbol_name, uintptr_t* symbol_address);

/**
 * @brief 专门用于从linker64中解析dlsym符号
 * 
 * @param symbol_address [out] 用于存储找到的dlsym地址的指针
 * @return RESOLVER_SUCCESS 或错误码
 */
int resolve_linker_dlsym(uintptr_t* symbol_address);

#endif // DOBBY_SYMBOL_RESOLVER_H 