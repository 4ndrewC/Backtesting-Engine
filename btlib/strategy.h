#pragma once
#include <optional>
#include "types.h"
#include "portfolio.h"

// Strategies implement this interface and can be compiled in separate files.
class IStrategy {
public:
    virtual ~IStrategy() = default;
    virtual std::optional<Order> on_bar(const Bar& bar, const Portfolio& pf) = 0;
};
