{
    "variables": {
        "platform": "<(OS)",
    },
    "targets": [
        {
            "default_configuration": "Release",
            "target_name": "node-dsr",
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
                        "sources": ["src/addon.cpp", "src/NodeDsr.cpp"],
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
                        "sources": ["src/addon.cpp", "src/NodeDsrNoLibWin.cpp"],
                    },
                ],
                [
                    'OS=="mac"',
                    {
                        "sources": ["src/addon.cpp", "src/NodeDsrNoLib.cpp"],
                    },
                ],
            ],
        },
    ],
}