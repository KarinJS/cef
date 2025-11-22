#include "cef_app.h"

#include <Windows.h>
#include <string>

#include "include/cef_app.h"
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"

namespace cef_screenshot {
namespace {
bool g_initialized = false;
}

bool InitializeCef(const std::wstring& cef_root, const std::wstring& cache_path) {
  if (g_initialized) {
    return true;
  }

  HINSTANCE hInstance = ::GetModuleHandleW(nullptr);
  CefMainArgs main_args(hInstance);

  // If this is a sub-process, execute and return the result immediately.
  int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
  if (exit_code >= 0) {
    return false;
  }

  CefSettings settings;
  settings.no_sandbox = true;
  settings.windowless_rendering_enabled = true;
  settings.command_line_args_disabled = true;  // 忽略 node 自带命令行参数
  settings.log_severity = LOGSEVERITY_INFO;

  wchar_t cwd[MAX_PATH] = {0};
  if (::GetCurrentDirectoryW(MAX_PATH, cwd)) {
    CefString(&settings.log_file) = std::wstring(cwd) + L"\\cef_debug.log";
    CefString(&settings.browser_subprocess_path) =
        std::wstring(cwd) + L"\\dist\\cef_subprocess.exe";
  }

  CefString(&settings.resources_dir_path) = cef_root;
  CefString(&settings.locales_dir_path) = cef_root + L"\\locales";
  CefString(&settings.locale) = "en-US";
  if (!cache_path.empty()) {
    CefString(&settings.cache_path) = cache_path;
  }

  g_initialized = CefInitialize(main_args, settings, nullptr, nullptr);
  return g_initialized;
}

void ShutdownCef() {
  if (!g_initialized) {
    return;
  }
  CefShutdown();
  g_initialized = false;
}

bool IsCefInitialized() {
  return g_initialized;
}

}  // namespace cef_screenshot
