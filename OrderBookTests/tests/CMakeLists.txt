cmake_minimum_required(VERSION 3.5)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(SOURCE_FILES OrderBookTests.cpp OrderBookInfoTests.cpp TestBook.cpp TestBook.h)
add_executable(RunTests ${SOURCE_FILES})

target_link_libraries(RunTests gtest gtest_main OrderBook)