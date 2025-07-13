# CLion Android Shellcode 项目配置指南

## 🎯 快速配置步骤

### 1. 添加工具链 (Toolchain)

1. **File** → **Settings** → **Build, Execution, Deployment** → **Toolchains**
2. 点击 **+** 添加新工具链，选择 **Remote Host** 或直接使用 **System**
3. 配置以下工具链：

#### Android ARM64 工具链
- **Name**: `Android-ARM64`
- **C Compiler**: `F:\AndroidSDK\ndk\25.1.8937393\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android21-clang.exe`
- **C++ Compiler**: `F:\AndroidSDK\ndk\25.1.8937393\toolchains\llvm\prebuilt\windows-x86_64\bin\aarch64-linux-android21-clang++.exe`
- **Debugger**: `F:\AndroidSDK\ndk\25.1.8937393\toolchains\llvm\prebuilt\windows-x86_64\bin\lldb.exe`

#### Android ARM32 工具链
- **Name**: `Android-ARM32`
- **C Compiler**: `F:\AndroidSDK\ndk\25.1.8937393\toolchains\llvm\prebuilt\windows-x86_64\bin\armv7a-linux-androideabi21-clang.exe`
- **C++ Compiler**: `F:\AndroidSDK\ndk\25.1.8937393\toolchains\llvm\prebuilt\windows-x86_64\bin\armv7a-linux-androideabi21-clang++.exe`
- **Debugger**: `F:\AndroidSDK\ndk\25.1.8937393\toolchains\llvm\prebuilt\windows-x86_64\bin\lldb.exe`

### 2. 配置CMake

1. **File** → **Settings** → **Build, Execution, Deployment** → **CMake**
2. 添加以下配置：

#### Android ARM64 配置
- **Name**: `Android-ARM64`
- **Build type**: `Release`
- **Toolchain**: `Android-ARM64`
- **CMake options**:
```
-DCMAKE_TOOLCHAIN_FILE=F:/AndroidSDK/ndk/25.1.8937393/build/cmake/android.toolchain.cmake
-DANDROID_ABI=arm64-v8a
-DANDROID_PLATFORM=android-21
-DCMAKE_BUILD_TYPE=Release
```
- **Build directory**: `cmake-build-android-arm64`

#### Android ARM32 配置
- **Name**: `Android-ARM32`
- **Build type**: `Release`
- **Toolchain**: `Android-ARM32`
- **CMake options**:
```
-DCMAKE_TOOLCHAIN_FILE=F:/AndroidSDK/ndk/25.1.8937393/build/cmake/android.toolchain.cmake
-DANDROID_ABI=armeabi-v7a
-DANDROID_PLATFORM=android-21
-DCMAKE_BUILD_TYPE=Release
```
- **Build directory**: `cmake-build-android-arm32`

### 3. 环境变量设置

在 **Settings** → **Build, Execution, Deployment** → **CMake** 的 **Environment variables** 中添加：

```
ANDROID_NDK=F:\AndroidSDK\ndk\25.1.8937393
```

### 4. 构建目标

配置完成后，在CLion的构建选择器中应该会看到：
- `shellcode_elf | Android-ARM64`
- `shellcode_elf | Android-ARM32`

## 🚀 快速构建

### 方法1：使用CLion界面
1. 在右上角选择构建配置（如 `Android-ARM64`）
2. 选择目标 `shellcode_elf`
3. 点击 **Build** 按钮（或按 **Ctrl+F9**）

### 方法2：使用Build菜单
- **Build** → **Build Project** (**Ctrl+F9**)
- **Build** → **Rebuild Project** (**Ctrl+Shift+F9**)

### 方法3：右键上下文菜单
- 在项目树中右键点击 `CMakeLists.txt`
- 选择 **Build Project**

## 📁 输出文件位置

构建完成后，生成的文件将位于：

### ARM64版本：
```
cmake-build-android-arm64/
├── shellcode           # ELF可执行文件
├── shellcode.bin       # 纯二进制shellcode
├── shellcode_data.h    # C头文件格式
└── shellcode.asm       # 反汇编文件
```

### ARM32版本：
```
cmake-build-android-arm32/
├── shellcode           # ELF可执行文件
├── shellcode.bin       # 纯二进制shellcode
├── shellcode_data.h    # C头文件格式
└── shellcode.asm       # 反汇编文件
```

## 🛠️ 调试和开发

### 代码提示和补全
- CLion会自动识别Android NDK的头文件
- 支持跳转到定义、查找引用等功能
- 内联汇编也有基本的语法高亮

### 代码分析
- **Code** → **Inspect Code** 进行静态分析
- **Code** → **Reformat Code** 格式化代码

### 自定义构建步骤
如果需要添加自定义构建步骤：
1. **Run** → **Edit Configurations**
2. 添加 **CMake Application** 配置
3. 设置目标为 `shellcode_elf`

## ⚡ 快捷键

- **Ctrl+F9**: 构建项目
- **Ctrl+Shift+F9**: 重新构建项目
- **Shift+F10**: 运行（如果配置了运行目标）
- **Alt+Shift+F10**: 选择配置并运行

## 🔧 故障排除

### 常见问题：

1. **找不到编译器**
   - 检查NDK路径是否正确
   - 确保NDK版本支持指定的API级别

2. **链接错误**
   - 检查CMakeLists.txt中的链接选项
   - 确保所有源文件都存在

3. **头文件找不到**
   - 检查include目录设置
   - 确保所有必要的头文件都存在

4. **Python脚本执行失败**
   - 安装Python 3
   - 或者临时注释掉生成.h文件的部分

### 清理和重建：
- **Build** → **Clean** 清理构建文件
- 删除 `cmake-build-*` 目录进行完全重建 