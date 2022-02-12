load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "libsodium",
    srcs = glob([
        "src/libsodium/**/*.c",
        "src/libsodium/**/*.h",
    ]),
    hdrs = glob(["src/libsodium/include/**/*.h"]),
    copts = ["-Iexternal/libsodium/src/libsodium/include/sodium/"],
    defines = ["SODIUM_STATIC"],
    local_defines = ["CONFIGURED"],
    strip_include_prefix = "src/libsodium/include",
    visibility = ["//visibility:public"],
)
