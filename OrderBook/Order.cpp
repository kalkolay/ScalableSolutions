#include "Order.h"

Order::Order()
    : _price   (0)
    , _quantity(0)
    , _id      (0)
{}

Order::Order(Type         type,
             PriceType    price,
             QuantityType quantity)
    : _type    (type)
    , _price   (price)
    , _quantity(quantity)
    , _id      (++_nextId)
{}

Order Order::split(QuantityType quantity,
                   PriceType    executionPrice)
{
    assert(quantity <= _quantity);
    assert( _type == Type::Ask && executionPrice >= _price ||
            _type == Type::Bid && executionPrice <= _price);

    Order newOrder = *this;
    newOrder._quantity = quantity;
    newOrder._price    = executionPrice;
    _quantity -= quantity;

    return newOrder;
}