#pragma once

#include <cassert>
#include <cstdint>

class Order
{
public:
    enum class Type
    {
        Ask,
        Bid
    };
    using IdType       = uint64_t;
    using PriceType    = int32_t;
    using QuantityType = uint32_t;

    Order(Type         type,
          PriceType    price,
          QuantityType quantity)
        : _type    (type)
        , _price   (price)
        , _quantity(quantity)
        , _id      (++next_id)
    {}

    [[nodiscard]] Type         getType    () const { return _type; }
    [[nodiscard]] PriceType    getPrice   () const { return _price; }
    [[nodiscard]] QuantityType getQuantity() const { return _quantity; }
    [[nodiscard]] IdType       getId      () const { return _id; }

    static Order makeZeroOrder()  // create explicitly zero order to save some order from split
    {
        return {};
    }

    /// Return order with the same id, price, type but split original _quantity
    /// by new quantity and rest which saved in current order
    Order split(QuantityType quantity,
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

private:
    IdType       _id;
    PriceType    _price;
    QuantityType _quantity;
    Type         _type;

    static IdType next_id;  // id generator for new order

    Order()
        : _price   (0)
        , _quantity(0)
        , _id      (0)
    {}
};