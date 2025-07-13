#ifndef ELF_PARSER_H
#define ELF_PARSER_H

#include "syscall_wrapper.h"
typedef __signed__ int __s32;
typedef unsigned int __u32;
// 基本类型定义，参考Android NDK
typedef unsigned char Elf_Byte;
typedef unsigned short Elf32_Half;
typedef unsigned short Elf64_Half;
typedef __s32 Elf32_Sword;
typedef unsigned int Elf32_Word;
typedef unsigned int Elf64_Word;
typedef unsigned long long Elf64_Xword;
typedef unsigned int Elf32_Addr;
typedef unsigned long long Elf64_Addr;
typedef unsigned int Elf32_Off;
typedef unsigned long long Elf64_Off;
typedef long long  Elf64_Sxword;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
// ELF标识符字段索引
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9
#define EI_NIDENT 16

// ELF类别
#define ELFCLASS32 1
#define ELFCLASS64 2

// 段类型
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11

// 符号类型
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4

// 符号绑定
#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

// ELF32 文件头
typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

// ELF64 文件头
typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

// ELF32 段头
typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

// ELF64 段头
typedef struct {
    Elf64_Word sh_name;
    Elf64_Word sh_type;
    Elf64_Xword sh_flags;
    Elf64_Addr sh_addr;
    Elf64_Off sh_offset;
    Elf64_Xword sh_size;
    Elf64_Word sh_link;
    Elf64_Word sh_info;
    Elf64_Xword sh_addralign;
    Elf64_Xword sh_entsize;
} Elf64_Shdr;

// ELF32 符号表项
typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

// ELF64 符号表项
typedef struct {
    Elf64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    Elf64_Half st_shndx;
    Elf64_Addr st_value;
    Elf64_Xword st_size;
} Elf64_Sym;

// ELF32 程序头
typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

// ELF64 程序头
typedef struct {
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;
typedef struct dynamic {
    Elf32_Sword d_tag;
    union {
        Elf32_Sword d_val;
        Elf32_Addr d_ptr;
    } d_un;
} Elf32_Dyn;
typedef struct {
    Elf64_Sxword d_tag;
    union {
        Elf64_Xword d_val;
        Elf64_Addr d_ptr;
    } d_un;
} Elf64_Dyn;
// 架构通用定义（根据编译目标选择）
#ifdef __LP64__
    // 64位架构 (ARM64)
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Shdr Elf_Shdr;
    typedef Elf64_Sym Elf_Sym;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Addr Elf_Addr;
    typedef Elf64_Off Elf_Off;
    typedef Elf64_Word Elf_Word;
    typedef Elf64_Xword Elf_Xword;
#else
    // 32位架构 (ARM32)
    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Shdr Elf_Shdr;
    typedef Elf32_Sym Elf_Sym;
    typedef Elf32_Phdr Elf_Phdr;
    typedef Elf32_Addr Elf_Addr;
    typedef Elf32_Off Elf_Off;
    typedef Elf32_Word Elf_Word;
    #define Elf_Xword Elf32_Word
#endif
#if defined(__LP64__)
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif
// 内存映射信息结构
typedef struct {
    Elf_Addr start;
    Elf_Addr end;
    char perms[5];
    Elf_Off offset;
    char dev[6];
    unsigned long inode;
    char pathname[256];
} MemoryMap;

// 符号信息结构
typedef struct {
    char name[128];
    Elf_Addr address;
    Elf_Xword size;
    unsigned char type;
} SymbolInfo;

// 符号类型和绑定宏
#define ELF_ST_BIND(i) ((i) >> 4)
#define ELF_ST_TYPE(i) ((i) & 0xf)
#define ELF_ST_INFO(b,t) (((b) << 4) + ((t) & 0xf))

// 函数声明
int parse_maps_file(MemoryMap *maps, int max_maps);
int find_library_base(const char *lib_name, Elf_Addr *base_addr);
int parse_elf_symbols(const char *lib_path, Elf_Addr base_addr, 
                     SymbolInfo *symbols, int max_symbols);
Elf_Addr find_symbol_address(const char *lib_name, const char *symbol_name);
int read_memory_maps(const char *maps_content, MemoryMap *maps, int max_maps);

#endif // ELF_PARSER_H 