cmake_minimum_required(VERSION 3.18.1)
project(AndroidShellCode)

# 强制设置为Android平台
if(NOT ANDROID)
    message(FATAL_ERROR "此项目仅支持Android平台，请使用Android NDK工具链")
endif()

# 编译标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Shellcode专用编译选项
set(SHELLCODE_FLAGS
        "-fPIC"                     # 位置无关代码
        "-fno-stack-protector"      # 关闭栈保护
        "-fomit-frame-pointer"      # 省略帧指针
        "-fno-builtin"              # 不使用内建函数
        "-ffreestanding"            # 独立环境
        "-nostdlib"                 # 不链接标准库
        "-nostartfiles"             # 不使用启动文件
        "-static"                   # 静态链接
        "-Os"                       # 优化大小
        "-Wall"                     # 警告
        "-Wextra"                   # 额外警告
)

# Shellcode专用链接选项
set(SHELLCODE_LINK_FLAGS
        "-nostdlib"
        "-nostartfiles" 
        "-static"
        "-Wl,--gc-sections"
        "-Wl,--strip-all"
        "-Wl,-T,${CMAKE_SOURCE_DIR}/shellcode.ld"
        "-Wl,--build-id=none"
)

# 应用编译选项
string(REPLACE ";" " " SHELLCODE_FLAGS_STR "${SHELLCODE_FLAGS}")
string(REPLACE ";" " " SHELLCODE_LINK_FLAGS_STR "${SHELLCODE_LINK_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SHELLCODE_FLAGS_STR}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SHELLCODE_LINK_FLAGS_STR}")

# 架构设置
if(ANDROID_ABI STREQUAL "arm64-v8a")
    add_definitions(-DARCH_ARM64)
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    add_definitions(-DARCH_ARM32)
else()
    message(FATAL_ERROR "不支持的Android ABI: ${ANDROID_ABI}")
endif()

add_definitions(-DANDROID_PLATFORM -D__ANDROID__)
include_directories(${CMAKE_SOURCE_DIR}/include)

# ===========================================
# 模块化源文件定义
# ===========================================

# 定义源文件列表
set(SOURCES
    src/modular_entry.c
    src/shellcode_main_refactored.c
    src/logger.c
    src/file_ops.c
    src/dobby_maps_parser.c
    src/dobby_symbol_resolver.c
    src/string_utils.c
    src/syscall_wrapper.c
)

# ===========================================
# 构建目标定义
# ===========================================

add_executable(shellcode ${SOURCES})
set_target_properties(shellcode PROPERTIES OUTPUT_NAME "shellcode")

# ===========================================
# 后期处理命令
# ===========================================

# 生成二进制文件
add_custom_command(TARGET shellcode POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary -j .text -j .rodata -j .data
    $<TARGET_FILE:shellcode>
    ${CMAKE_BINARY_DIR}/shellcode.bin
    COMMENT "gen shellcode.bin"
)

# 生成反汇编
add_custom_command(TARGET shellcode POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d $<TARGET_FILE:shellcode> > ${CMAKE_BINARY_DIR}/shellcode.asm
    COMMENT "gen shellcode.asm"
)

# ===========================================
# 构建信息
# ===========================================

message(STATUS "===== 模块化 Android Shellcode 构建配置 =====")
message(STATUS "架构: ${ANDROID_ABI}")
message(STATUS "输出目标: shellcode.bin, shellcode.asm")
message(STATUS "=============================================")

# 生成链接脚本
configure_file(
    ${CMAKE_SOURCE_DIR}/shellcode.ld.in
    ${CMAKE_SOURCE_DIR}/shellcode.ld
    @ONLY
)
