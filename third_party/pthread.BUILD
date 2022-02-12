load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "pthreads_win32",
    srcs = ["pthread.c"],
    hdrs = glob(["*.h"]),
    defines = [
        "HAVE_STRUCT_TIMESPEC",
        "PTW32_STATIC_LIB",
    ],
    local_defines = ["PTW32_BUILD"],
    textual_hdrs = glob(
        include = ["*.c"],
        exclude = ["pthread.c"],
    ),
)

cc_library(
    name = "pthreads_posix",
    linkopts = ["-lpthread"],
)

cc_library(
    name = "pthread",
    visibility = ["//visibility:public"],
    deps = select({
        "@platforms//os:linux": [":pthreads_posix"],
        "@platforms//os:windows": [":pthreads_win32"],
    }),
)
