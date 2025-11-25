cmake_minimum_required(VERSION 3.20)
project(cef_screenshot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT DEFINED ENV{CEF_ROOT})
  message(FATAL_ERROR "Please set CEF_ROOT to the CEF binary root (contains include/ and Release/libcef.lib)")
endif()

set(CEF_ROOT $ENV{CEF_ROOT})
message(STATUS "Using CEF from ${CEF_ROOT}")

add_library(cef_screenshot SHARED
  src/cef_app.cc
  src/screenshot_handler.cc
)

target_include_directories(cef_screenshot PUBLIC
  ${CEF_ROOT}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cef_screenshot PRIVATE
  ${CEF_ROOT}/Release/libcef.lib
  version.lib
  user32.lib
  gdi32.lib
  advapi32.lib
  ole32.lib
  shell32.lib
  winmm.lib
)

target_compile_definitions(cef_screenshot PRIVATE
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  CEF_USE_SANDBOX=0
)
