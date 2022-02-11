cmake_minimum_required(VERSION 3.21)
project(pynanotime)
set(CMAKE_CXX_STANDARD 23)

find_package(pybind11)
include_directories(deps/fmt/include)
add_subdirectory(deps/fmt)

aux_source_directory(. SOURCE)
pybind11_add_module(${PROJECT_NAME} SHARED ${SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif ()

