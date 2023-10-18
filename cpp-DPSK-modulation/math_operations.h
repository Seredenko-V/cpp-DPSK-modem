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
    /// Перевод из 2-ой в 10-ю систему счисления. Сложность: O(2*N)
    uint32_t ConvertationBinToDec(std::vector<bool>::const_iterator left_bound, std::vector<bool>::const_iterator right_bound);

    /// Перевод последовательности бит в последовательность символов 10 СС. Сложность: O(N).
    /// Если bits.size() не кратен num_bits_per_symbol, то дописываются нули слева.
    std::vector<uint32_t> ConvertationBitsToDecValues(const std::vector<bool>& bits, int32_t num_bits_per_symbol);

    /// Сравнение двух double с заданной точностью. По умолчанию 1e-6. Сложность: O(1)
    bool IsSameDouble(double lhs, double rhs, double delta = 1e-6);

    template <typename Container>
    bool IsSameContainersWithDouble(const Container& lhs, const Container& rhs, double delta = 1e-6) {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        for (std::size_t i = 0; i < lhs.size(); ++i) {
            if (!IsSameDouble(lhs.at(i), rhs.at(i), delta)) {
                return false;
            }
        }
        return true;
    }

    /// Перевод углов в радианы
    double DegreesToRadians(double angle_degree);

    namespace tests {
        void TestIsPowerOfTwo();
        void TestExtractNumBitsFormValue();
        void TestConvertationBinToDec();
        void TestConvertationBitsToDecValues();
        void TestIsSameDouble();
        void TestDegreesToRadians();
        void RunAllTests();
    } // namespace tests
} // namespace math
