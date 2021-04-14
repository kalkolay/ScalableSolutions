#include "OrderBook.h"

bool OrderBook::isEmpty() const
{
    return _asks.empty() && _bids.empty();
}