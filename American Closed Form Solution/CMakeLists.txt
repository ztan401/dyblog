cmake_minimum_required(VERSION 3.19)
project(am_option_test)

set(CMAKE_CXX_STANDARD 20)

add_executable(am_option_test am_option_test.cpp bsm.h bsm.cpp norm_dist.h bi_norm_dist.h bi_norm_dist.cpp baw.cpp baw.h bjerk2002.h bjerk2002.cpp bjerk1993.cpp bjerk1993.h crr.h crr.cpp tabulate.hpp juzhong.h juzhong.cpp)