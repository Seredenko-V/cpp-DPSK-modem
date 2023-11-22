#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

namespace math {
    template <typename Type>
    using Matrix = std::vector<std::vector<Type>>;

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
    bool IsSameDouble(double lhs, double rhs, double delta = 1e-6) noexcept;

    /// Сравнение двух контейнеров с double. Сложность: O(1)
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

    /// Перенос значение фазы в пределы [0, 2 * M_PI). Сложность: O(phase / (2 * M_PI))
    void PhaseToRangeFrom0To2PI(double& phase) noexcept;

    /// Кратное число должно быть больше или меньше некоторого value
    enum class MultipleValue {
        MORE,
        LESS
    };

    /// Поиск ближайшего к value числа, которое делит без остатка multiple. Сложность: O(N)
    uint32_t FindNearestMultiple(uint32_t value, uint32_t divisible, MultipleValue is_more = MultipleValue::MORE);

    /// Наибольший общий делитель (НОД). Сложность: O(1)
    uint32_t GetGCD(int first, int second) noexcept;

    template <typename Number>
    std::string NumberToString(Number value, int num_digits_after_point = 10) {
        std::ostringstream tmp_stream;
        tmp_stream << std::fixed << std::setprecision(num_digits_after_point) << value;
        return tmp_stream.str();
    }

    /// Извлечь дробную часть числа до num_digits_after_point знака после запятой. Сложность: O(N)
    /// 62.125 -> 125; 0.04 -> 4.
    std::string GetTextAfterPoint(double value, int num_digits_after_point = 10);
    uint64_t GetValueAfterPoint(double value, int num_digits_after_point = 10);

    /// Получить количество цифр после запятой. Используется для нахождения знаменателя дроби. Сложность: O(N)
    /// 0.004 -> 3; 7.12345670 -> 7
    uint64_t GetDigitsNumAfterPoint(double value, int num_digits_after_point = 10);

    /// Определить период дроби. Сложность: O(2 * N)
    /// 1/3 -> 3; 1/7 -> 142857
    std::string GetTextPeriodFraction(double value, int num_digits_after_point = 16);

    struct OrdinaryFraction {
        uint32_t integer = 0u;
        uint32_t numerator = 0u;
        uint32_t denumerator = 1u;
        void Shorten(); // сократить дробь
    };

    inline bool operator==(const OrdinaryFraction& lhs, const OrdinaryFraction& rhs) {
        return lhs.integer == rhs.integer && lhs.numerator == rhs.numerator && lhs.denumerator == rhs.denumerator;
    }

    /// Перевод десятичной дроби в обыкновенную
    OrdinaryFraction DecimalToOrdinary(double value);

    namespace tests {
        void TestIsPowerOfTwo();
        void TestExtractNumBitsFormValue();
        void TestConvertationBinToDec();
        void TestConvertationBitsToDecValues();
        void TestIsSameDouble();
        void TestDegreesToRadians();
        void TestPhaseToRangeFrom0To2PI();
        void TestFindNearestMultiple();
        void TestGetGCD();
        void TestGetValueAfterPoint();
        void TestGetDigitsNumAfterPoint();
        void TestDecimalToOrdinary();
        void RunAllTests();
    } // namespace tests
} // namespace math
