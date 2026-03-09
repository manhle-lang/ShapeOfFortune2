{
  'variables': {
    'openssl_root%': '<!(echo ${OPENSSL_ROOT:-$(brew --prefix openssl@3)})',
  },
  'targets': [
    {
      'target_name': 'shape_of_fortune',
      'sources': [
          'src/math/native/math/Math.cpp',
          'src/math/native/math/GameConfig.cpp',
          'src/math/native/wrap/Wrapper.cpp',
          'src/math/libs/CommonLibrary.cpp',
          'src/math/libs/Converter.cpp',
          'src/math/libs/RgRNG.cpp',
      ],
      "cflags": ["-static-libgcc", "-static-libstdc++", "-std=c++20","-O2", "-Wall", "-Wextra", "-Wno-unknown-pragmas", "-fexceptions"],
      "ldflags": ["-static-libgcc", "-static-libstdc++", "-fexceptions"],
      "cflags_cc": ["-static-libgcc", "-static-libstdc++", "-std=c++20", "-fexceptions"],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
      },
      "include_dirs": [
        "src/math/libs",
        "src/math/native/math",
        "src/math/native/wrap",

        "<(openssl_root)/include", # Path to OpenSSL headers
      ],
      "libraries": [
        # Link against OpenSSL libraries
        "-L<(openssl_root)/lib",
        "-lssl",
        "-lcrypto",
      ]
    }
  ],
}