#pragma once
#include <bits/stdc++.h>
#include "types.h"

// date,open,high,low,close,volume
inline std::vector<Bar> load_csv(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Failed to open CSV: " + path);

    std::string line;

    if (!std::getline(f, line)) throw std::runtime_error("Empty CSV: " + path);

    std::vector<Bar> bars;
    bars.reserve(1 << 20);

    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string cell;
        Bar b;

        std::getline(ss, b.date, ',');
        auto read_num = [&](double& out){
            if (!std::getline(ss, cell, ',')) return false;
            out = std::atof(cell.c_str());
            return true;
        };
        if (!read_num(b.open)) continue;
        read_num(b.high); read_num(b.low); read_num(b.close); read_num(b.volume);

        bars.push_back(b);
    }
    return bars;
}

