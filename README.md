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
It requires [Boost libraries](https://www.boost.org) and uses [GoogleTest](https://github.com/google/googletest.git) for unit testing and [Git](https://git-scm.com) for VCS.

To build & run the test assignment, execute the following commands from *cmake-build-debug* folder:
```shell
# 0. Install Boost via any package manager.
# E.g. "sudo apt-get install libboost-all-dev" on Linux
# or "brew install boost" on macOS
cmake -G "Unix Makefiles" .. && make    # 1. build
./OrderBookTests/tests/RunTests         # 2. run tests
```
P.S. [CMake 3.5+](https://cmake.org) is required for building the project.