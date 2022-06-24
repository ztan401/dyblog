cmake_minimum_required(VERSION 3.16)
project(shmkv)

set(CMAKE_CXX_STANDARD 20)
find_package(spdlog)

aux_source_directory(. SOURCE)
add_executable(${PROJECT_NAME} ${SOURCE})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

