#include <gtest/gtest.h>
#include <OrderBook.h>

TEST(OrderBookTests, EmptyTest)  // NOLINT
{
    OrderBook orderBook;
    SUCCEED();
}



/**
 *  @brief Run all tests
 */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}