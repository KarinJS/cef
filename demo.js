const path = require('path')
const fs = require('fs')

// 确保 DLL 可被找到
const releaseDir = path.resolve(__dirname, 'dist')
process.env.PATH = `${releaseDir};${process.env.PATH}`

// 默认指向已打包好的运行时目录（包含 libcef.dll、Resources、locales 等）
const defaultCefRoot = releaseDir

const cefRoot = process.env.CEF_ROOT || defaultCefRoot
const output = path.resolve(__dirname, 'shot.png')

  ; (async () => {
    try {
      console.log('[demo] PATH prepend:', releaseDir)
      console.log('[demo] CEF_ROOT:', cefRoot)
      console.log('[demo] cwd:', process.cwd())

      if (!fs.existsSync(cefRoot)) {
        throw new Error('CEF_ROOT 不存在，请设置环境变量或检查路径。')
      }


      const addon = require('./dist/cefaddon.node')
      console.log('[demo] addon loaded')

      addon.init({ cefRoot, cachePath: path.resolve(__dirname, 'cache') })
      console.log('[demo] init ok, capturing...')

      addon.capture({
        url: 'https://baidu.com',
        width: 1280,
        height: 720,
        output
      })

      console.log('[demo] Saved to', output)
      process.exit(0)
    } catch (err) {
      console.error('[demo] error:', err)
      process.exit(1)
    }
  })()

process.on('uncaughtException', (err) => {
  console.error('[demo] uncaughtException:', err)
  process.exit(1)
})
process.on('unhandledRejection', (reason) => {
  console.error('[demo] unhandledRejection:', reason)
  process.exit(1)
})
