cmake_minimum_required(VERSION 3.10)

project(bench)
set(CMAKE_CXX_STANDARD 14)

set(
        _CLI_OPTIONS
        CLI_options.h CLI_options.cc
)
# add third party directories
include_directories(../../thirdparty)
add_executable(CLI main.cc options.h ${_CLI_OPTIONS})