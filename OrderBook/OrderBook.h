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
     *  @brief Explicitly create order book
     *
     *  @param executedOrderCallback std::function which accepts executed orders
     *  @param canceledOrderCallback std::function which accepts canceled orders
     *
     *  @note Parameters may be nullptr
     */
    explicit OrderBook(OrderCallback executedOrderCallback = nullptr,
                       OrderCallback canceledOrderCallback = nullptr);

    /**
     *  @brief Add order to order book
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

    /**
     *  @brief Order book information in JSON format
     *
     *  @param bidOrderLimit Max number bid positions
     *  @param askOrderLimit Max number ask positions
     *
     *  @note -1 means output all bid positions
     */
    std::string orderbookInfoJson(int bidOrderLimit = -1,
                                  int askOrderLimit = -1) const;

private:

    // TODO: Add PImpl

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

    OrderContainerAsk   _askQueue;
    OrderContainerBid   _bidQueue;
    IdOrderLink         _idOrderLink;
    OrderCallback       _executedOrderCallback;
    OrderCallback       _canceledOrderCallback;

    bool                _haveTransactionsStarted;
    Order::PriceType    _lastPrice;
    Order::QuantityType _lastQuantity;

    void sendExecutedOrder(Order order);

    /**
     *  @return true if incoming order fully executed
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

    struct PricePosition
    {
        Order::PriceType    price    = 0;
        Order::QuantityType quantity = 0;
    };

    class PriceAggregator
    {
    public:
        PriceAggregator(OrderContainerIterator startPos,
                        OrderContainerIterator endPos);
        std::pair<bool, PricePosition> nextPrice();
    private:
        OrderContainerIterator       _curPos;
        const OrderContainerIterator _endPos;
    };

    PriceAggregator makePriceAggregator(Order::Type type) const;

    friend void outOrdersJson(std::ostream&               outStr,
                              int                         orderLimit,
                              OrderBook::PriceAggregator& aggregator);

    /**
     *  @brief Helper method for retrieving order book information in JSON format
     *
     *  @see orderbookInfoJson
     */
    void orderbookInfoJsonInternal(std::ostream& outStr,
                                   int           bidOrderLimit,
                                   int           askOrderLimit) const;


};
