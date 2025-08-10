#pragma once
#include "types.h"

struct Position {
    double qty{0.0};
    double avg_price{0.0};
};

class Portfolio {
public:
    explicit Portfolio(double starting_cash = 100000.0);

    void on_fill(const Fill& f, double commission);
    double equity(double last_price) const;

    const Position& position() const { return pos_; }
    double cash() const { return cash_; }

private:
    double cash_{0.0};
    Position pos_;
};
