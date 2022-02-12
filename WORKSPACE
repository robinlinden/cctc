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

http_archive(
    name = "libsodium",  # ISC
    build_file = "//third_party:libsodium.BUILD",
    patch_cmds = ["cp builds/msvc/version.h src/libsodium/include/sodium/"],
    sha256 = "d59323c6b712a1519a5daf710b68f5e7fde57040845ffec53850911f10a5d4f4",
    strip_prefix = "libsodium-1.0.18",
    url = "https://github.com/jedisct1/libsodium/archive/1.0.18.tar.gz",
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
    sha256 = "577e23fe52f8be6739a9fffb2b16bfefd3a0ef4994d0714cb28a1ecca3669ca6",
    patch_cmds = [
        "rm toxcore/BUILD.bazel",
        "rm toxencryptsave/BUILD.bazel",
    ],
    strip_prefix = "c-toxcore-0.2.15",
    url = "https://github.com/TokTok/c-toxcore/archive/v0.2.15.tar.gz",
)
