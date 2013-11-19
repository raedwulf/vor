cmake_minimum_required(VERSION 2.8)
project(vor C)

set(CMAKE_C_FLAGS "-std=c11")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

include_directories(include)

set(VOR_HEADERS include/acoder.h include/pd_order0.h)

add_executable(acoder_test tests/acoder_test.c ${VOR_HEADERS})
add_executable(vor src/vor.c ${VOR_HEADERS})

target_link_libraries(acoder_test m)
target_link_libraries(vor m)
