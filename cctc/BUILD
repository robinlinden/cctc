load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "cctc",
    srcs = ["tox.cc"],
    hdrs = ["tox.h"],
    deps = ["@toxcore"],
)

cc_test(
    name = "cctc_test",
    size = "small",
    srcs = ["tox_test.cc"],
    deps = [
        ":cctc",
        "@hastur//etest",
    ],
)
