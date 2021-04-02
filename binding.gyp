{
    "variables": {
        "platform": "<(OS)",
    },
    "targets": [
        {
            "default_configuration": "Debug",
            "target_name": "node-dsr",
            "sources": ["src/addon.cpp", "src/NodeDsr.cpp"],
            "cflags!": ["-fno-exceptions"],
            "cflags": ["-std=c++11", "-Wall"],
            "cflags_cc!": ["-fno-exceptions"],
            "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<(module_root_dir)/include",
            ],
            "conditions": [
                [
                    'OS=="linux"',
                    {
                        "libraries": [
                            "<(module_root_dir)/lib/linux/libDRFL.a",
                            "-lPocoFoundation",
                            "-lPocoNet",
                        ],
                    },
                ],
                [
                    'OS=="win"',
                    {
                        "libraries": [
                            "<(module_root_dir)/lib/win/DRFLWin64.lib",
                            "<(module_root_dir)/lib/win/PocoFoundation.lib",
                            "<(module_root_dir)/lib/win/PocoNet.lib",
                        ],
                    },
                ],
            ],
        },
    ],
}