#include "portfolio.h"
#include <bits/stdc++.h>

Portfolio::Portfolio(double starting_cash) : cash_(starting_cash) {}

void Portfolio::on_fill(const Fill& f, double commission) {
    double signed_qty = (f.side == Side::Buy ? +f.qty : -f.qty);
    double trade_cost = f.price * signed_qty;

    double new_qty = pos_.qty + signed_qty;

    if (pos_.qty == 0.0 || (pos_.qty > 0 && signed_qty > 0) || (pos_.qty < 0 && signed_qty < 0)) {
        // Increasing in same direction or opening new
        double total_abs = std::fabs(pos_.qty) + std::fabs(signed_qty);
        if (total_abs > 1e-12) {
            pos_.avg_price = (std::fabs(pos_.qty) * pos_.avg_price + std::fabs(signed_qty) * f.price) / total_abs;
        }
    } else {
        if (std::fabs(new_qty) < 1e-12) pos_.avg_price = 0.0; // fully flat
    }

    pos_.qty = new_qty;

    // Cash: buy consumes, sell adds; pay commission
    cash_ -= trade_cost;
    cash_ -= commission;
}

double Portfolio::equity(double last_price) const {
    double position_value = pos_.qty * last_price;
    return cash_ + position_value;
}
