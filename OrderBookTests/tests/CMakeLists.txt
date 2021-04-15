cmake_minimum_required(VERSION 3.5)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(RunTests OrderBookTests.cpp OrderBookInfoTests.cpp TestBook.cpp TestBook.h)

target_link_libraries(RunTests gtest gtest_main OrderBook)

target_include_directories(RunTests PRIVATE ${Boost_INCLUDE_DIR})