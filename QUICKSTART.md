# 快速开始指南

本指南帮助你快速开始使用 CEF 和 WebView2 截图工具。

## 前置要求检查

### 通用要求

- ✅ Windows 10/11 x64
- ✅ PowerShell 5.1+

### CEF Node Addon 额外要求

- ✅ Visual Studio 2022（含 Desktop C++ workload）
- ✅ Node.js 18+
- ✅ Python 3.x

### WebView2 Screenshot Tool 额外要求

- ✅ .NET 8 SDK

## 方案选择

### 我应该选择哪个工具？

**选择 WebView2 Screenshot Tool，如果：**

- ✅ 你需要一个简单的命令行截图工具
- ✅ 你希望最小化依赖和体积
- ✅ 你只在 Windows 平台使用
- ✅ 你不需要深度自定义浏览器行为

**选择 CEF Node Addon，如果：**

- ✅ 你需要在 Node.js 项目中集成截图功能
- ✅ 你需要完整的 Chromium 功能和自定义选项
- ✅ 你计划未来支持其他平台
- ✅ 你需要更精细的控制（Cookie、证书、GPU 设置等）

## 快速开始：WebView2 Screenshot Tool

### 1. 安装 .NET 8 SDK

```powershell
winget install Microsoft.DotNet.SDK.8
```

### 2. 构建项目

```powershell
cd packages/webview2-screenshot-tool
dotnet build -c Release
```

### 3. 运行测试

```powershell
# 使用命令行直接运行
.\bin\Release\net8.0\webview2-screenshot.exe `
  --url https://example.com `
  --output test.png

# 或使用 Node.js demo
node demo.js
```

## 快速开始：CEF Node Addon

### 1. 安装开发工具

确保已安装：

- Visual Studio 2022（Desktop C++ workload）
- Node.js 18+
- Python 3.x

**安装 CMake：**

```powershell
# 推荐：使用 winget 安装系统级 CMake
winget install Kitware.CMake

# 或者：使用便携版 CMake（下载后添加到 PATH）
# 从 https://cmake.org/download/ 下载
# $env:PATH = "C:\path\to\cmake\bin;$env:PATH"
```

### 2. 构建 CEF Wrapper（仅首次）

```powershell
# 回到项目根目录
cd ../..

# 生成并构建 CEF wrapper
cmake `
  -S third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta `
  -B third_party/cef/build_wrapper `
  -G "Visual Studio 17 2022" -A x64

cmake `
  --build third_party/cef/build_wrapper `
  --config Release `
  --target libcef_dll_wrapper

# 复制 lib 文件
Copy-Item third_party/cef/build_wrapper/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib `
  third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta/Release/ -Force
```

### 3. 构建 Node 扩展

```powershell
cd packages/cef-node-addon

# 设置 CEF_ROOT
$env:CEF_ROOT = (Resolve-Path ../../third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta)

# 安装依赖并构建
npm install --ignore-scripts
npx node-gyp rebuild -- "-DCEF_ROOT=$env:CEF_ROOT"
```

### 4. 运行测试

```powershell
node demo.js
```

## 故障排除

### WebView2 Runtime 未找到

```powershell
winget install Microsoft.EdgeWebView2Runtime
```

### Visual Studio 找不到

确保安装了 Visual Studio 2022 并包含 "Desktop development with C++" workload。

### node-gyp 构建失败

1. 确保 Python 3.x 在 PATH 中
2. 确保 Visual Studio 2022 正确安装
3. 以管理员身份运行 PowerShell

### CEF_ROOT 未设置

```powershell
# 设置环境变量
$env:CEF_ROOT = (Resolve-Path ./third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta)
```

### CMake 未找到

```powershell
# 安装 CMake
winget install Kitware.CMake

# 或检查 PATH 是否包含 cmake
cmake --version
```

## 下一步

- 阅读 [CEF Node Addon 详细文档](packages/cef-node-addon/README.md)
- 阅读 [WebView2 Screenshot Tool 详细文档](packages/webview2-screenshot-tool/README.md)
- 查看 [主 README](README.md) 了解项目结构

## 获取帮助

如遇到问题，请：

1. 查看各工具的 README 中的"故障排除"章节
2. 在 [GitHub Issues](https://github.com/KarinJS/cef/issues) 提交问题
3. 确保环境符合所有前置要求
