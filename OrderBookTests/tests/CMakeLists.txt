cmake_minimum_required(VERSION 3.5)

include_directories(${gtest_SOURCE_DIR}/include $gtest_SOURCE_DIR})

add_executable(RunTests OrderBookTests.cpp)

target_link_libraries(RunTests gtest gtest_main OrderBook)