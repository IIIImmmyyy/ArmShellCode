# ARM平台下Shellcode开发技术原理与实现

## 0. 前言

在移动安全领域，ShellCode技术已经成为一个不可或缺的重要组成部分。无论是应用加固、安全防护，还是漏洞利用研究，都离不开对ShellCode的深入理解和灵活运用。本文将以一个实际的开源项目为例，详细介绍在ARM平台下ShellCode的实现原理和关键技术。

## 1. 基础概念

ShellCode本质上是一段位置无关的代码片段。根据应用场景的不同，它的用途大体上有两种：

1. **动态注入执行**：通过加固SO加载ShellCode，执行自定义逻辑（目前主流的加固都有运用，可以实现解密→执行→加密的流程，完成对ShellCode代码的保护）
2. **静态链接注入**：通过LIEF等工具，将ShellCode注入到对应二进制文件中，通过修改重定向来完成ShellCode的调用

选择哪种方案取决于具体的使用场景和安全限制。本文将重点介绍第一种方案的技术细节。

## 2. 核心技术挑战

### 2.1 位置无关代码(PIC)
在ARM64架构下，ShellCode必须保证位置无关性。这是因为：
- 注入时的加载地址是不确定的
- 需要支持在内存任意位置执行
- 不能包含任何绝对地址引用
- **特别注意**：如果是静态链接注入，需慎用全局变量，因为注入的ShellCode基本都在LOAD段，是无法有写入权限的。如果使用单例模式，会导致写入权限异常

### 2.2 系统调用接口
由于ShellCode的独立性要求，大量标准库函数无法直接使用。这要求我们：
- 必须通过系统调用实现基础功能
- 需要自行处理参数传递和返回值
- 确保系统调用的兼容性

### 2.3 符号解析
在没有链接器支持的情况下，需要：
- 实现运行时符号解析
- 处理动态链接库加载
- 计算函数实际地址

## 3. 系统调用封装原理

### 3.1 ARM64系统调用约定
在ARM64架构中，系统调用使用特定的寄存器约定：
- `x8`: 系统调用号
- `x0-x5`: 参数寄存器
- `svc #0`: 触发系统调用指令

### 3.2 关键代码实现

```c
// ARM64内联汇编系统调用实现
static inline long syscall3(long number, long arg1, long arg2, long arg3) {
    register long x8 __asm__("x8") = number;  // 系统调用号
    register long x0 __asm__("x0") = arg1;    // 第一个参数
    register long x1 __asm__("x1") = arg2;    // 第二个参数  
    register long x2 __asm__("x2") = arg3;    // 第三个参数
    __asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1), "r"(x2) : "memory");
    return x0;  // 返回值
}

// 基于系统调用的文件操作
ssize_t sys_write(int fd, const void *buf, size_t count) {
    return syscall3(SYS_write, fd, (long)buf, count);
}
```

通过这种方式，我们实现了一套完整的系统调用封装框架，支持0-6个参数的系统调用。

## 4. 入口点设计原理

### 4.1 naked函数属性
```c
void __attribute__((naked, noreturn, section(".text._start"))) _start(void) {
```
关键属性说明：
- `naked`: 编译器不生成函数序言和尾声代码
- `noreturn`: 告知编译器函数不会返回
- `section(".text._start")`: 确保入口点位于代码段开头

### 4.2 关键入口点代码
```c
#ifdef ARCH_ARM64
__asm__ volatile (
    // 保存链接寄存器和帧指针
    "stp x29, x30, [sp, #-16]!\n"
    "mov x29, sp\n"
    
    // 调用主函数 (x0寄存器已包含回调函数指针)
    "bl shellcode_main_refactored\n"
    
    // 恢复寄存器并返回
    "mov sp, x29\n"
    "ldp x29, x30, [sp], #16\n"
    "ret\n"
    :
    :
    : "memory", "x0", "x29", "x30"
);
#endif
```

这段代码实现了标准的ARM64函数调用约定，确保ShellCode能够正确地接收参数并返回结果。

## 5. 动态符号解析技术

### 5.1 内存映射解析
通过读取`/proc/self/maps`文件获取已加载库的基址。这里参考了Dobby框架的实现，解析运行时模块信息：

```c
typedef struct {
    char path[MAX_PATH_LEN];
    uintptr_t load_address;
} dobby_runtime_module_t;

int dobby_get_runtime_module(const char* name, dobby_runtime_module_t* module) {
    // 解析 /proc/self/maps 找到目标库
    // 返回库的加载基址
}
```

### 5.2 ELF符号表解析
解析目标SO文件的符号表以获取函数地址：

```c
int dobby_resolve_symbol(const char* lib_name, const char* symbol_name, uintptr_t* symbol_address) {
    dobby_runtime_module_t module;
    
    // 1. 获取库的运行时信息
    if (dobby_get_runtime_module(lib_name, &module) != MAPS_PARSER_SUCCESS) {
        return RESOLVER_ERR_NOT_FOUND;
    }
    
    // 2. 解析ELF符号表
    // 3. 计算符号的实际地址 = 基址 + 偏移
    
    return RESOLVER_SUCCESS;
}
```

通过这种方式，我们就在ShellCode中实现了动态符号解析的功能，可以在运行时获取目标函数的地址，无需依赖系统链接器。

## 6. 链接脚本设计

### 6.1 紧凑的内存布局
```ld
SECTIONS
{
    . = 0x0;
    
    .text : {
        *(.text._start)     /* 确保_start函数在最前面 */
        *(.text)            /* 其他代码 */
        *(.text.*)
    }
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* 丢弃不需要的段 */
    /DISCARD/ : {
        *(.note.*)
        *(.comment)
        *(.eh_frame)
        /* ... 更多不需要的段 */
    }
}
```

### 6.2 关键编译选项
```cmake
set(SHELLCODE_FLAGS
    "-fPIC"                     # 位置无关代码
    "-fno-stack-protector"      # 关闭栈保护
    "-nostdlib"                 # 不链接标准库
    "-nostartfiles"             # 不使用启动文件
    "-static"                   # 静态链接
    "-Os"                       # 优化代码大小
)
```

这些编译选项确保生成的ShellCode具有最小的体积和最大的兼容性。

## 7. 实际应用示例

### 7.1 目标函数调用
```c
uintptr_t shellcode_main_refactored() {
    // 目标配置
    static const char* TARGET_LIBRARY = "libGuardCore.so";
    static const char* TARGET_SYMBOL = "_Z5ioctlv";
    
    uintptr_t symbol_addr;
    int result = dobby_resolve_symbol(TARGET_LIBRARY, TARGET_SYMBOL, &symbol_addr);
    
    if (result != RESOLVER_SUCCESS) {
        return (uintptr_t)result;
    }
    
    // 调用解析到的函数
    typedef void (*ioctl_func)();
    ioctl_func target_func = (ioctl_func)symbol_addr;
    target_func();
    
    return symbol_addr;
}
```

### 7.2 加载器集成
```cpp
void LoadShellCode() {
    // 1. 分配可执行内存
    void* shellcode_memory = mmap(NULL, file_size,
                                  PROT_READ | PROT_WRITE | PROT_EXEC,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    // 2. 加载shellcode
    memcpy(shellcode_memory, shellcode_data, file_size);
    
    // 3. 清除指令缓存
    __builtin___clear_cache((char*)shellcode_memory, 
                           (char*)shellcode_memory + file_size);
    
    // 4. 执行shellcode
    typedef void (*shellcode_func_t)(void);
    auto shellcode_func = (shellcode_func_t)shellcode_memory;
    shellcode_func();
}
```

## 8. 技术特点总结

### 8.1 核心优势
1. **完全独立**：不依赖任何系统库，通过系统调用实现所有功能
2. **位置无关**：可以在内存任意位置执行
3. **动态解析**：支持运行时符号解析，灵活性强
4. **体积优化**：通过精心设计的链接脚本，生成最小化的二进制文件

### 8.2 适用场景
- Android应用加固中的代码保护
- 动态代码注入和执行
- 安全研究和漏洞分析
- 二进制分析工具开发

### 8.3 技术局限
- 需要对目标系统有深入了解
- 调试相对困难
- 对系统版本敏感
- 需要处理各种边界情况

## 9. 项目开源与交流

本项目已在GitHub开源，欢迎大家参与讨论和改进。

---

> 本文作者：Imy
> 
> 项目开源地址：[https://github.com/IIIImmmyyy/ArmShellCode]
> 
> 声明：本文仅供安全研究和学习交流使用 