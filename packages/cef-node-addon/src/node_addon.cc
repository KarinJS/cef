#include <napi.h>

#include <codecvt>
#include <locale>
#include <string>

#include "cef_app.h"
#include "screenshot_handler.h"

namespace {

std::wstring Utf8ToWide(const std::string& input) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  return conv.from_bytes(input);
}

std::string WideToUtf8(const std::wstring& input) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
  return conv.to_bytes(input);
}

}  // namespace

Napi::Value Init(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "init(options) expected an object").ThrowAsJavaScriptException();
    return env.Null();
  }

  auto obj = info[0].As<Napi::Object>();
  std::string cef_root = obj.Get("cefRoot").As<Napi::String>().Utf8Value();
  std::string cache_path;
  if (obj.Has("cachePath")) {
    cache_path = obj.Get("cachePath").As<Napi::String>().Utf8Value();
  }

  bool ok = cef_screenshot::InitializeCef(Utf8ToWide(cef_root), Utf8ToWide(cache_path));
  if (!ok) {
    Napi::Error::New(env, "Failed to initialize CEF").ThrowAsJavaScriptException();
  }
  return Napi::Boolean::New(env, ok);
}

Napi::Value Capture(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "capture(options) expected an object").ThrowAsJavaScriptException();
    return env.Null();
  }

  auto obj = info[0].As<Napi::Object>();
  cef_screenshot::CaptureRequest request;
  request.url = obj.Get("url").As<Napi::String>().Utf8Value();
  request.width = obj.Has("width") ? obj.Get("width").As<Napi::Number>().Int32Value() : 1280;
  request.height = obj.Has("height") ? obj.Get("height").As<Napi::Number>().Int32Value() : 720;
  request.output_path = Utf8ToWide(obj.Get("output").As<Napi::String>().Utf8Value());
  if (obj.Has("timeoutMs")) {
    request.timeout_ms = obj.Get("timeoutMs").As<Napi::Number>().Int32Value();
  }

  cef_screenshot::CaptureResult result = cef_screenshot::CapturePage(request);
  if (!result.success) {
    Napi::Error::New(env, result.error).ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::String::New(env, WideToUtf8(result.output_path));
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, Init));
  exports.Set(Napi::String::New(env, "capture"), Napi::Function::New(env, Capture));
  return exports;
}

NODE_API_MODULE(cefaddon, InitAll)
