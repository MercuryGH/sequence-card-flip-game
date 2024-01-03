#pragma once

#include <random>

// get public rng here
std::mt19937& get_rng() {
    static std::random_device dev;
    static std::mt19937 rng(dev());

    return rng;
}