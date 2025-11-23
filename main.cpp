#include <thread>
#include <mutex>
#include <vector>

#include "btlib/csv_loader.h"
#include "btlib/broker.h"
#include "btlib/backtester.h"
#include "strategy/sma_crossover.h"

// #define debug

#ifdef debug
#include <chrono>
#endif

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

#ifdef debug

void naive(const std::vector<Bar> &bars, int count){
    auto start = std::chrono::high_resolution_clock::now();

    auto strat_create = [](){
        int fast = 20, slow = 50;
        return std::make_unique<SMACrossover>(fast, slow, 0.95);
    };

    for(int i = 0; i<count; i++){
        // std::cout<<i<<"th iteration\n";
        BrokerConfig cfg;
        cfg.commission_per_share = 0.005;
        cfg.commission_min = 1.00;
        cfg.slippage_bps = 1.0;
        cfg.allow_short = false;
        Broker broker(cfg);
        auto strat = strat_create();
        Backtester bt(bars, std::move(strat), std::move(broker), 100000.0);
        Results res = bt.run();
        // print_summary(res);
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "Program took " << diff.count() << " seconds\n";

}

#endif

void monte_carlo(const std::vector<Bar> &bars, int count){
    #ifdef debug
    auto start = std::chrono::high_resolution_clock::now();
    #endif
    const unsigned int cores = std::thread::hardware_concurrency();
    std::vector<std::thread> runs(cores);
    std::vector<std::atomic<bool>> done(cores);
    for(int i = 0; i<cores; i++) done[i] = true;
    
    // user define
    auto strat_create = [](){
        int fast = 20, slow = 50;
        return std::make_unique<SMACrossover>(fast, slow, 0.95);
    };


    std::mutex print_mtx;
    
    for(int i = 1; i<=count; i++){
        int open = 0;
        int idx = 0;
        while(!open){
            for(int j = 0; j<cores; j++){
                if(done[j]){
                    if(runs[j].joinable()) runs[j].join();
                    idx = j;
                    open = 1;
                }
            }
        }
        done[idx] = false;
        
        runs[idx] = std::thread([i, idx, &done, strat_create, &bars, &print_mtx]() mutable {

            auto strat = strat_create();

            BrokerConfig cfg;
            cfg.commission_per_share = 0.005;
            cfg.commission_min = 1.00;
            cfg.slippage_bps = 1.0;
            cfg.allow_short = false;
            Broker broker(cfg);

            Backtester bt(bars, std::move(strat), std::move(broker), 100000.0);
            Results res = bt.run();

            {
                std::lock_guard<std::mutex> lock(print_mtx);
                std::cout<<"---------------------------\n"<<i<<"th iteration: \n";
                print_summary(res);
            }

            done[idx] = true;
        });
    }

    for(auto &i: runs){
        if(i.joinable()) i.join();
    }

    #ifdef debug

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "Program took " << diff.count() << " seconds\n";

    #endif
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

