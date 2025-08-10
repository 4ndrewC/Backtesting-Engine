# Backtesting-Engine
heh...

# Write Your Own Strategy
---
In ```./strategy/ ```, create your own strategy header file (e.g ```SMACrossover.h```)
In ```main.cpp```, integrate your strategy like so
```cpp
int fast = 20, slow = 50;
auto strat = std::make_unique<SMACrossover>(fast, slow, 0.95);
```
In your strategy class, the function ```cpp optional<Order> on_bar(const Bar& bar, const Portfolio& pf) override() ``` is your per bar trade action.

# Run
---
In terminal:
```shell
g++ ./btlib/*.cpp main.cpp -o backtester
backtester ./data/{data.txt} # replace with market data in csv format
```
