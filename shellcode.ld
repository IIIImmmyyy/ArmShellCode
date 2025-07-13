/* Shellcode链接脚本 - 生成紧凑的二进制文件 */

ENTRY(_start)

SECTIONS
{
    /* 将所有代码段合并到一个连续的内存区域 */
    . = 0x0;
    
    /* 代码段 - 放在最前面，确保_start是第一个 */
    .text : {
        *(.text._start)     /* 确保_start函数在最前面 */
        *(.text)            /* 其他代码 */
        *(.text.*)          /* 所有代码段 */
    }
    
    /* 只读数据段 */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* 初始化数据段 */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* 未初始化数据段 */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /* 丢弃不需要的段 */
    /DISCARD/ : {
        *(.note.*)
        *(.comment)
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.gnu.hash)
        *(.gnu.version)
        *(.gnu.version_d)
        *(.gnu.version_r)
        *(.dynsym)
        *(.dynstr)
        *(.dynamic)
        *(.hash)
        *(.plt)
        *(.plt.got)
        *(.got)
        *(.got.plt)
        *(.interp)
        *(.rela.*)
        *(.rel.*)
    }
} 
