# CEF Node Addon

基于 Chromium Embedded Framework (CEF) 的 Node.js 原生扩展，提供网页截图功能。使用无窗口渲染 (OSR) 方式创建浏览器，等待页面加载完毕后截取 PNG 格式图片。

> **注意**：本项目主要用于示例和起步，生产环境请根据需要补充错误处理、并发控制和资源回收策略。

## 功能特性

- ✅ 无窗口渲染（Off-Screen Rendering）
- ✅ 支持完整的 Chromium 渲染引擎
- ✅ 可自定义浏览器窗口大小
- ✅ 等待页面加载完成后截图
- ✅ 输出 PNG 格式

## 环境要求

- Windows 10/11 x64
- Visual Studio 2022（含 Desktop C++ workload）
- Node.js 18+
- Python 3.x（node-gyp 依赖）
- CMake 3.20+（推荐通过 `winget install Kitware.CMake` 安装）

> **注意**：如果使用便携版 CMake，需要将其 bin 目录添加到 PATH 环境变量。

## 安装步骤

### 1. 构建 CEF Wrapper（仅首次或更新 CEF 后需要）

```powershell
# 回到项目根目录
cd ../..

# 使用 CMake 生成 Visual Studio 项目
cmake `
  -S third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta `
  -B third_party/cef/build_wrapper `
  -G "Visual Studio 17 2022" -A x64

# 构建 libcef_dll_wrapper
cmake `
  --build third_party/cef/build_wrapper `
  --config Release `
  --target libcef_dll_wrapper

# 复制生成的 lib 到 CEF Release 目录
Copy-Item third_party/cef/build_wrapper/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib `
  third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta/Release/ -Force
```

### 2. 构建 Node.js 扩展

```powershell
# 进入本包目录
cd packages/cef-node-addon

# 设置 CEF_ROOT 环境变量
$env:CEF_ROOT = (Resolve-Path ../../third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta)

# 安装依赖
npm install --ignore-scripts

# 构建扩展
npx node-gyp rebuild -- "-DCEF_ROOT=$env:CEF_ROOT"
```

构建成功后，会在 `build/Release/` 目录下生成 `cefaddon.node`，同时会自动复制 CEF 运行时所需的 DLL 和资源文件。

## 使用示例

```javascript
const cef = require('./build/Release/cefaddon.node');

// 初始化 CEF
cef.init({
  cachePath: './cache',
  logFile: './cef_debug.log',
  logSeverity: 'info'
});

// 截图
cef.capture({
  url: 'https://example.com',
  output: 'screenshot.png',
  width: 1280,
  height: 720
});

console.log('Screenshot saved!');
```

运行示例：

```powershell
node demo.js
```

## 项目结构

```text
cef-node-addon/
├── src/
│   ├── cef_app.h/cc           # CEF 初始化和配置
│   ├── screenshot_handler.h/cc # 截图处理器（CefClient + CefRenderHandler）
│   ├── node_addon.cc          # N-API 封装，导出 Node.js 接口
│   └── subprocess.cc          # CEF 子进程入口点
├── binding.gyp                # Node-gyp 构建配置
├── CMakeLists.txt             # CMake 构建配置（可选）
├── package.json
├── demo.js                    # 使用示例
└── README.md
```

## API 文档

### cef.init(options)

初始化 CEF 运行时。

**参数：**

- `options.cachePath` (string): 缓存目录路径
- `options.logFile` (string): 日志文件路径
- `options.logSeverity` (string): 日志级别（'verbose', 'info', 'warning', 'error', 'disable'）

### cef.capture(options)

截取网页截图。

**参数：**

- `options.url` (string): 要截图的网页 URL
- `options.output` (string): 输出 PNG 文件路径
- `options.width` (number): 视口宽度（像素）
- `options.height` (number): 视口高度（像素）

## 注意事项

1. **仅支持 Windows x64**：当前版本仅针对 Windows 64 位系统。
2. **沙箱已关闭**：`CEF_USE_SANDBOX` 已禁用，依赖当前 Node.js 进程作为子进程入口。
3. **阻塞调用**：截图是同步阻塞操作，会在 CEF 消息循环内等待页面加载完成。长页面或慢网速可能需要调整超时。
4. **GPU 加速**：当前默认禁用 GPU 并强制使用 OSR。如需 GPU 加速，请根据需求调整 CEF 设置。

## 更新 CEF 版本

如需使用不同版本的 CEF：

```powershell
# 回到项目根目录
cd ../..

# 运行下载脚本（需自行指定版本）
pwsh scripts/fetch_cef.ps1 -Version <CEF_VERSION>
```

请确保下载 **standard** 版本（包含 wrapper 源码）。

## 后续改进

- [ ] 增加异步调用与并发队列，避免阻塞 Node 事件循环
- [ ] 改为独立子进程/worker 负责截图，防止与业务代码竞争
- [ ] 增加更多浏览器设置（User-Agent、Cookie、证书等）
- [ ] 完善错误恢复逻辑和资源清理
- [ ] 支持更多输出格式（JPEG、WebP 等）

## 许可证

MIT
