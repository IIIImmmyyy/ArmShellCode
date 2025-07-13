# Android Shellcode 项目

这是一个专门为Android平台设计的shellcode开发框架，可以生成完全独立的二进制文件，用于安全研究和渗透测试。

## 🎯 主要特性

- **纯C实现** - 使用内联汇编，无需单独的.s文件
- **无依赖库** - 完全通过系统调用实现，不依赖任何系统库
- **跨架构支持** - 支持ARM32和ARM64架构
- **ELF符号解析** - 可以读取maps文件并解析SO文件的符号表
- **位置无关代码** - 生成的shellcode可以在任意地址执行
- **易于集成** - 自动生成C头文件，便于嵌入到其他程序中

## 📁 项目结构

```
ShellCode/
├── CMakeLists.txt              # CMake构建配置
├── shellcode.ld.in             # 链接脚本模板
├── bin_to_header.py            # 二进制转C头文件工具
├── build_shellcode.sh          # 自动构建脚本
├── README.md                   # 项目说明
├── include/                    # 头文件目录
│   ├── syscall_wrapper.h       # 系统调用包装器
│   ├── android_utils.h         # Android工具函数
│   └── elf_parser.h            # ELF文件解析器
└── src/                        # 源文件目录
    ├── entry.c                 # 程序入口点
    ├── syscall_wrapper.c       # 系统调用实现
    ├── android_utils.c         # 工具函数实现
    ├── elf_parser.c            # ELF解析实现
    ├── shellcode_manager.c     # Shellcode管理
    └── main.c                  # 主程序演示
```




## 💡 使用示例

### 在其他SO文件中加载和执行shellcode

```c
#include <sys/mman.h>
#include <string.h>
#include "shellcode_data.h"  // 自动生成的头文件

int execute_shellcode() {
    // 分配可执行内存
    void *exec_mem = mmap(NULL, shellcode_size,
                         PROT_READ | PROT_WRITE | PROT_EXEC,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (exec_mem == MAP_FAILED) {
        return -1;
    }
    
    // 复制shellcode到可执行内存
    memcpy(exec_mem, shellcode_data, shellcode_size);
    
    // 执行shellcode (从偏移0开始，即_start函数)
    void (*shellcode_func)(void) = (void (*)(void))exec_mem;
    shellcode_func();
    
    // 清理内存
    munmap(exec_mem, shellcode_size);
    
    return 0;
}
```

### 直接加载二进制文件

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int load_and_execute_shellcode(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return -1;
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // 分配可执行内存
    void *exec_mem = mmap(NULL, size,
                         PROT_READ | PROT_WRITE | PROT_EXEC,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (exec_mem == MAP_FAILED) {
        fclose(file);
        return -1;
    }
    
    // 读取并执行
    fread(exec_mem, 1, size, file);
    fclose(file);
    
    void (*shellcode_func)(void) = (void (*)(void))exec_mem;
    shellcode_func();
    
    munmap(exec_mem, size);
    return 0;
}
```

## 🔧 核心功能

### ELF符号解析

```c
// 查找指定库中的符号
unsigned long malloc_addr = find_symbol_address("libc.so", "malloc");

// 解析内存映射
MemoryMap maps[256];
int map_count = parse_maps_file(maps, 256);

// 枚举库中的所有符号
SymbolInfo symbols[1024];
int symbol_count = parse_elf_symbols("/system/lib64/libc.so", base_addr, symbols, 1024);
```

### 系统调用

```c
// 直接使用系统调用
ssize_t bytes = sys_write(1, "Hello Android\n", 14);
int fd = sys_open("/proc/version", O_RDONLY, 0);
void *mem = sys_mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
```

### 该项目使用CLion构建 请查看CLion配置说明或者自行构建

### 编译器标志

项目使用以下关键编译标志：

- `-fPIC` - 生成位置无关代码
- `-nostdlib` - 不链接标准库
- `-nostartfiles` - 不使用启动文件
- `-fno-stack-protector` - 关闭栈保护
- `-Os` - 优化代码大小

## 📋 系统要求

- **Android NDK** r21或更高版本
- **CMake** 3.18.1或更高版本
- **Python 3** (可选，用于生成C头文件)
- **Linux/macOS/Windows** 开发环境

## ⚠️ 注意事项

1. **仅用于合法用途** - 此工具仅供安全研究和教育目的使用
2. **测试环境** - 请在安全的测试环境中使用
3. **权限要求** - 某些功能可能需要root权限
4. **兼容性** - 生成的shellcode与特定的Android版本和架构相关

## 🔗 相关链接

- [Android NDK](https://developer.android.com/ndk)
- [CMake Documentation](https://cmake.org/documentation/)
- [ARM Assembly Reference](https://developer.arm.com/documentation/)

## 📝 许可证

本项目仅供学习和研究使用，请遵守相关法律法规。 