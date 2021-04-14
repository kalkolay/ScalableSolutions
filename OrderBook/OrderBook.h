#pragma once

#include <map>
#include <boost/optional.hpp>

class OrderBook
{
public:


    bool isEmpty() const;

private:
    std::map<int, int> _asks, _bids;
};
