load("@rules_cc//cc:defs.bzl", "cc_library")

# toxcore
# =========================================================

cc_library(
    name = "ccompat",
    hdrs = ["toxcore/ccompat.h"],
)

cc_library(
    name = "toxcore",
    srcs = glob([
        "toxcore/*.c",
        "toxcore/*.h",
    ]),
    hdrs = ["toxcore/tox.h"],
    include_prefix = "tox",
    linkopts = select({
        "@platforms//os:linux": [],
        "@platforms//os:windows": [
            "-DEFAULTLIB:Iphlpapi",
            "-DEFAULTLIB:Ws2_32",
        ],
    }),
    local_defines = select({
        "@platforms//os:linux": ["TCP_SERVER_USE_EPOLL=1"],
        "@platforms//os:windows": [],
    }),
    strip_include_prefix = "toxcore",
    visibility = ["//visibility:public"],
    deps = [
        ":toxencryptsave_defines",
        "@libsodium",
        "@pthread",
    ],
)

# toxencryptsave
# =========================================================

cc_library(
    name = "toxencryptsave_defines",
    hdrs = ["toxencryptsave/defines.h"],
)

cc_library(
    name = "toxencryptsave",
    srcs = ["toxencryptsave/toxencryptsave.c"],
    hdrs = ["toxencryptsave/toxencryptsave.h"],
    include_prefix = "tox",
    strip_include_prefix = "toxencryptsave",
    visibility = ["//visibility:public"],
    deps = [
        ":ccompat",
        ":crypto_core",
        ":toxencryptsave_defines",
    ],
)
