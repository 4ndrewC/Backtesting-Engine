#pragma once
#include <bits/stdc++.h>

#include "types.h"
#include "portfolio.h"
#include "broker.h"
#include "strategy.h"

struct Results {
    std::vector<double> equity_curve;
    std::vector<std::string> dates;
    double start_equity{0}, end_equity{0};
    double max_drawdown{0};
    double cagr{0};
    double sharpe{0};
    size_t trades{0};
};

class Backtester {
public:
    Backtester(std::vector<Bar> bars, std::unique_ptr<IStrategy> strat, Broker broker, double starting_cash = 100000.0);
    Results run();

private:
    std::vector<Bar> bars_;
    std::unique_ptr<IStrategy> strat_;
    Broker broker_;
    Portfolio pf_;
};
