cmake_minimum_required(VERSION 3.5)
project(ScalableSolutions)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost!")
endif()

include_directories(OrderBook)

add_subdirectory(OrderBook)
add_subdirectory(OrderBookTests)