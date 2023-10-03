cmake_minimum_required(VERSION 3.15)
project(warping-cache-simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(src)
add_subdirectory(src)
