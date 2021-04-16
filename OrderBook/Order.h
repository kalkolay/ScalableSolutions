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

    Order(Type, PriceType, QuantityType);

    [[nodiscard]] Type         getType    () const { return _type;     }
    [[nodiscard]] PriceType    getPrice   () const { return _price;    }
    [[nodiscard]] QuantityType getQuantity() const { return _quantity; }
    [[nodiscard]] IdType       getId      () const { return _id;       }

    /**
     *  @brief Create explicitly empty order to save an order from split
     */
    static Order makeEmptyOrder() { return {}; }

    /**
     *  @brief Return order with the same ID, price, type but split original _quantity
     *         by new quantity and rest which saved in current order
     */
    Order split(QuantityType quantity,
                PriceType    executionPrice);

private:
    Type         _type;
    IdType       _id;
    PriceType    _price;
    QuantityType _quantity;

    /**
     *  @brief ID generator for new order
     */
    static IdType _nextId;

    /**
     *  @note Forbid creating objects via default constructor
     */
    Order();
};