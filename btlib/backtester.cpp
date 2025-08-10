#include <bits/stdc++.h>
#include "backtester.h"



Backtester::Backtester(std::vector<Bar> bars, std::unique_ptr<IStrategy> strat, Broker broker, double starting_cash)
    : bars_(std::move(bars)), strat_(std::move(strat)), broker_(std::move(broker)), pf_(starting_cash) {
    if (bars_.empty()) throw std::invalid_argument("No bars provided.");
}

Results Backtester::run() {
    Results r;
    r.start_equity = pf_.equity(bars_.front().close);

    std::vector<double> rets;
    rets.reserve(bars_.size());
    double peak = r.start_equity;

    long global_order_id = 0;

    for (const auto& b : bars_) {
        auto ord_opt = strat_->on_bar(b, pf_);
        if (ord_opt) {
            Order o = *ord_opt;
            if (o.id == 0) o.id = ++global_order_id;

            auto fill = broker_.try_execute(o, b);
            if (fill) {
                double comm = broker_.commission(fill->qty, fill->price);
                pf_.on_fill(*fill, comm);
                r.trades++;
            }
        }

        double eq = pf_.equity(b.close);
        r.equity_curve.push_back(eq);
        r.dates.push_back(b.date);

        peak = std::max(peak, eq);
        double dd = (peak - eq) / std::max(peak, 1e-9);
        r.max_drawdown = std::max(r.max_drawdown, dd);

        if (r.equity_curve.size() > 1) {
            double prev = r.equity_curve[r.equity_curve.size()-2];
            double ret = (eq - prev) / std::max(prev, 1e-9);
            rets.push_back(ret);
        }
    }

    r.end_equity = r.equity_curve.back();

    double yrs = std::max(1e-9, r.equity_curve.size() / 252.0);
    r.cagr = std::pow(std::max(1e-12, r.end_equity / r.start_equity), 1.0 / yrs) - 1.0;

    if (!rets.empty()) {
        double mean = std::accumulate(rets.begin(), rets.end(), 0.0) / rets.size();
        double var = 0.0;
        for (double x : rets) var += (x - mean) * (x - mean);
        var /= std::max<size_t>(1, rets.size() - 1);
        double stddev = std::sqrt(var);
        r.sharpe = (stddev > 0) ? (mean / stddev) * std::sqrt(252.0) : 0.0;
    }

    return r;
}
