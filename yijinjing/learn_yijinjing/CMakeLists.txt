cmake_minimum_required(VERSION 3.22)
project(learn_mini_yijinjing)

set(CMAKE_CXX_STANDARD 20)

include_directories(deps/spdlog/include)
add_subdirectory(deps/spdlog)
include_directories(include)

aux_source_directory(src/paged PAGED)
aux_source_directory(src/journal JOURNAL)
aux_source_directory(. SOURCES)

add_executable(${PROJECT_NAME} ${PAGED} ${JOURNAL} ${SOURCES})


