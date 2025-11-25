const path = require('path')
const fs = require('fs')

// 确保 DLL 可被找到
const releaseDir = path.resolve(__dirname, 'build', 'Release')
process.env.PATH = `${releaseDir};${process.env.PATH}`

const output = path.resolve(__dirname, 'screenshot.png')

  ; (async () => {
    try {
      console.log('[demo] Release directory:', releaseDir)
      console.log('[demo] Current working directory:', process.cwd())

      if (!fs.existsSync(releaseDir)) {
        throw new Error('build/Release 目录不存在，请先构建项目。')
      }

      const addon = require('./build/Release/cefaddon.node')
      console.log('[demo] addon loaded')

      addon.init({
        cachePath: path.resolve(__dirname, 'cache'),
        logFile: path.resolve(__dirname, 'cef_debug.log'),
        logSeverity: 'info'
      })
      console.log('[demo] CEF initialized, capturing...')

      addon.capture({
        url: 'https://example.com',
        width: 1280,
        height: 720,
        output
      })

      console.log('[demo] Screenshot saved to:', output)
      process.exit(0)
    } catch (err) {
      console.error('[demo] Error:', err)
      process.exit(1)
    }
  })()

process.on('uncaughtException', (err) => {
  console.error('[demo] Uncaught Exception:', err)
  process.exit(1)
})

process.on('unhandledRejection', (reason) => {
  console.error('[demo] Unhandled Rejection:', reason)
  process.exit(1)
})
