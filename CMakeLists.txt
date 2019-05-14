cmake_minimum_required(VERSION 3.14)
project(bishe)

link_libraries(pthread)
link_libraries(crypto)
link_libraries(ssl)
link_libraries(boost_system)

aux_source_directory(src/cli SOURCE_CLI)
aux_source_directory(src/client SOURCE_CLIENT)
aux_source_directory(src/core SOURCE_CORE)
aux_source_directory(src/utils SOURCE_UTILS)
include_directories(3rdparty)
set(CMAKE_CXX_STANDARD 14)

add_executable(bishe
        main.cc
        ${SOURCE_CLI}
        ${SOURCE_CLIENT}
        ${SOURCE_CORE}
        ${SOURCE_UTILS}
        )