#pragma once

#include <functional>
#include <map>
#include <set>

#include "Order.h"

class OrderBook
{
public:
    /**
     *  @brief Callback type for executed and canceled orders
     */
    using OrderCallback = std::function<void (Order)>;

    /**
     *  @brief Explicitly create OrderBook
     *
     *  @param executedOrderCallback std::function which accepts executed orders
     *  @param canceledOrderCallback std::function which accepts canceled orders
     *
     *  @note Parameters may be nullptr
     */
    explicit OrderBook(OrderCallback executedOrderCallback = nullptr,
                       OrderCallback canceledOrderCallback = nullptr);

    /**
     *  @brief Add order to OrderBook
     *
     *  @param type     Order type
     *  @param price    Order price
     *  @param quantity Order quantity
     *
     *  @note Type can be either Order::Type::Bid or Order::Type::Ask
     */
    Order::IdType addOrder(Order::Type         type,
                           Order::PriceType    price,
                           Order::QuantityType quantity);

    /**
     *  @brief Cancel order
     *
     *  @param id Order ID
     *
     *  @throws NotFoundException Thrown in case the order cannot be found
     */
    void cancelOrder(Order::IdType id);

    /**
     *  @brief Get order copy
     *
     *  @param id Order id
     *
     *  @throws NotFoundException Thrown in case the order cannot be found
     *
     *  @note Can be used to print order info
     */
    Order getOrderById(Order::IdType id) const;

private:
    struct AskOrderSort
    {
        bool operator ()(const Order& o1,
                         const Order& o2) const
        {
            return o1.getPrice() < o2.getPrice() ||
                   o1.getPrice() == o2.getPrice() && o1.getId() < o2.getId();
        }
    };
    struct BidOrderSort
    {
        bool operator ()(const Order& o1,
                         const Order& o2) const
        {
            return o1.getPrice() > o2.getPrice() ||
                   o1.getPrice() == o2.getPrice() && o1.getId() < o2.getId();
        }
    };
    using OrderContainerAsk      = std::set<Order, AskOrderSort>;
    using OrderContainerBid      = std::set<Order, BidOrderSort>;
    using OrderContainerIterator = std::set<Order>::iterator;
    using IdOrderLink            = std::map<Order::IdType, OrderContainerIterator>;

    OrderContainerAsk _askQueue;
    OrderContainerBid _bidQueue;
    IdOrderLink _idOrderLink;
    OrderCallback _executedOrderCallback;
    OrderCallback _canceledOrderCallback;

    bool _haveTransactionsStarted;
    Order::PriceType _lastPrice;
    Order::QuantityType _lastQuantity;

    void sendExecutedOrder(Order order);

    /**
     *  @brief Return true if incoming order fully executed
     *
     *  @overload
     */
    bool tryExecute(Order &order);

    using CompareOrderFunction = std::function<bool (Order::PriceType, Order::PriceType)>;
    template <typename Container>
    bool tryExecute(Order&                      order,
                    Container&                  container,
                    const CompareOrderFunction& possibleExecution);

    bool checkConsistency() const;

    IdOrderLink::const_iterator findOrder(Order::IdType id) const;


};
