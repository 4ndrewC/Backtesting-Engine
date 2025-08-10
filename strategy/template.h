#pragma once
#include <bits/stdc++.h>
#include "../btlib/strategy.h"

class Strategy final : public IStrategy {
public:
    explicit Strategy(){}

    std::optional<Order> on_bar(const Bar& bar, const Portfolio& pf) override {
    	Order o;

        return o;  
    }

private:

};
