# Scalable Solutions test assignment

The purpose of this assignment was to implement a simplified **order book**.\
Functional requirements:
- GTC orders placement
- Cancellation of orders by the identifier
- Retrieving orders data by the identifier
- Orders matching
- Retrieving market data snapshot:
```json
{
  "asks": [
    {
      "price": "<value>",
      "quantity": "<value>"
    },
    ...
  ],
  "bids": [...]
}
```
- Code should be developed on C++11 or C++14, be documented and be built on *gcc5+* or *clang* via CMake for Linux
- Class should be covered with unit tests

## How to build

This project is developed on C++14 (*clang-1200.0.32.29*) for macOS/Linux.\
It requires [CMake 3.5+](https://cmake.org) for building and uses [GoogleTest](https://github.com/google/googletest.git) for unit testing and [Git](https://git-scm.com) for VCS.

To build & run the test assignment, execute the following commands from *cmake-build-debug* folder:
```shell
cmake -G "Unix Makefiles" .. && make    # 1. build
./OrderBookTests/tests/RunTests         # 2. run tests
```

## Orders matching logic

The following rules are used for orders matching:
- If a bid order comes in at a price greater or equal than the lowest ask price, then we execute order by ask price. The buyer buys at his proposed price or less. The seller sells at his proposed price.
- Either if an ask order comes in at a price lower or equal to the highest bid price in the order book, then the order is executed by bid price. The seller sells at his proposed price or more. The buyer buys at his proposed price.