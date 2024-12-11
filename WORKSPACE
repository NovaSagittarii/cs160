workspace(
    name = "tetrsolver",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "parlaylib",
    build_file = "@//externals:BUILD.parlaylib.bazel",
    commit = "36459f42a84207330eae706c47e6fab712e6a149",
    recursive_init_submodules = True,
    remote = "git@github.com:cmuparlay/parlaylib.git",
)
