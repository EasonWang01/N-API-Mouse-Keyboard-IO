{
  "targets": [
    {
      "target_name": "io_listener",
      "sources": ["lib/io_listener.cpp"],
      "include_dirs": [
        "node_modules/node-addon-api"
      ],
      "conditions": [
        ["OS=='mac'", {
          "xcode_settings": {
            "OTHER_LDFLAGS": [
              "-framework", "ApplicationServices",
              "-framework", "CoreFoundation"
            ]
          }
        }],
        ["OS=='win'", {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "AdditionalOptions": [
                "/EHsc"
              ]
            }
          },
          "libraries": [
            "-lkernel32",
            "-luser32"
          ]
        }]
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"]
    }
  ]
}
