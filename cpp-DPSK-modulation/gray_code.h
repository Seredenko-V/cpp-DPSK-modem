#pragma once

#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace gray_code {
    /// Является ли число степенью двойки: N^2 = value. Сложность: O(1)
    bool IsPowerOfTwo(int value) noexcept;

    /// Извлечь количество бит, занимаемых числом. Сложность: O(1)
    int ExtractNumBitsFormValue(int value);

    std::vector<boost::dynamic_bitset<>> MakeGrayCodes(int num_codes);

    namespace tests {
        void TestIsPowerOfTwo();
        void TestExtractNumBitsFormValue();
        void RunAllTests();
    } // namespace tests
} // namespace gray_code
