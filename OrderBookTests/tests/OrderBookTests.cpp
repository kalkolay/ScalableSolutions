#include <gtest/gtest.h>
#include <OrderBook.h>

TEST(OrderBookTests, OrderBookEmptyByDefault)  // NOLINT
{
    OrderBook book;
    EXPECT_TRUE( book.isEmpty() );
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}