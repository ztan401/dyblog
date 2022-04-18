cmake_minimum_required(VERSION 3.22)
project(yijinjing)

set(CMAKE_CXX_STANDARD 20)

include_directories(deps/spdlog/include)
add_subdirectory(deps/spdlog)
include_directories(yijinjing/include)

aux_source_directory(yijinjing/paged PAGED)
aux_source_directory(yijinjing/journal JOURNAL)
aux_source_directory(. SOURCES)

add_executable(${PROJECT_NAME} ${PAGED} ${JOURNAL} ${SOURCES})