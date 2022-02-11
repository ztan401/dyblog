cmake_minimum_required(VERSION 3.21)
project(nanotime)

set(CMAKE_CXX_STANDARD 23)

include_directories(deps/fmt/include)
add_subdirectory(deps/fmt)

aux_source_directory(. SOURCE)
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
