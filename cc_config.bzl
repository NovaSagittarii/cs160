"""
CC Compiler options to be used for all project files.
"""

CC_OPTS = select({
    "@bazel_tools//src/conditions:windows": ["/std:c++20"],
    "//conditions:default": ["-std=c++20"],
})

CC_FAST_OPTS = select({
    "@bazel_tools//src/conditions:windows": [],
    "//conditions:default": ["-O2"], #["-O3", "-march=native", "-mtune=native"],
})
