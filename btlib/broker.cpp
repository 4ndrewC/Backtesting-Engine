#include "broker.h"
#include <bits/stdc++.h>

Broker::Broker(BrokerConfig cfg) : cfg_(cfg) {}

std::optional<Fill> Broker::try_execute(const Order& o, const Bar& bar) const {
    double px = bar.close;

    if (o.type == OrderType::Limit) {
        if (o.side == Side::Buy && !(bar.low <= o.limit_price)) return std::nullopt;
        if (o.side == Side::Sell && !(bar.high >= o.limit_price)) return std::nullopt;
        px = o.limit_price;
    } else if (o.type == OrderType::Stop) {
        bool trig = (o.side == Side::Sell) ? (bar.low <= o.stop_price) : (bar.high >= o.stop_price);
        if (!trig) return std::nullopt;
        px = o.stop_price;
    }

    double slip = cfg_.slippage_bps * 1e-4 * px;
    if (o.side == Side::Buy) px += slip; else px -= slip;

    Fill f;
    f.order_id = o.id;
    f.side = o.side;
    f.qty = o.qty;
    f.price = px;
    f.date = bar.date;
    return f;
}

double Broker::commission(double qty, double /*price*/) const {
    double c = std::fabs(qty) * cfg_.commission_per_share;
    return std::max(c, cfg_.commission_min);
}
