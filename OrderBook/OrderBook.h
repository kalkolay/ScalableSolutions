#pragma once

#include <map>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/optional.hpp>

#include "Order.h"

class OrderBook
{
public:
    using orderID_t = boost::uuids::uuid;
    //boost::uuids::random_generator uuid_gen;
    //m_id = uuid_gen();



    bool isEmpty() const;

private:
    std::map<int, int> _asks, _bids;
};
