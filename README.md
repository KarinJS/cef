# CEF & WebView2 Screenshot Tools

本项目提供两个 Windows 平台的网页截图工具，适用于不同场景：

- **CEF Node Addon**：基于 Chromium Embedded Framework (CEF) 的 Node.js 原生扩展
- **WebView2 Screenshot Tool**：基于 Edge WebView2 Runtime 的轻量级命令行工具

## 📚 文档导航

- **[快速开始指南](QUICKSTART.md)** - 5分钟上手教程
- [CEF Node Addon 详细文档](packages/cef-node-addon/README.md)
- [WebView2 Screenshot Tool 详细文档](packages/webview2-screenshot-tool/README.md)

## 项目结构

```text
packages/
├── cef-node-addon/               # CEF Node.js 原生扩展
│   ├── src/                      # C++ 源码
│   │   ├── cef_app.h/cc
│   │   ├── screenshot_handler.h/cc
│   │   ├── node_addon.cc
│   │   └── subprocess.cc
│   ├── binding.gyp               # Node-gyp 构建配置
│   ├── CMakeLists.txt            # CMake 构建配置（可选）
│   ├── package.json
│   ├── demo.js
│   └── README.md
│
└── webview2-screenshot-tool/     # WebView2 截图工具
    ├── src/
    │   └── Program.cs            # C# 源码
    ├── webview2-screenshot.csproj
    ├── package.json
    ├── demo.js
    └── README.md

third_party/
└── cef/                          # CEF 运行时库

scripts/
└── fetch_cef.ps1                 # CEF 下载脚本

tools/                            # 工具目录（不包含在仓库中）
```

## 环境要求

### 通用环境

- Windows 10/11 x64
- PowerShell 5.1+

### CEF Node Addon

- Visual Studio 2022（含 Desktop C++ workload）
- Node.js 18+
- Python 3.x（node-gyp 依赖）
- CMake 3.20+（推荐系统安装，也支持便携版）

### WebView2 Screenshot Tool

- .NET 8 SDK（通过 `winget install Microsoft.DotNet.SDK.8` 安装）
- WebView2 Runtime（Windows 11 自带，Windows 10 需安装）

## 快速开始

### 选项 1：使用 WebView2 工具（推荐用于快速截图）

```powershell
# 1. 构建
cd packages/webview2-screenshot-tool
dotnet build -c Release

# 2. 运行
.\bin\Release\net8.0\webview2-screenshot.exe `
  --url https://example.com `
  --output screenshot.png `
  --width 1280 --height 720
```

### 选项 2：使用 CEF Node Addon（推荐用于集成到 Node.js 项目）

```powershell
# 1. 构建 CEF wrapper（仅首次）
cmake `
  -S third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta `
  -B third_party/cef/build_wrapper `
  -G "Visual Studio 17 2022" -A x64
cmake `
  --build third_party/cef/build_wrapper --config Release --target libcef_dll_wrapper
Copy-Item third_party/cef/build_wrapper/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib `
  third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta/Release/ -Force

# 2. 构建 Node 扩展
cd packages/cef-node-addon
$env:CEF_ROOT = (Resolve-Path ../../third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta)
npm install --ignore-scripts
npx node-gyp rebuild -- "-DCEF_ROOT=$env:CEF_ROOT"

# 3. 运行示例
node demo.js
```

## 工具对比

| 特性 | CEF Node Addon | WebView2 Tool |
|------|----------------|---------------|
| 体积 | 大（~200MB CEF 运行时） | 小（系统自带运行时） |
| 依赖 | 独立 CEF 运行时 | 系统 WebView2 Runtime |
| 使用场景 | Node.js 项目集成 | 独立命令行工具 |
| 灵活性 | 高（可自定义 CEF 配置） | 中（WebView2 API 限制） |
| 构建复杂度 | 高（需要 C++ 编译环境） | 低（dotnet build 即可） |

## 详细文档

- [CEF Node Addon 文档](packages/cef-node-addon/README.md)
- [WebView2 Screenshot Tool 文档](packages/webview2-screenshot-tool/README.md)

## License

MIT  
