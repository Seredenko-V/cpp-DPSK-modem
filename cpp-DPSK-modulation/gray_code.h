#pragma once

#include <vector>
#include <cstdint>

namespace gray_code {
    /// Является ли число степенью двойки: N^2 = value. Сложность: O(1)
    bool IsPowerOfTwo(int value) noexcept;

    /// Извлечь количество бит, занимаемых числом. Сложность: O(1)
    int ExtractNumBitsFormValue(int value);

    /// Вычисление факториала числа. Сложность: O(value)
    uint64_t Factorial(int32_t value);

    /// Вычисление количества сочетаний (С из n по k). Сложность: O(k)
    uint32_t CalculateNumberCombinations(int32_t n, int32_t k);

    /// Сгенерировать num_codes кодов Грея. num_codes должно быть степенью двойки.
    /// Не используется boost::dynamic_bitset<> т.к. он не имеет конструктора, принимающего список инициализации
    std::vector<std::vector<uint8_t>> MakeGrayCodes(int num_codes);

    namespace tests {
        void TestIsPowerOfTwo();
        void TestExtractNumBitsFormValue();
        void TestFactorial();
        void TestCalculateNumberCombinations();
        void TestMakeGrayCodes();
        void RunAllTests();
    } // namespace tests
} // namespace gray_code
