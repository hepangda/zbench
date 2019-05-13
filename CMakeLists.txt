cmake_minimum_required(VERSION 3.14)
project(bishe)

link_libraries(pthread)
link_libraries(crypto)
link_libraries(ssl)
link_libraries(boost_system)

include_directories(3rdparty)
set(CMAKE_CXX_STANDARD 14)

add_executable(bishe main.cc src/client/https_session.cc src/client/https_session.h src/utils/net.h src/core/benchmarker.cc src/core/benchmarker.h src/utils/read_buffer_adapter.h src/utils/write_buffer_adapter.h src/client/client_report.h src/client/client_scheduler.h src/client/http_session.cc src/client/http_session.h src/utils/http_request.cc src/utils/http_request.h src/utils/linux_file.h src/utils/linux_process.h src/utils/linux_temp_file.h src/utils/process_info.cc src/utils/process_info.h src/cli/cli_buffer.h src/utils/id_generator.h src/cli/command_option.cc src/cli/command_option.h src/cli/option.h src/cli/optfwd.h src/cli/web_server.cc src/cli/web_server.h)