{
  "targets": [
    {
      "target_name": "cefaddon",
      "sources": [
        "src/node_addon.cc",
        "src/cef_app.cc",
        "src/screenshot_handler.cc"
      ],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include\")",
        "node_modules/node-addon-api",
        "src",
        "<(CEF_ROOT)"
      ],
      "defines": [
        "NAPI_CPP_EXCEPTIONS",
        "WIN32_LEAN_AND_MEAN",
        "NOMINMAX",
        "CEF_USE_SANDBOX=0"
      ],
      "conditions": [
        ["OS=='win'", {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "AdditionalOptions": [ "/std:c++17" ]
            }
          },
          "dependencies": [ "cef_subprocess" ],
          "libraries": [
            "<(CEF_ROOT)/Release/libcef.lib",
            "<(CEF_ROOT)/Release/libcef_dll_wrapper.lib",
            "version.lib",
            "user32.lib",
            "gdi32.lib",
            "advapi32.lib",
            "ole32.lib",
            "shell32.lib",
            "winmm.lib"
          ],
          "copies": [
            {
              "destination": "<(PRODUCT_DIR)",
              "files": [
                "<(CEF_ROOT)/Release/libcef.dll",
                "<(CEF_ROOT)/Resources/icudtl.dat",
                "<(CEF_ROOT)/Resources/resources.pak",
                "<(CEF_ROOT)/Resources/chrome_100_percent.pak",
                "<(CEF_ROOT)/Resources/chrome_200_percent.pak",
                "<(CEF_ROOT)/Release/v8_context_snapshot.bin",
                "<(CEF_ROOT)/Release/libEGL.dll",
                "<(CEF_ROOT)/Release/libGLESv2.dll",
                "<(CEF_ROOT)/Release/vk_swiftshader.dll",
                "<(CEF_ROOT)/Release/vk_swiftshader_icd.json",
                "<(CEF_ROOT)/Release/vulkan-1.dll",
                "<(CEF_ROOT)/Release/d3dcompiler_47.dll",
                "<(CEF_ROOT)/Release/dxcompiler.dll",
                "<(CEF_ROOT)/Release/dxil.dll",
                "<(CEF_ROOT)/Release/chrome_elf.dll"
              ]
            },
            {
              "destination": "<(PRODUCT_DIR)/locales",
              "files": [
                "<(CEF_ROOT)/Resources/locales/en-US.pak"
              ]
            }
          ]
        }]
      ]
    },
    {
      "target_name": "cef_subprocess",
      "type": "executable",
      "sources": [
        "src/subprocess.cc"
      ],
      "include_dirs": [
        "<(CEF_ROOT)"
      ],
      "conditions": [
        ["OS=='win'", {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "AdditionalOptions": [ "/std:c++17" ]
            },
            "VCLinkerTool": {
              "SubSystem": "2",  # Windows
              "EntryPointSymbol": "wWinMainCRTStartup"
            }
          },
          "libraries": [
            "<(CEF_ROOT)/Release/libcef.lib",
            "<(CEF_ROOT)/Release/libcef_dll_wrapper.lib",
            "user32.lib",
            "gdi32.lib",
            "advapi32.lib",
            "ole32.lib",
            "shell32.lib",
            "winmm.lib"
          ]
        }]
      ]
    }
  ]
}
