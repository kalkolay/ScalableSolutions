#include "OrderBook.h"

#include <iomanip>
#include <sstream>

uint64_t Order::_nextId = 0;

OrderBook::OrderBook(OrderCallback executedOrderCallback,
                     OrderCallback canceledOrderCallback)
    : _executedOrderCallback  ( std::move(executedOrderCallback) )
    , _canceledOrderCallback  ( std::move(canceledOrderCallback) )
    , _haveTransactionsStarted( false )
    , _lastPrice              ( 0 )
    , _lastQuantity           ( 0 )
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
    Order containerOrder = Order::makeEmptyOrder();

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
    else  // Order::Type::Ask
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

OrderBook::PriceAggregator::PriceAggregator(OrderContainerIterator startPos,
                                            OrderContainerIterator endPos)
    : _curPos(startPos)
    , _endPos(endPos)
{}

std::pair<bool, OrderBook::PricePosition> OrderBook::PriceAggregator::nextPrice()
{
    PricePosition pricePosition;

    if (_curPos == _endPos)  // End of container
        return std::make_pair(false, pricePosition);

    pricePosition.price    = _curPos->getPrice();
    pricePosition.quantity = _curPos->getQuantity();

    while (++_curPos != _endPos && _curPos->getPrice() == pricePosition.price)
        pricePosition.quantity += _curPos->getQuantity();

    return std::make_pair(true, pricePosition);
}

OrderBook::PriceAggregator OrderBook::makePriceAggregator(Order::Type type) const
{
    if (type == Order::Type::Ask)
        return { _askQueue.cbegin(), _askQueue.cend() };
    else  // Order::Type::Bid
        return { _bidQueue.cbegin(), _bidQueue.cend() };
}

void outputOrdersJson(std::ostream&               outStr,
                      int                         orderLimit,
                      OrderBook::PriceAggregator& aggregator)
{
    bool nextIteration = false;

    while (orderLimit < 0 || orderLimit-- != 0)
    {
        auto pricePositionPair = aggregator.nextPrice();
        if (!pricePositionPair.first)
            break;  // No more prices in the queue
        if (nextIteration)
            outStr << ',' << std::endl;
        outStr  << std::setw(8) << ' '
                << '{' << std::endl
                << std::setw(12) << ' '
                << "\"price\": " << pricePositionPair.second.price << ',' << std::endl
                << std::setw(12) << ' '
                << "\"quantity\": " << pricePositionPair.second.quantity << std::endl
                << std::setw(8) << ' '
                << '}';
        nextIteration = true;
    }
}

void OrderBook::orderBookInfoJsonInternal(std::ostream& outStr,
                                          int           bidOrderLimit,
                                          int           askOrderLimit) const
{
    outStr << R"V(    "asks": [
)V";
    {
        auto aggregator = makePriceAggregator(Order::Type::Ask);
        outputOrdersJson(outStr, askOrderLimit, aggregator);
    }
    outStr  << std::endl
            << std::setw(4) << ' '
            << "]," << std::endl
            << std::setw(4) << ' '
            << "\"bids\": [" << std::endl;
    {
        auto aggregator = makePriceAggregator(Order::Type::Bid);
        outputOrdersJson(outStr, bidOrderLimit, aggregator);
    }
    outStr  << std::endl
            << std::setw(4) << ' '
            << ']' << std::endl;
}

std::string OrderBook::getOrderBookInfoJson(int bidOrderLimit,
                                            int askOrderLimit) const
{
    std::ostringstream outStr;
    outStr << '{' << std::endl;
    orderBookInfoJsonInternal(outStr, bidOrderLimit, askOrderLimit);
    outStr << '}' << std::endl;
    return outStr.str();
}

void outputBestAskJson(std::ostream&                                    outStr,
                       const std::pair<bool, OrderBook::PricePosition>& askPricePositionPair)
{
    if (askPricePositionPair.first)
    {
        const auto& pricePosition = askPricePositionPair.second;
        outStr  << R"V(
    "best_ask": {
        "price": )V"
                << pricePosition.price << R"V(,
        "quantity": )V"
                << pricePosition.quantity << R"V(
    })V";
    }
}

void outputBestBidJson(std::ostream&                                    outStr,
                       const std::pair<bool, OrderBook::PricePosition>& bidPricePositionPair)
{
    if (bidPricePositionPair.first)
    {
        const auto& pricePosition = bidPricePositionPair.second;
        outStr  << R"V(
    "best_bid": {
        "price": )V"
                << pricePosition.price << R"V(,
        "quantity": )V"
                << pricePosition.quantity << R"V(
    })V";
    }
}

void OrderBook::marketDataL1JsonInternal(std::ostream& outStr,
                                         bool&         nextComma) const
{
    auto askPricePositionPair = makePriceAggregator(Order::Type::Ask).nextPrice();
    outputBestAskJson(outStr, askPricePositionPair);
    nextComma = askPricePositionPair.first;
    auto bidPricePositionPair = makePriceAggregator(Order::Type::Bid).nextPrice();
    if (nextComma && bidPricePositionPair.first)
        outStr << ',';
    outputBestBidJson(outStr, bidPricePositionPair);

    if (_haveTransactionsStarted)
    {
        if (nextComma)
            outStr << ',';
        outStr << R"V(
    "last_transaction": {
        "price": )V"    << _lastPrice    << R"V(,
        "quantity": )V" << _lastQuantity << R"V(
    })V";
    }
}

std::string OrderBook::marketDataL1JsonSnapshot() const
{
    std::ostringstream outStr;
    outStr << '{';
    bool nextComma = false;
    marketDataL1JsonInternal(outStr, nextComma);
    outStr << std::endl << '}' << std::endl;
    return outStr.str();
}

std::string OrderBook::marketDataL2JsonSnapshot(int bidOrderLimit,
                                                int askOrderLimit) const
{
    std::ostringstream outStr;
    outStr << '{';
    bool nextComma = false;
    marketDataL1JsonInternal(outStr, nextComma);
    outStr << ',' << std::endl;
    orderBookInfoJsonInternal(outStr, bidOrderLimit, askOrderLimit);
    outStr << '}' << std::endl;
    return outStr.str();
}