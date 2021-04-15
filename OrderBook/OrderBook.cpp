#include "OrderBook.h"
#include "NotFoundException.h"

uint64_t Order::_nextId = 0;

OrderBook::OrderBook(OrderCallback executedOrderCallback,
                     OrderCallback canceledOrderCallback)
    : _executedOrderCallback ( std::move(executedOrderCallback) )
    , _canceledOrderCallback ( std::move(canceledOrderCallback) )
    , _haveTransactionsStarted(false )
    , _lastPrice             ( 0 )
    , _lastQuantity          ( 0 )
{}

void OrderBook::sendExecutedOrder(Order order)
{
    if (_executedOrderCallback)
        _executedOrderCallback(order);
}

template <typename Container>
bool OrderBook::tryExecute(Order&                      order,
                           Container&                  container,
                           const CompareOrderFunction& possibleExecution)
{
    Order containerOrder = Order::makeZeroOrder();

    for (auto it = container.begin();
         it != container.end() && order.getQuantity() > 0 &&
         possibleExecution( it->getPrice(), order.getPrice() );)
    {
        /// Remove first order from book
        containerOrder = *it;
        _idOrderLink.erase( it->getId() );
        it = container.erase(it);

        /// Determine execution parameters
        auto executionQuantity = std::min( containerOrder.getQuantity(), order.getQuantity() );
        auto executionPrice = containerOrder.getPrice();

        /// Execution
        auto executedOrder = containerOrder.split(executionQuantity, executionPrice);
        sendExecutedOrder(executedOrder);  // May be full order or a part
        auto executedIncomingOrder = order.split(executionQuantity, executionPrice);
        sendExecutedOrder(executedIncomingOrder);  // May be full order or a part

        /// Update market data
        if (_haveTransactionsStarted && _lastPrice == executionPrice)
            _lastQuantity += executionQuantity;
        else
            _lastQuantity = executionQuantity;
        _lastPrice = executionPrice;
        _haveTransactionsStarted = true;
    }

    if (containerOrder.getQuantity() != 0)  // Return the rest part of order to OrderBook
    {
        auto res = container.emplace(containerOrder);
        auto& it = res.first;
        assert(res.second);
        _idOrderLink.emplace( std::make_pair(it->getId(), it) );
    }
    return order.getQuantity() == 0;
}

bool OrderBook::tryExecute(Order& order)
{
    if (order.getType() == Order::Type::Bid)
    {
        return tryExecute(order, _askQueue,
                          [](Order::PriceType priceOrderInQueue, Order::PriceType priceOrderCome)
                          {
                              return priceOrderInQueue <= priceOrderCome;
                          });
    }
    else
    {
        return tryExecute(order, _bidQueue,
                          [](Order::PriceType priceOrderInQueue, Order::PriceType priceOrderCome)
                          {
                              return priceOrderInQueue >= priceOrderCome;
                          });
    }
}

bool OrderBook::checkConsistency() const
{
    return _askQueue.size() + _bidQueue.size() == _idOrderLink.size();
}

Order::IdType OrderBook::addOrder(Order::Type         type,
                                  Order::PriceType    price,
                                  Order::QuantityType quantity)
{
    Order order(type, price, quantity);
    Order::IdType id = order.getId();

    auto isFullyExecuted = tryExecute(order);
    if (not isFullyExecuted)  // Place order to book
    {
        std::pair<OrderContainerIterator, bool> res;
        res = ( type == Order::Type::Bid ? _bidQueue.emplace(order) : _askQueue.emplace(order) );
        assert(res.second);
        auto& it = res.first;
        _idOrderLink.emplace( std::make_pair(it->getId(), it) );
    }

    assert( checkConsistency() );
    return id;
}

OrderBook::IdOrderLink::const_iterator OrderBook::findOrder(Order::IdType id) const
{
    auto orderLink = _idOrderLink.find(id);
    if ( orderLink == _idOrderLink.end() )
        throw NotFoundException( std::string("Order id ") + std::to_string(id) + "not found" );
    return orderLink;
}

void OrderBook::cancelOrder(Order::IdType id)
{
    auto orderLink = findOrder(id);
    if (_canceledOrderCallback)
        _canceledOrderCallback( *(orderLink->second) );

    orderLink->second->getType() == Order::Type::Ask ?
        _askQueue.erase(orderLink->second) :
        _bidQueue.erase(orderLink->second);
    _idOrderLink.erase(orderLink);

    assert( checkConsistency() );
}

Order OrderBook::getOrderById(Order::IdType id) const
{
    auto orderLink = findOrder(id);
    return *(orderLink->second);
}

