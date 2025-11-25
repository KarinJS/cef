const { spawnSync } = require('child_process')
const path = require('path')
const fs = require('fs')

// 路径指向 bin/Release/net8.0 下的可执行文件
const exePath = path.resolve(__dirname, 'bin', 'Release', 'net8.0', 'webview2-screenshot.exe')

if (!fs.existsSync(exePath)) {
  console.error('未找到 webview2-screenshot.exe')
  console.error('请先运行: dotnet build -c Release')
  process.exit(1)
}

const output = path.resolve(__dirname, 'screenshot.png')
const url = 'https://example.com'

console.log('运行截图工具...')
console.log('URL:', url)
console.log('输出:', output)

const result = spawnSync(exePath, [
  '--url', url,
  '--output', output,
  '--width', '1280',
  '--height', '720',
  '--userdata', path.resolve(__dirname, 'wv2-cache')
], {
  stdio: 'inherit'
})

if (result.status !== 0) {
  console.error('截图失败，退出码:', result.status)
  process.exit(result.status || 1)
}

console.log('截图已保存到:', output)
