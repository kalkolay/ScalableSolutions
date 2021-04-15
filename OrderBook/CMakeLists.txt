cmake_minimum_required(VERSION 3.5)
project(OrderBook)

set(CMAKE_CXX_STANDARD 14)

set(HEADER_FILES NotFoundException.h Order.h OrderBook.h)
set(SOURCE_FILES Order.cpp OrderBook.cpp)

add_library(OrderBook STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(OrderBook PRIVATE ${Boost_INCLUDE_DIR})