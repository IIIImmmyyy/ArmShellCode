#include "dobby_symbol_resolver.h"
#include "dobby_maps_parser.h"
#include "file_ops.h"
#include "logger.h"
#include "string_utils.h"
#include "android_utils.h"

#define TAG "DOBBY_SYMBOL_RESOLVER"

// 全局resolver上下文

// 辅助函数：将数字转换为十六进制字符串
static void hex_to_str_fixed(char* buf, uintptr_t val, int width) {
    const char hex_chars[] = "0123456789ABCDEF";
    buf[width] = '\0';
    for (int i = width - 1; i >= 0; i--) {
        buf[i] = hex_chars[val & 0xF];
        val >>= 4;
    }
}

// 新增：打印符号表项的二进制数据
static void print_sym_binary_data(const ElfW(Sym)* sym) {
    char log_buf[512];
    char hex_buf[8];
    
    // 构建日志消息
    str_copy(log_buf, "Symbol binary data:\n  st_name: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, sym->st_name, 8);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    
    str_append(log_buf, "\n  st_info: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, sym->st_info, 2);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    str_append(log_buf, " (bind: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, ELF_ST_BIND(sym->st_info), 1);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    str_append(log_buf, ", type: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, ELF_ST_TYPE(sym->st_info), 1);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    str_append(log_buf, ")\n", sizeof(log_buf));
    
    str_append(log_buf, "  st_other: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, sym->st_other, 2);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    
    str_append(log_buf, "\n  st_shndx: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, sym->st_shndx, 4);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    
    str_append(log_buf, "\n  st_value: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, sym->st_value, 16);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    
    str_append(log_buf, "\n  st_size: 0x", sizeof(log_buf));
    hex_to_str_fixed(hex_buf, sym->st_size, 16);
    str_append(log_buf, hex_buf, sizeof(log_buf));
    
//    LOG_D(TAG, log_buf);
    
    // 打印原始字节
    str_copy(log_buf, "Raw bytes: ", sizeof(log_buf));
    const unsigned char* raw = (const unsigned char*)sym;
    for (size_t i = 0; i < sizeof(ElfW(Sym)); i++) {
        hex_to_str_fixed(hex_buf, raw[i], 2);
        str_append(log_buf, hex_buf, sizeof(log_buf));
        str_append(log_buf, " ", sizeof(log_buf));
    }
    LOG_D(TAG, log_buf);
}

// Dobby 风格的ELF上下文结构体
typedef struct {
    void* header;
    uintptr_t load_bias;
    ElfW(Sym)* dynsym;
    const char* dynstr;
    size_t dynsym_count;
    ElfW(Sym)* symtab;
    const char* strtab;
    size_t symtab_count;
} dobby_elf_ctx_t;

// 初始化上下文
static int dobby_elf_ctx_init(dobby_elf_ctx_t* ctx, void* header, uintptr_t load_address) {
    ElfW(Ehdr)* ehdr = (ElfW(Ehdr)*)header;
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1) {
        return RESOLVER_ERR_INVALID_ELF;
    }
    
    ctx->header = header;
    ElfW(Phdr)* phdr = (ElfW(Phdr)*)((char*)header + ehdr->e_phoff);

    // 计算 load_bias，Dobby 的关键逻辑
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            ctx->load_bias = load_address - phdr[i].p_vaddr;
            break;
        }
    }
    
    ElfW(Shdr)* shdr = (ElfW(Shdr)*)((char*)header + ehdr->e_shoff);
    const char* sh_strtab = (const char*)header + shdr[ehdr->e_shstrndx].sh_offset;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char* name = sh_strtab + shdr[i].sh_name;
        if (shdr[i].sh_type == SHT_DYNSYM) {
            ctx->dynsym = (ElfW(Sym)*)((char*)header + shdr[i].sh_offset);
            ctx->dynsym_count = shdr[i].sh_size / sizeof(ElfW(Sym));
        } else if (shdr[i].sh_type == SHT_SYMTAB) {
            ctx->symtab = (ElfW(Sym)*)((char*)header + shdr[i].sh_offset);
            ctx->symtab_count = shdr[i].sh_size / sizeof(ElfW(Sym));
        } else if (shdr[i].sh_type == SHT_STRTAB) {
            if (str_compare(name, ".dynstr") == 0) {
                ctx->dynstr = (const char*)((char*)header + shdr[i].sh_offset);
            } else if (str_compare(name, ".strtab") == 0) {
                ctx->strtab = (const char*)((char*)header + shdr[i].sh_offset);
            }
        }
    }
    return RESOLVER_SUCCESS;
}

// 遍历符号
static uintptr_t dobby_elf_iterate_symbols(dobby_elf_ctx_t* ctx, const char* symbol_name) {

    if (ctx->dynsym) {
//        LOG_I(TAG, "Iterating .dynsym table...");
        for (size_t i = 0; i < ctx->dynsym_count; i++) {
            const char* current_sym_name = ctx->dynstr + ctx->dynsym[i].st_name;

            if (str_compare(symbol_name, current_sym_name) == 0) {
                // 打印符号表项的二进制数据
//                print_sym_binary_data(&ctx->dynsym[i]);
                if (ctx->dynsym[i].st_value != 0) {
                    return ctx->dynsym[i].st_value;
                }
            }
        }
    }
    if (ctx->symtab) {
//        LOG_I(TAG, "Iterating .symtab table...");
        for (size_t i = 0; i < ctx->symtab_count; i++) {
            const char* current_sym_name = ctx->strtab + ctx->symtab[i].st_name;

            if (str_compare(symbol_name, current_sym_name) == 0) {
                // 打印符号表项的二进制数据

                if (ctx->symtab[i].st_value != 0) {
                   return ctx->symtab[i].st_value;
                }
            }
        }
    }
    return 0;
}



// 修改主要的符号解析函数
int dobby_resolve_symbol(const char* lib_name, const char* symbol_name, uintptr_t* symbol_address) {
    if (!symbol_address) {
        return RESOLVER_ERR_NOT_FOUND;
    }



    dobby_runtime_module_t module;
    if (dobby_get_runtime_module(lib_name, &module) != MAPS_PARSER_SUCCESS) {
//        LOG_E(TAG, "Failed to find library in maps");
        return RESOLVER_ERR_NOT_FOUND;
    }

    file_info_t file_info;
    if (file_open_and_map(module.path, &file_info) != FILE_SUCCESS) {
//        LOG_E_ERR(TAG, "Failed to open library file", 0);
        return RESOLVER_ERR_FILE_ACCESS;
    }
    
    dobby_elf_ctx_t elf_ctx = {0};
    if (dobby_elf_ctx_init(&elf_ctx, file_info.mapped_memory, module.load_address) != RESOLVER_SUCCESS) {
        file_close(&file_info);
        return RESOLVER_ERR_INVALID_ELF;
    }

    uintptr_t offset = dobby_elf_iterate_symbols(&elf_ctx, symbol_name);
    file_close(&file_info);

    if (offset == 0) {
//        LOG_E(TAG, "Symbol not found");
        return RESOLVER_ERR_NOT_FOUND;
    }

    *symbol_address = elf_ctx.load_bias + offset;
//    LOG_I_HEX(TAG, "Symbol resolved via ELF parsing:", *symbol_address);
    return RESOLVER_SUCCESS;
} 
