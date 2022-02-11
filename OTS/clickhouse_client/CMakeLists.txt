cmake_minimum_required(VERSION 3.21)
project(clickhouse_client)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)
include_directories(include/contrib)

add_executable(${PROJECT_NAME} main.cpp)
target_link_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/lib")
target_link_libraries(${PROJECT_NAME} clickhouse-cpp-lib)


IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # there is a problem with __builtin_mul_overflow call at link time
    # the error looks like: ... undefined reference to `__muloti4' ...
    # caused by clang bug https://bugs.llvm.org/show_bug.cgi?id=16404
    # explicit linking to compiler-rt allows to workaround the problem
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --rtlib=compiler-rt")
ENDIF ()
