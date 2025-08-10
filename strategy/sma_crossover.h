#pragma once
#include <bits/stdc++.h>
#include "../btlib/strategy.h"  

// Simple SMA crossover: enter long when fast SMA > slow SMA; exit when fast <= slow
class SMACrossover final : public IStrategy {
public:
    explicit SMACrossover(int fast, int slow, double risk_fraction = 0.95)
        : fast_(fast), slow_(slow), risk_fraction_(risk_fraction) {
        if (fast_ <= 0 || slow_ <= 0 || fast_ >= slow_) throw std::invalid_argument("Invalid SMA params");
    }

    std::optional<Order> on_bar(const Bar& bar, const Portfolio& pf) override {
        closes_.push_back(bar.close);
        if ((int)closes_.size() < slow_) return std::nullopt;

        if ((int)closes_.size() == slow_) {
            fast_sum_ = std::accumulate(closes_.end()-fast_, closes_.end(), 0.0);
            slow_sum_ = std::accumulate(closes_.end()-slow_, closes_.end(), 0.0);
        } else {
            fast_sum_ += bar.close - *(closes_.end()-1-fast_);
            slow_sum_ += bar.close - *(closes_.end()-1-slow_);
        }

        double fast = fast_sum_ / fast_;
        double slow = slow_sum_ / slow_;
        bool want_long = fast > slow;
        bool have_pos  = std::fabs(pf.position().qty) > 1e-9;

        if (want_long && !have_pos) {
            double budget = pf.cash() * risk_fraction_;
            double qty = std::floor(budget / std::max(1e-9, bar.close));
            if (qty <= 0) return std::nullopt;
            Order o; o.side = Side::Buy; o.type = OrderType::Market; o.qty = qty; o.tag = "enter long";
            o.id = ++next_id_;
            return o;
        } else if (!want_long && have_pos) {
            double qty = std::fabs(pf.position().qty);
            Order o; o.side = Side::Sell; o.type = OrderType::Market; o.qty = qty; o.tag = "exit long";
            o.id = ++next_id_;
            return o;
        }
        return std::nullopt;
    }

private:
    int fast_, slow_;
    double risk_fraction_;
    std::vector<double> closes_;
    double fast_sum_{0.0}, slow_sum_{0.0};
    long next_id_{0};
};
