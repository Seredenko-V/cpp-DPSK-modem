#pragma once

#include <vector>
#include <cstdint>

namespace gray_code {
    /// Сгенерировать num_codes кодов Грея. num_codes должно быть степенью двойки. Сложность O(N * log2(N))
    /// Не используется boost::dynamic_bitset<> т.к. он не имеет конструктора, принимающего список инициализации
    std::vector<std::vector<uint8_t>> MakeGrayCodes(int num_codes);

    namespace tests {
        void TestMakeGrayCodes();
        void RunAllTests();
    } // namespace tests
} // namespace gray_code
