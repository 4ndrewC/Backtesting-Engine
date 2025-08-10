#pragma once
#include "types.h"

struct BrokerConfig {
    double commission_per_share{0.0};
    double commission_min{0.0};
    double slippage_bps{0.0};
    bool  allow_short{true};
};

class Broker {
public:
    explicit Broker(BrokerConfig cfg);

    std::optional<Fill> try_execute(const Order& o, const Bar& bar) const;
    double commission(double qty, double price) const;
    const BrokerConfig& config() const { return cfg_; }

private:
    BrokerConfig cfg_;
};
