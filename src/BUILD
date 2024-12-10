load("//:cc_config.bzl", "CC_OPTS")

cc_library(
    name = "board",
    srcs = ["board.cc"],
    hdrs = ["board.hh"],
)

cc_library(
    name = "pieces",
    hdrs = ["pieces.hh"],
    deps = ["board"],
)

cc_test(
    name = "pieces_test",
    srcs = ["pieces_test.cc"],
    copts = CC_OPTS,
    deps = [
        "pieces",
        "@googletest//:gtest_main",
    ],
)
