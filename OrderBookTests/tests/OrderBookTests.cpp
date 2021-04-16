#include <gtest/gtest.h>
#include <array>

#include "TestBook.h"

TEST(OrderBookTests, EmptyTest)  // NOLINT
{
    OrderBook orderBook;
    SUCCEED();
}

TEST(OrderBookTests, OrderAdd)  // NOLINT
{
    const Order::PriceType price = 1000;
    const Order::QuantityType quantity = 100;
    const std::array<Order::Type, 2> allOrderTypes = {Order::Type::Bid, Order::Type::Ask};
    for (auto orderType : allOrderTypes)
    {
        OrderBook orderBook;
        auto id = orderBook.addOrder(orderType, 1000, 100);
        auto order = orderBook.getOrderById(id);
        ASSERT_EQ( price,     order.getPrice()    );
        ASSERT_EQ( quantity,  order.getQuantity() );
        ASSERT_EQ( orderType, order.getType()     );
    }
}

TEST(OrderBookTests, OrderCancel)  // NOLINT
{
    const Order::PriceType price = 1000;
    const Order::QuantityType quantity = 100;
    const std::array<Order::Type, 2> allOrderTypes = {Order::Type::Bid, Order::Type::Ask};
    for (auto orderType : allOrderTypes)
    {
        std::vector<Order> canceledOrders;
        OrderBook orderBook(nullptr,
                            [&canceledOrders](Order order) { canceledOrders.push_back(order); });
        auto id = orderBook.addOrder(orderType, 1000, 100);
        orderBook.cancelOrder(id);
        ASSERT_EQ(canceledOrders.size(), 1);
        auto& order = canceledOrders[0];
        ASSERT_EQ( price,     order.getPrice()    );
        ASSERT_EQ( quantity,  order.getQuantity() );
        ASSERT_EQ( orderType, order.getType()     );
        ASSERT_THROW(
                {
                    auto tmpOrder = orderBook.getOrderById(id);
                },
                NotFoundException);
    }
}

TEST(OrderBookTests, OrderBookMarketData2)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    auto result = R"V({
    "best_ask": {
        "price": 1001,
        "quantity": 30
    },
    "best_bid": {
        "price": 999,
        "quantity": 40
    },
    "asks": [
        {
            "price": 1001,
            "quantity": 30
        },
        {
            "price": 1002,
            "quantity": 30
        },
        {
            "price": 1003,
            "quantity": 90
        }
    ],
    "bids": [
        {
            "price": 999,
            "quantity": 40
        },
        {
            "price": 900,
            "quantity": 79
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    ASSERT_STREQ(marketData2Json.c_str(), result);
}

TEST(OrderBookTests, OrderAddBid)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1000, 300);
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    auto result = R"V({
    "best_ask": {
        "price": 1001,
        "quantity": 30
    },
    "best_bid": {
        "price": 1000,
        "quantity": 300
    },
    "asks": [
        {
            "price": 1001,
            "quantity": 30
        },
        {
            "price": 1002,
            "quantity": 30
        },
        {
            "price": 1003,
            "quantity": 90
        }
    ],
    "bids": [
        {
            "price": 1000,
            "quantity": 300
        },
        {
            "price": 999,
            "quantity": 40
        },
        {
            "price": 900,
            "quantity": 79
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    ASSERT_STREQ(marketData2Json.c_str(), result);
}

TEST(OrderBookTests, OrderAddAsk)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Ask, 1000, 300);
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    auto result = R"V({
    "best_ask": {
        "price": 1000,
        "quantity": 300
    },
    "best_bid": {
        "price": 999,
        "quantity": 40
    },
    "asks": [
        {
            "price": 1000,
            "quantity": 300
        },
        {
            "price": 1001,
            "quantity": 30
        },
        {
            "price": 1002,
            "quantity": 30
        },
        {
            "price": 1003,
            "quantity": 90
        }
    ],
    "bids": [
        {
            "price": 999,
            "quantity": 40
        },
        {
            "price": 900,
            "quantity": 79
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    ASSERT_STREQ(marketData2Json.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionBidBigPriceMarket1)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1002, 45);
    auto result = R"V({
    "best_ask": {
        "price": 1002,
        "quantity": 15
    },
    "best_bid": {
        "price": 999,
        "quantity": 40
    },
    "last_transaction": {
        "price": 1002,
        "quantity": 15
    }
}
)V";
    auto marketData1Json = orderBook.marketDataL1JsonSnapshot();
    ASSERT_STREQ(marketData1Json.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionBidBigPriceExecutedTransactions)  // NOLINT
{
    std::vector<Order> executedOrders;
    OrderBook orderBook = testOrderBook([&executedOrders](Order order)
            {
                executedOrders.push_back(order);
            });
    orderBook.addOrder(Order::Type::Bid, 1002, 45);
    static constexpr int size = 6;
    ASSERT_EQ(executedOrders.size(), size);
    std::array<Data, size> results = {
            Data{Order::Type::Ask, 1001, 20},
            Data{Order::Type::Bid, 1001, 20}, // partial execution of incoming order
            Data{Order::Type::Ask, 1001, 10},
            Data{Order::Type::Bid, 1001, 10}, // partial execution of incoming order
            Data{Order::Type::Ask, 1002, 15},
            Data{Order::Type::Bid, 1002, 15}  // final execution of incoming order
    };
    for (auto i = 0; i < size; ++i)
    {
        const auto& order = executedOrders[i];
        const auto& result = results[i];
        ASSERT_EQ(order.getType(),     result.type    );
        ASSERT_EQ(order.getPrice(),    result.price   );
        ASSERT_EQ(order.getQuantity(), result.quantity);
    }
}

TEST(OrderBookTests, OrderExecutionBidBigPrice)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1002, 45);
    auto result = R"V({
    "best_ask": {
        "price": 1002,
        "quantity": 15
    },
    "best_bid": {
        "price": 999,
        "quantity": 40
    },
    "last_transaction": {
        "price": 1002,
        "quantity": 15
    },
    "asks": [
        {
            "price": 1002,
            "quantity": 15
        },
        {
            "price": 1003,
            "quantity": 90
        }
    ],
    "bids": [
        {
            "price": 999,
            "quantity": 40
        },
        {
            "price": 900,
            "quantity": 79
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    ASSERT_STREQ(marketData2Json.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionBidSmallPrice)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1001, 45);
    auto result = R"V({
    "best_ask": {
        "price": 1002,
        "quantity": 30
    },
    "best_bid": {
        "price": 1001,
        "quantity": 15
    },
    "last_transaction": {
        "price": 1001,
        "quantity": 30
    },
    "asks": [
        {
            "price": 1002,
            "quantity": 30
        },
        {
            "price": 1003,
            "quantity": 90
        }
    ],
    "bids": [
        {
            "price": 1001,
            "quantity": 15
        },
        {
            "price": 999,
            "quantity": 40
        },
        {
            "price": 900,
            "quantity": 79
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    ASSERT_STREQ(marketData2Json.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionAskBigPrice)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Ask, 900, 80);
    auto result = R"V({
    "best_ask": {
        "price": 1001,
        "quantity": 30
    },
    "best_bid": {
        "price": 900,
        "quantity": 39
    },
    "last_transaction": {
        "price": 900,
        "quantity": 40
    },
    "asks": [
        {
            "price": 1001,
            "quantity": 30
        },
        {
            "price": 1002,
            "quantity": 30
        },
        {
            "price": 1003,
            "quantity": 90
        }
    ],
    "bids": [
        {
            "price": 900,
            "quantity": 39
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    ASSERT_STREQ(marketData2Json.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionAskSmallPrice)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Ask, 980, 80);
    auto result = R"V({
    "best_ask": {
        "price": 980,
        "quantity": 40
    },
    "best_bid": {
        "price": 900,
        "quantity": 79
    },
    "last_transaction": {
        "price": 999,
        "quantity": 40
    },
    "asks": [
        {
            "price": 980,
            "quantity": 40
        },
        {
            "price": 1001,
            "quantity": 30
        },
        {
            "price": 1002,
            "quantity": 30
        },
        {
            "price": 1003,
            "quantity": 90
        }
    ],
    "bids": [
        {
            "price": 900,
            "quantity": 79
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    ASSERT_STREQ(marketData2Json.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionBidBigPriceBigQuantity)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1010, 300);
    auto result = R"V({
    "best_bid": {
        "price": 1010,
        "quantity": 150
    }
    "last_transaction": {
        "price": 1003,
        "quantity": 90
    },
    "asks": [

    ],
    "bids": [
        {
            "price": 1010,
            "quantity": 150
        },
        {
            "price": 999,
            "quantity": 40
        },
        {
            "price": 900,
            "quantity": 79
        },
        {
            "price": 800,
            "quantity": 55
        }
    ]
}
)V";
    auto marketData2Json = orderBook.marketDataL2JsonSnapshot();
    ASSERT_STREQ(marketData2Json.c_str(), result);
}


/**
 *  @brief Run all tests
 */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}