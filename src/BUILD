load("//:cc_config.bzl", "CC_OPTS")

cc_library(
    name = "board",
    srcs = ["board.cc"],
    hdrs = ["board.hh"],
)

cc_library(
    name = "hash",
    hdrs = ["hash.h"],
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

cc_library(
    name = "player_board",
    srcs = ["player_board.cc"],
    hdrs = ["player_board.hh"],
    deps = [
        "pieces",
        "hash",
    ],
)

cc_test(
    name = "player_board_test",
    srcs = ["player_board_test.cc"],
    copts = CC_OPTS,
    deps = [
        "player_board",
        "@googletest//:gtest_main",
    ],
)
