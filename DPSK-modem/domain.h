#pragma once

#include <cstdint>
#include <vector>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange() = default;
    explicit IteratorRange(Iterator begin_range, Iterator end_range)
        : begin_range_(begin_range), end_range_(end_range) {
    }
    Iterator begin() const {
        return begin_range_;
    }
    Iterator end() const {
        return end_range_;
    }
    std::size_t size() const {
        return distance(begin_range_, end_range_);
    }
private:
    Iterator begin_range_;
    Iterator end_range_;
};

void AddGausNoise(std::vector<double>& samples, double standard_deviation = 0, double average = 0);
