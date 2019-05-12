cmake_minimum_required(VERSION 3.10)

project(bench)
set(CMAKE_CXX_STANDARD 14)

set(
        _COMMAND_OPTIONS
        command_options.h command_options.cc
)
set(
        _WEB_SERVER
        web_server.h web_server.cc
)
set(
        _UTILS
        id_generator.hpp
)
# add third party directories
include_directories(../../thirdparty)

add_executable(CLI main.cc options.h ${_COMMAND_OPTIONS} ${_WEB_SERVER} ${_UTILS})
target_link_libraries(CLI boost_system)