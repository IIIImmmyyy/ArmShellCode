#ifndef ELF_COMMON_H
#define ELF_COMMON_H

#include <elf.h>

// 采用 JNI/NDK 中通用的 ElfW() 宏，用于优雅地处理32位和64位架构差异。
// 使用方法: ElfW(Ehdr), ElfW(Sym), ElfW(Phdr) 等。
#ifdef ARCH_ARM64
#define ElfW(type) Elf64_##type
#else // 默认为 ARM32 或其他32位架构
#define ElfW(type) Elf32_##type
#endif

// 注意：
// 我们不再需要为 Elf_Ehdr, Elf_Shdr 等类型手动 typedef。
// 所有代码将直接使用 ElfW(Ehdr) 这种形式，使代码更具可移植性和一致性。

#endif // ELF_COMMON_H 