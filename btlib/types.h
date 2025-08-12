#pragma once
#include <bits/stdc++.h>
#include <optional>

enum class Side { Buy, Sell };
enum class OrderType { Market, Limit, Stop };

struct Bar {
    std::string date;
    double open{}, high{}, low{}, close{}, volume{};
};

struct Order {
    OrderType type{OrderType::Market};
    Side side{Side::Buy};
    double qty{0.0};
    double limit_price{NAN};
    double stop_price{NAN};
    std::string tag;
    long id{0};
};

struct Fill {
    long order_id{0};
    Side side{Side::Buy};
    double qty{0.0};
    double price{0.0};
    std::string date;
};

