# WebView2 Screenshot Tool

一个基于 Edge WebView2 Runtime 的轻量级无头截图工具，可将网页渲染为 PNG 图片。相比 CEF，体积小巧，依赖系统自带的 WebView2 运行时。

## 功能特性

- ✅ 基于系统 WebView2 Runtime，无需额外打包大型运行时
- ✅ 命令行工具，使用简单
- ✅ 支持自定义视口尺寸
- ✅ 输出 PNG 格式
- ✅ 可配置用户数据目录

## 环境要求

- Windows 10/11 x64
- .NET 8 SDK
- WebView2 Runtime（Windows 11 自带；Windows 10 可能需要安装 Evergreen Runtime）

### 安装 .NET 8 SDK

如果尚未安装 .NET 8 SDK：

```powershell
winget install Microsoft.DotNet.SDK.8
```

## 构建

```powershell
cd packages/webview2-screenshot-tool
dotnet build -c Release
```

构建成功后，可执行文件位于：`bin/Release/net8.0/webview2-screenshot.exe`

## 使用方法

### 基本用法

```powershell
.\bin\Release\net8.0\webview2-screenshot.exe `
  --url https://example.com `
  --output screenshot.png
```

### 完整参数示例

```powershell
.\bin\Release\net8.0\webview2-screenshot.exe `
  --url https://example.com `
  --output screenshot.png `
  --width 1280 `
  --height 720 `
  --userdata .\wv2-cache
```

### 参数说明

| 参数 | 必需 | 默认值 | 说明 |
|------|------|--------|------|
| `--url` | ✅ | - | 要截图的网页 URL |
| `--output` | ✅ | - | PNG 输出文件路径 |
| `--width` | ❌ | 1280 | 视口宽度（像素） |
| `--height` | ❌ | 720 | 视口高度（像素） |
| `--userdata` | ❌ | 临时目录 | WebView2 用户数据目录 |

## 示例

### 截取整页

```powershell
.\bin\Release\net8.0\webview2-screenshot.exe `
  --url https://www.bing.com `
  --output bing-homepage.png `
  --width 1920 --height 1080
```

### 使用自定义缓存目录

```powershell
.\bin\Release\net8.0\webview2-screenshot.exe `
  --url https://github.com `
  --output github.png `
  --userdata D:\MyCache\webview2
```

### 从 Node.js 调用

```javascript
const { execSync } = require('child_process');
const path = require('path');

const toolPath = path.join(__dirname, 'bin/Release/net8.0/webview2-screenshot.exe');

try {
  execSync(`"${toolPath}" --url https://example.com --output output.png`, {
    stdio: 'inherit'
  });
  console.log('Screenshot created successfully!');
} catch (error) {
  console.error('Screenshot failed:', error);
}
```

## 注意事项

1. **WebView2 Runtime 依赖**：
   - Windows 11 默认已安装
   - Windows 10 可能需要安装 [WebView2 Evergreen Runtime](https://developer.microsoft.com/microsoft-edge/webview2/)

2. **截图机制**：
   - 使用 WinForms 隐藏窗口 + `CapturePreviewAsync` API
   - 等待 `NavigationCompleted` 事件后截图
   - 适合基础静态页面截屏需求

3. **性能考虑**：
   - 每次运行会创建新的 WebView2 实例
   - 对于批量截图，建议复用用户数据目录以提高性能

4. **动态内容**：
   - 当前实现仅等待主文档加载完成
   - 对于需要 JavaScript 执行时间的动态内容，可能需要添加延迟

## 项目结构

```text
webview2-screenshot-tool/
├── src/
│   └── Program.cs              # 主程序（包含参数解析和截图逻辑）
├── webview2-screenshot.csproj  # .NET 项目配置
├── package.json                # npm 脚本配置
├── demo.js                     # Node.js 调用示例
├── README.md
└── bin/                        # 构建输出
    └── Release/
        └── net8.0/
            └── webview2-screenshot.exe
```

## 与 CEF 对比

| 特性 | WebView2 Tool | CEF Node Addon |
|------|---------------|----------------|
| 运行时大小 | ~100KB（依赖系统） | ~200MB |
| 构建复杂度 | 低（dotnet build） | 高（C++ + CMake） |
| 启动速度 | 快 | 较慢 |
| 自定义能力 | 中等 | 高 |
| 跨平台 | 仅 Windows | 多平台（需适配） |

## 故障排除

### WebView2 Runtime 未找到

**错误信息**：`Could not find a compatible WebView2 Runtime`

**解决方法**：

```powershell
# 下载并安装 WebView2 Runtime
winget install Microsoft.EdgeWebView2Runtime
```

### .NET 运行时错误

**错误信息**：`You must install .NET 8.0 to run this application`

**解决方法**：

```powershell
winget install Microsoft.DotNet.SDK.8
```

## 后续改进

- [ ] 添加等待延迟参数，支持动态内容
- [ ] 支持 JPEG/WebP 输出格式
- [ ] 添加自定义 User-Agent
- [ ] 支持截取特定元素
- [ ] 添加日志和调试选项

## 许可证

MIT
