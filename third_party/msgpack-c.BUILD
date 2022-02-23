load("@rules_cc//cc:defs.bzl", "cc_library")

genrule(
    name = "generate_sysdep",
    srcs = ["cmake/sysdep.h.in"],
    outs = ["include/msgpack/sysdep.h"],
    cmd = "cp $< $@",
)

genrule(
    name = "generate_pack_template",
    srcs = ["cmake/pack_template.h.in"],
    outs = ["include/msgpack/pack_template.h"],
    cmd = "cp $< $@",
)

cc_library(
    name = "msgpack-c",
    srcs = glob(["src/*.c"]),
    hdrs = glob(["include/**/*.h"]) + [
        ":generate_pack_template",
        ":generate_sysdep",
    ],
    defines = [
        "MSGPACK_ENDIAN_LITTLE_BYTE=1",
        "MSGPACK_ENDIAN_BIG_BYTE=0",
    ],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
