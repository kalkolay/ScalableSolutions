cmake_minimum_required(VERSION 3.5)
project(ScalableSolutions VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(OrderBook)

add_subdirectory(OrderBook)
add_subdirectory(OrderBookTests)