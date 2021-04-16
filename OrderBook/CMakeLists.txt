cmake_minimum_required(VERSION 3.5)
project(OrderBook)

set(HEADER_FILES NotFoundException.h Order.h OrderBook.h)
set(SOURCE_FILES Order.cpp OrderBook.cpp)

add_library(OrderBook STATIC ${SOURCE_FILES} ${HEADER_FILES})