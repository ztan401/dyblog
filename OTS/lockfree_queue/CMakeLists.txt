cmake_minimum_required(VERSION 3.21)
project(lockfree_queue)

set(CMAKE_CXX_STANDARD 23)

add_executable(lockfree_queue main.cpp atomicops.h readerwriterqueue.h)
