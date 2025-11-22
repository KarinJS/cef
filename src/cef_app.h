#pragma once

#include <string>

namespace cef_screenshot {

bool InitializeCef(const std::wstring& cef_root, const std::wstring& cache_path);
void ShutdownCef();
bool IsCefInitialized();

}  // namespace cef_screenshot
