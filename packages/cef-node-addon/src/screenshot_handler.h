#pragma once

#include <string>

namespace cef_screenshot {

struct CaptureRequest {
  std::string url;
  int width = 1280;
  int height = 720;
  std::wstring output_path;
  int timeout_ms = 15000;
};

struct CaptureResult {
  bool success = false;
  std::string error;
  std::wstring output_path;
};

CaptureResult CapturePage(const CaptureRequest& request);

}  // namespace cef_screenshot
