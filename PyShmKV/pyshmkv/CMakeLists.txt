if (APPLE)
    set(PYTHON_EXECUTABLE /usr/local/Caskroom/miniconda/base/bin/python)
endif ()
set(CMAKE_BUILD_TYPE "Release")

cmake_minimum_required(VERSION 3.16)
project(pyshmkv)

set(CMAKE_CXX_STANDARD 20)
set(PYTHON_EXECUTABLE /usr/local/Caskroom/miniconda/base/bin/python)

find_package(spdlog)
find_package(pybind11)

aux_source_directory(. SOURCE)
pybind11_add_module(${PROJECT_NAME} SHARED ${SOURCE})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

#if (NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
#endif ()

