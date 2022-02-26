load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Bazel
# =========================================================

http_archive(
    name = "platforms",  # Apache-2.0
    sha256 = "079945598e4b6cc075846f7fd6a9d0857c33a7afc0de868c2ccb96405225135d",
    url = "https://github.com/bazelbuild/platforms/releases/download/0.0.4/platforms-0.0.4.tar.gz",
)

# Third-party
# =========================================================

# HEAD as of 2022-02-26
http_archive(
    name = "hastur",  # BSD-2-Clause
    sha256 = "b81ae14d9862476c2da799984529b6dd9c6311b0d8f176d047c7149a9195d38c",
    strip_prefix = "hastur-25c528e1c9cf22ff56cbaf5a686d7cfa43a94313",
    url = "https://github.com/robinlinden/hastur/archive/25c528e1c9cf22ff56cbaf5a686d7cfa43a94313.tar.gz",
)

http_archive(
    name = "libsodium",  # ISC
    build_file = "//third_party:libsodium.BUILD",
    patch_cmds = ["cp builds/msvc/version.h src/libsodium/include/sodium/"],
    sha256 = "d59323c6b712a1519a5daf710b68f5e7fde57040845ffec53850911f10a5d4f4",
    strip_prefix = "libsodium-1.0.18",
    url = "https://github.com/jedisct1/libsodium/archive/1.0.18.tar.gz",
)

http_archive(
    name = "msgpack-c",  # BSL-1.0
    build_file = "//third_party:msgpack-c.BUILD",
    sha256 = "420fe35e7572f2a168d17e660ef981a589c9cbe77faa25eb34a520e1fcc032c8",
    strip_prefix = "msgpack-c-4.0.0",
    url = "https://github.com/msgpack/msgpack-c/releases/download/c-4.0.0/msgpack-c-4.0.0.tar.gz",
)

http_archive(
    name = "pthread",  # LGPL-2.1-only
    build_file = "//third_party:pthread.BUILD",
    sha256 = "e6aca7aea8de33d9c8580bcb3a0ea3ec0a7ace4ba3f4e263ac7c7b66bc95fb4d",
    strip_prefix = "pthreads-w32-2-9-1-release",
    url = "https://sourceware.org/pub/pthreads-win32/pthreads-w32-2-9-1-release.tar.gz",
)

http_archive(
    name = "toxcore",  # GPL-3.0-or-later
    build_file = "//third_party:toxcore.BUILD",
    patch_cmds = [
        "rm toxcore/BUILD.bazel",
        "rm toxencryptsave/BUILD.bazel",
    ],
    sha256 = "653aa42654b607f0940cecfac873e9ce55605119a90d1dc454d1090ff6ca29c0",
    strip_prefix = "c-toxcore-0.2.16",
    url = "https://github.com/TokTok/c-toxcore/archive/v0.2.16.tar.gz",
)
