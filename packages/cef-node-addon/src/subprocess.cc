#include <Windows.h>

#include "include/cef_app.h"
#include "include/cef_sandbox_win.h"

// Minimal CEF subprocess entry point. This executable is used for renderer/GPU
///utility processes so that we don't launch node.exe with CEF's switches.
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, wchar_t*, int) {
  CefMainArgs main_args(hInstance);
  return CefExecuteProcess(main_args, nullptr, nullptr);
}
