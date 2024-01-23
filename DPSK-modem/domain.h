#pragma once

#include <cstdint>
#include <vector>
#include <random>

namespace domain {
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

    template <typename Iterator>
    void AddGausNoise(Iterator begin_it, Iterator end_it, double standard_deviation = 0, double average = 0) {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        std::normal_distribution<double> dist(average, standard_deviation);
        for (Iterator it = begin_it; it != end_it; ++it) {
            *it += dist(mt);
        }
    }

    namespace tests {
        void TestAddGausNoiseToVector();
        void TestAddGausNoiseToIteratorRange();
        void RunAllTests();
    } // namespace tests
} // namespace domain
