#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

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

    /// Проверка на равенство двух double с заданной точностью. По умолчанию 1e-6. Сложность: O(1)
    bool IsSameDouble(double lhs, double rhs, double delta = 1e-6) noexcept;

    /// Проверка на равенство двух контейнеров с double. Сложность: O(N)
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

    /// Перевод углов в радианы. Сложность: O(1)
    double DegreesToRadians(double angle_degree) noexcept;

    /// Перенос значения фазы в пределы [0, 2 * M_PI). Сложность: O(phase / (2 * M_PI))
    void PhaseToRangeFrom0To2PI(double& phase) noexcept;

    /// Кратное число должно быть больше или меньше некоторого value
    enum class MultipleValue {
        MORE,
        LESS
    };

    /// Поиск ближайшего к value числа, которое делит без остатка multiple. Сложность: O(N)
    uint32_t FindNearestMultiple(uint32_t value, uint32_t divisible, MultipleValue is_more = MultipleValue::MORE);

    namespace tests {
        void TestIsPowerOfTwo();
        void TestExtractNumBitsFormValue();
        void TestConvertationBinToDec();
        void TestConvertationBitsToDecValues();
        void TestIsSameDouble();
        void TestDegreesToRadians();
        void TestPhaseToRangeFrom0To2PI();
        void TestFindNearestMultiple();
        void RunAllTests();
    } // namespace tests
} // namespace math
