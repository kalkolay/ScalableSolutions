#include "TestBook.h"

#include <array>

OrderBook testOrderBook(OrderBook::OrderCallback executedOrderCallback,
                        OrderBook::OrderCallback canceledOrderCallback)
{
    OrderBook orderBook( std::move(executedOrderCallback),
                         std::move(canceledOrderCallback) );
    std::array<Data, 10> orders =
            {
                    Data{Order::Type::Ask, 1003, 50},
                    Data{Order::Type::Ask, 1003, 40},
                    Data{Order::Type::Ask, 1002, 30},
                    Data{Order::Type::Ask, 1001, 20},
                    Data{Order::Type::Ask, 1001, 10},
                    Data{Order::Type::Bid, 999,  15},
                    Data{Order::Type::Bid, 999,  25},
                    Data{Order::Type::Bid, 900,  35},
                    Data{Order::Type::Bid, 900,  44},
                    Data{Order::Type::Bid, 800,  55}
            };
    for (const auto& order : orders)
        orderBook.addOrder(order.type, order.price, order.quantity);
    return orderBook;
}