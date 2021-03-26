{
    "targets": [
        {
            "default_configuration": "Debug",
            "target_name": "node-dsr",
            "sources": ["src/addon.cpp"],
            "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<(module_root_dir)/include",
            ],
            "libraries": [
                "-lpthread",
                "<(module_root_dir)/lib/libDRFL.a",
                "-lPocoFoundation",
                "-lPocoNet",
            ],
            "cflags!": ["-fno-exceptions"],
            "cflags": ["-std=c++11", "-Wall"],
            "cflags_cc!": ["-fno-exceptions"],
        }
    ]
}