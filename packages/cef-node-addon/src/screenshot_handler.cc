#include "screenshot_handler.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

#include "cef_app.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_image.h"
#include "include/cef_render_handler.h"
#include "include/cef_request_context.h"

namespace cef_screenshot {
namespace {

class ScreenshotClient : public CefClient,
                         public CefLifeSpanHandler,
                         public CefLoadHandler,
                         public CefRenderHandler {
 public:
  ScreenshotClient(int width, int height)
      : width_(width), height_(height) {}

  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
    return this;
  }
  CefRefPtr<CefLoadHandler> GetLoadHandler() override {
    return this;
  }
  CefRefPtr<CefRenderHandler> GetRenderHandler() override {
    return this;
  }

  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
    browser_ = browser;
    browser_->GetHost()->WasResized();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    load_finished_.store(true);
    browser->GetHost()->Invalidate(PET_VIEW);
  }

  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
    rect = CefRect(0, 0, width_, height_);
  }

  void OnPaint(CefRefPtr<CefBrowser> browser,
               PaintElementType type,
               const RectList& dirtyRects,
               const void* buffer,
               int width,
               int height) override {
    if (type != PET_VIEW) {
      return;
    }
    if (!load_finished_.load() || captured_.load()) {
      return;
    }

    CefRefPtr<CefImage> image = CefImage::CreateImage();
    const size_t pixel_size = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    image->AddBitmap(1.0f, width, height, CEF_COLOR_TYPE_BGRA_8888,
                     CEF_ALPHA_TYPE_PREMULTIPLIED, buffer, pixel_size);
    int pixel_width = 0;
    int pixel_height = 0;
    CefRefPtr<CefBinaryValue> png =
        image->GetAsPNG(1.0f, true, pixel_width, pixel_height);
    if (png && png->GetSize() > 0) {
      std::vector<uint8_t> data(png->GetSize());
      png->GetData(data.data(), data.size(), 0);
      {
        std::lock_guard<std::mutex> lock(data_lock_);
        png_bytes_ = std::move(data);
      }
      captured_.store(true);
    }
  }

  void CloseBrowser() {
    if (browser_) {
      browser_->GetHost()->CloseBrowser(true);
      browser_ = nullptr;
    }
  }

  bool IsComplete() const {
    return load_finished_.load() && captured_.load();
  }

  std::vector<uint8_t> TakePng() {
    std::lock_guard<std::mutex> lock(data_lock_);
    return png_bytes_;
  }

 private:
  CefRefPtr<CefBrowser> browser_;
  int width_ = 0;
  int height_ = 0;
  std::atomic<bool> load_finished_{false};
  std::atomic<bool> captured_{false};
  std::mutex data_lock_;
  std::vector<uint8_t> png_bytes_;

  IMPLEMENT_REFCOUNTING(ScreenshotClient);
};

}  // namespace

CaptureResult CapturePage(const CaptureRequest& request) {
  CaptureResult result;
  if (!IsCefInitialized()) {
    result.error = "CEF not initialized. Call init() first.";
    return result;
  }

  CefWindowInfo window_info;
  window_info.SetAsWindowless(nullptr);

  CefBrowserSettings browser_settings;
  browser_settings.windowless_frame_rate = 30;

  CefRefPtr<ScreenshotClient> client =
      new ScreenshotClient(request.width, request.height);
  bool created = CefBrowserHost::CreateBrowser(
      window_info, client, request.url, browser_settings, nullptr,
      CefRequestContext::GetGlobalContext());
  if (!created) {
    result.error = "Failed to create CEF browser.";
    return result;
  }

  const auto deadline = std::chrono::steady_clock::now() +
                        std::chrono::milliseconds(request.timeout_ms);
  while (std::chrono::steady_clock::now() < deadline) {
    CefDoMessageLoopWork();
    if (client->IsComplete()) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  if (!client->IsComplete()) {
    result.error = "Timed out waiting for page paint.";
    client->CloseBrowser();
    return result;
  }

  std::vector<uint8_t> png = client->TakePng();
  client->CloseBrowser();

  if (png.empty()) {
    result.error = "No PNG data captured.";
    return result;
  }

  std::ofstream out(request.output_path, std::ios::binary);
  if (!out.is_open()) {
    result.error = "Failed to open output path.";
    return result;
  }
  out.write(reinterpret_cast<const char*>(png.data()),
            static_cast<std::streamsize>(png.size()));
  out.close();

  result.success = out.good();
  if (!result.success) {
    result.error = "Failed to write PNG data.";
  } else {
    result.output_path = request.output_path;
  }
  return result;
}

}  // namespace cef_screenshot
