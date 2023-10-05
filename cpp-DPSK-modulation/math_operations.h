#pragma once

#include <cstdint>
#include <vector>

namespace math {
    /// Является ли число степенью двойки: N^2 = value. Сложность: O(1)
    bool IsPowerOfTwo(int value) noexcept;

    /// Извлечь количество бит, занимаемых числом. Сложность: O(1)
    int ExtractNumBitsFormValue(int value);

    /// Перевод из 2-ой в 10-ю систему счисления. Сложность: O(N)
    uint32_t ConvertationBinToDec(const std::vector<bool>& bits);

    namespace tests {
        void TestIsPowerOfTwo();
        void TestExtractNumBitsFormValue();
        void TestConvertationBinToDec();
        void RunAllTests();
    } // namespace tests
} // namespace math
