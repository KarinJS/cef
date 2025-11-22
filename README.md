# CEF Node Screenshot DLL (Windows)

本目录提供一个使用 Chromium Embedded Framework (CEF) 的 DLL，并通过 Node 原生扩展暴露一个简单的截图 API。代码以无窗口渲染 (OSR) 方式创建浏览器，等待页面加载完毕后截取 PNG。

> 说明：未附带 CEF 运行时，请按下方步骤下载。代码主要用于示例/起步，真实项目请根据需要补充错误处理、并发控制和资源回收策略。

## 快速开始

1) 安装工具  
   - Visual Studio 2022 (含 Desktop C++ workload)  
   - Python 3（Node-gyp 需要）  
   - Node.js 18+  
   - CMake 3.20+（已在 `tools/cmake/cmake-4.2.0-windows-x86_64` 下载便携版）

2) 拉取 CEF 运行时  
   - 已预下发 `third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta`（standard 包含 wrapper 源码）。  
   - 若需要更新，可参考脚本：`pwsh scripts/fetch_cef.ps1 -Version <CEF_VERSION>` 并确保使用 standard 版，以便包含 wrapper 源码。

3) 构建 libcef_dll_wrapper（仅首次或更新 CEF 后需要）  
   ```powershell
   $env:PATH = "$env:PATH;$(Resolve-Path tools/cmake/cmake-4.2.0-windows-x86_64/bin)"
   .\tools\cmake\cmake-4.2.0-windows-x86_64\bin\cmake.exe `
     -S third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta `
     -B third_party/cef/build_wrapper `
     -G "Visual Studio 17 2022" -A x64
   .\tools\cmake\cmake-4.2.0-windows-x86_64\bin\cmake.exe `
     --build third_party/cef/build_wrapper --config Release --target libcef_dll_wrapper
   Copy-Item third_party/cef/build_wrapper/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib `
     third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta/Release/ -Force
   ```

4) 构建 Node 扩展  
   ```powershell
   $env:CEF_ROOT = (Resolve-Path ./third_party/cef/cef_binary_143.0.6+g05edb3b+chromium-143.0.7499.40_windows64_beta)
   npm install --ignore-scripts
   npx node-gyp rebuild -- "-DCEF_ROOT=$env:CEF_ROOT"
   ```
   生成的 `build/Release/cefaddon.node` 供 `require()` 使用，目录下会自动复制 CEF 运行时所需 DLL/资源。

5) 调用示例 (Node)
   ```powershell
   node demo.js
   ```

## 项目结构
- `src/cef_app.*`：初始化/关闭 CEF，设置无窗口渲染参数。  
- `src/screenshot_handler.*`：实现 `CefClient` 与 `CefRenderHandler`，保存 PNG。  
- `src/node_addon.cc`：N-API 封装，导出 `init` 与 `capture`。  
- `binding.gyp`：Node 构建配置。  
- `CMakeLists.txt`：可用 CMake 单独构建 DLL（非必须）。  
- `scripts/fetch_cef.ps1`：下载 CEF 的示例脚本（需自行指定版本）。  

## 运行时注意
- 仅针对 Windows x64；`CEF_USE_SANDBOX` 关闭，依赖当前进程 (node.exe) 作为子进程入口。  
- 截图是阻塞调用，会在 CEF 消息循环内等待页面加载与首次绘制；长页面/慢网速可能需要调整超时。  
- 如果页面需要 GPU/硬件加速，请根据需要调整 CEF 设置或启用 GPU；当前默认禁用 GPU 并强制使用 OSR。

## 后续可改进
- 增加异步调用与并发队列，避免阻塞 Node 事件循环。  
- 改为独立子进程/worker 负责截图，防止与业务 JS 同进程竞争。  
- 增加更多浏览器设置（UA、Cookie、证书等）与错误恢复逻辑。  
