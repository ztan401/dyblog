cmake_minimum_required(VERSION 3.16)
project(ctpfuture_client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE "Release")

include_directories(ctp/include)
link_directories(ctp/lib)
set(LD_LIBRARY_PATH /ctp/lib)

find_package(pybind11)
include_directories(deps/spdlog/include)
add_subdirectory(deps)

aux_source_directory(. SOURCES)
pybind11_add_module(${PROJECT_NAME} ${SOURCES})

# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH $ORIGIN)

target_link_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/ctp/lib")
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
target_link_libraries(${PROJECT_NAME} PUBLIC thostmduserapi_se.so thosttraderapi_se.so)

FILE(COPY ctp/lib/libthostmduserapi_se.so DESTINATION ${CMAKE_BINARY_DIR})
FILE(COPY ctp/lib/libthosttraderapi_se.so DESTINATION ${CMAKE_BINARY_DIR})
FILE(COPY test.py DESTINATION ${CMAKE_BINARY_DIR})
message(${CMAKE_BINARY_DIR})