cmake_minimum_required(VERSION 3.16)
project(logger)

set(CMAKE_CXX_STANDARD 20)

include_directories(deps/spdlog-1.9.2/include)
add_subdirectory(deps)

aux_source_directory(. SOURCE)
add_executable(${PROJECT_NAME} ${SOURCE})