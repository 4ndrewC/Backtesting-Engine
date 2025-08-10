#include <bits/stdc++.h>
#include "btlib/csv_loader.h"
#include "btlib/broker.h"
#include "btlib/backtester.h"
#include "strategy/sma_crossover.h"

static void print_summary(const Results& r) {
    auto pct = [](double x){ return 100.0 * x; };
    std::cout.setf(std::ios::fixed); std::cout << std::setprecision(2);
    std::cout << "====== Backtest Summary ======\n";
    std::cout << "Start Equity : $" << r.start_equity << "\n";
    std::cout << "End   Equity : $" << r.end_equity   << "\n";
    std::cout << "CAGR         : " << pct(r.cagr) << "%\n";
    std::cout << "Max Drawdown : " << pct(r.max_drawdown) << "%\n";
    std::cout << "Sharpe       : " << r.sharpe << "\n";
    std::cout << "Trades       : " << r.trades << "\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " data.csv\n";
        return 1;
    }
    std::string path = argv[1];

    auto bars = load_csv(path);

    int fast = 20, slow = 50;
    auto strat = std::make_unique<SMACrossover>(fast, slow, 0.95);  // example

    BrokerConfig cfg;
    cfg.commission_per_share = 0.005;
    cfg.commission_min = 1.00;
    cfg.slippage_bps = 1.0;
    cfg.allow_short = false;
    Broker broker(cfg);

    Backtester bt(std::move(bars), std::move(strat), std::move(broker), 100000.0);
    Results res = bt.run();
    print_summary(res);

    // Optional: write equity curve CSV
    std::ofstream out("equity_curve.csv");
    out << "date,equity\n";
    for (size_t i = 0; i < res.equity_curve.size(); ++i) {
        out << res.dates[i] << "," << std::setprecision(10) << res.equity_curve[i] << "\n";
    }
    std::cerr << "Wrote equity_curve.csv (" << res.equity_curve.size() << " rows)\n";
    return 0;
}
