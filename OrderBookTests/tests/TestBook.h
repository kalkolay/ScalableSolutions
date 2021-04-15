#pragma once

#include <OrderBook.h>

struct Data
{
    Order::Type         type;
    Order::PriceType    price;
    Order::QuantityType quantity;
};

OrderBook testOrderBook(OrderBook::OrderCallback executedOrderCallback = nullptr,
                        OrderBook::OrderCallback canceledOrderCallback = nullptr);