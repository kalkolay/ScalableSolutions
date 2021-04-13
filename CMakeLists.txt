cmake_minimum_required(VERSION 3.5)
project(ScalableSolutions)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(ScalableSolutions ${SOURCE_FILES})

include_directories(OrderBook)