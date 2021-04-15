#include <gtest/gtest.h>
#include <array>

#include "TestBook.h"

TEST(OrderBookTests, OrderBookInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    auto res = R"V({
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
    ASSERT_STREQ(orderbookInfoJson.c_str(), res);
}

TEST(OrderBookTests, OrderBookInfoLimit)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    auto orderbookInfoJson = orderBook.orderbookInfoJson(2,1);
    auto res = R"V({
    "asks": [
        {
            "price": 1001,
            "quantity": 30
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
        }
    ]
}
)V";
    ASSERT_STREQ(orderbookInfoJson.c_str(), res);
}

TEST(OrderBookTests, OrderAddBidInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1000, 300);
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    auto res = R"V({
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
    ASSERT_STREQ(orderbookInfoJson.c_str(), res);
}

TEST(OrderBookTests, OrderAddAskInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Ask, 1000, 300);
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    auto res = R"V({
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
    ASSERT_STREQ(orderbookInfoJson.c_str(), res);
}

TEST(OrderBookTests, OrderExecutionBidBigPriceInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1002, 45);
    auto result = R"V({
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
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    ASSERT_STREQ(orderbookInfoJson.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionBidSmallPriceInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1001, 45);
    auto result = R"V({
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
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    ASSERT_STREQ(orderbookInfoJson.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionAskBigPriceInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Ask, 900, 80);
    auto result = R"V({
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
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    ASSERT_STREQ(orderbookInfoJson.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionAskSmallPriceInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Ask, 980, 80);
    auto result = R"V({
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
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    ASSERT_STREQ(orderbookInfoJson.c_str(), result);
}


TEST(OrderBookTests, OrderExecutionBidBigPriceBigQuantityInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1010, 300);
    auto result = R"V({
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
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    ASSERT_STREQ(orderbookInfoJson.c_str(), result);
}

TEST(OrderBookTests, OrderExecutionAskBigPriceBigQuantityInfo)  // NOLINT
{
    OrderBook orderBook = testOrderBook();
    orderBook.addOrder(Order::Type::Bid, 1010, 300);
    auto result = R"V({
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
    auto orderbookInfoJson = orderBook.orderbookInfoJson();
    ASSERT_STREQ(orderbookInfoJson.c_str(), result);
}