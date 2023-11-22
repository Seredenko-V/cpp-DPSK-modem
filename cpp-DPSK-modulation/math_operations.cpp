#include "math_operations.h"

#include <cmath>
#include <stdexcept>

#include <iostream>
using namespace std;

namespace math {
    bool IsPowerOfTwo(int value) noexcept {
        return value && !(value & (value - 1));
    }

    int ExtractNumBitsFormValue(int value) {
        // число должно быть положительным
        if (value <= 0) {
            throw invalid_argument("Number is not positive."s);
        }
        return floor(log2(value)) + 1;
    }

    uint32_t ConvertationBinToDec(const vector<bool>& bits) {
        // Количество бит не должно превышать пределов uint8_t, иначе число в 10й СС будет слишком большим.
        // Например, если количество бит будет 224, то число в 10й СС равно 1.9255017829374E+67
        if (bits.size() >= 255) {
            throw out_of_range("Number bits over 255."s);
        }
        uint32_t dec_value = 0;
        uint8_t exponents_counter = static_cast<uint8_t>(bits.size() - 1); // счетчик показателя степени
        for (uint8_t bit : bits) {
            dec_value += bit * pow(2, exponents_counter--);
        }
        return dec_value;
    }

    uint32_t ConvertationBinToDec(vector<bool>::const_iterator left_bound, vector<bool>::const_iterator right_bound) {
        uint8_t exponents_counter = distance(left_bound, right_bound) - 1; // счетчик показателя степени
        uint32_t dec_value = 0;
        for (vector<bool>::const_iterator it = left_bound; it != right_bound; ++it) {
            dec_value += *it * pow(2, exponents_counter--);
        }
        return dec_value;
    }

    vector<uint32_t> ConvertationBitsToDecValues(const vector<bool>& bits, int32_t num_bits_per_symbol) {
        if (num_bits_per_symbol < 1) {
            throw invalid_argument("Number bits per symbol less or equal 1"s);
        }
        vector<uint32_t> symbols((bits.size() - 1) / num_bits_per_symbol + 1); // количество символов определяется целочисленным делением с округлением вверх
        uint16_t id_symbol = 0u;
        const uint32_t kNumBitsLastSend = bits.size() % num_bits_per_symbol; // количество бит в последней посылке, если общее количество бит не кратно количеству бит в символе
        uint32_t num_needed_bits = 0; // количество недостающих бит до кратности числу бит в одном символе

        // первый символ с дописыванием нулей слева
        if (kNumBitsLastSend > 0) {
            num_needed_bits = num_bits_per_symbol - kNumBitsLastSend;
            vector<bool> first_symbol_bits(num_bits_per_symbol);
            // по умолчанию все элементы false (0), поэтому заполняется информационными битами только правая часть
            for (uint32_t i = 0; i < num_needed_bits; ++i) {
                first_symbol_bits[i + num_needed_bits] = bits[i];
            }
            symbols[id_symbol++] = ConvertationBinToDec(first_symbol_bits);
        }
        for (vector<bool>::const_iterator left_bound = bits.cbegin() + kNumBitsLastSend; left_bound != bits.cend(); left_bound += num_bits_per_symbol) {
            vector<bool>::const_iterator right_bound = left_bound + num_bits_per_symbol;
            symbols[id_symbol++] = ConvertationBinToDec(left_bound, right_bound);
        }
        return symbols;
    }

    bool IsSameDouble(double lhs, double rhs, double delta) noexcept {
        return abs(lhs - rhs) <= delta;
    }

    double DegreesToRadians(double angle_degree) noexcept {
        return angle_degree * M_PI / 180;
    }

    void PhaseToRangeFrom0To2PI(double& phase) noexcept {
        while (phase >= 2 * M_PI) {
            phase -= 2 * M_PI;
        }
        while (phase < 0) {
            phase += 2 * M_PI;
        }
    }

    uint32_t FindNearestMultiple(uint32_t value, uint32_t divisible, MultipleValue is_more) {
        if (value == 0) {
            return 0;
        }
        if (divisible % value == 0) {
            return value;
        }
        int32_t int_part = divisible / value;
        if (int_part == 0) {
            throw invalid_argument("Rounding up is not possible"s);
        }

        if (is_more == MultipleValue::MORE) {
            while (divisible % int_part && int_part > 0) {
                --int_part;
            }
        } else {
            while (divisible % ++int_part);
        }
        return divisible / int_part;
    }

    uint32_t GetGCD(int first, int second) noexcept {
        if (first == 0 || second == 0) {
            return 1;
        }
        if (first < second) {
            swap(first, second);
        }
        while (first % second != 0) {
            first = first % second;
            swap(first, second);
        }
        return std::abs(second);
    }

    string GetTextAfterPoint(double value, int num_digits_after_point) {
        if (num_digits_after_point < 0) {
            throw invalid_argument("Number digits after decimal point is negative."s);
        }
        if (num_digits_after_point > 16) {
            throw invalid_argument("Too many digits after decimal point. "s + to_string(num_digits_after_point) + '>' + "16."s);
        }
        string num_str = NumberToString(value, num_digits_after_point);
        size_t point_pos = num_str.find('.');
        size_t first_no_zero_digit = num_str.find_first_not_of('0', point_pos + 1);
        if (first_no_zero_digit == string::npos) {
            return "0"s;
        }
        size_t last_no_zero_digit = num_str.find_last_not_of('0');
        return num_str.substr(first_no_zero_digit, last_no_zero_digit - first_no_zero_digit + 1);
    }

    uint64_t GetValueAfterPoint(double value, int num_digits_after_point) {
        return stoi(GetTextAfterPoint(value, num_digits_after_point));
    }

    uint64_t GetDigitsNumAfterPoint(double value, int num_digits_after_point) {
        string num_str = NumberToString(value, num_digits_after_point);
        size_t point_pos = num_str.find('.');
        size_t last_no_zero_digit = num_str.find_last_not_of('0');
        return last_no_zero_digit - point_pos;
    }

    string GetTextPeriodFraction(double value, int num_digits_after_point) {
        string num_str = GetTextAfterPoint(value, num_digits_after_point);
        if (num_str.size() == 1) {
            return {};
        }
        string one_period(num_str.size() / 2, num_str.front());
        // накопление периода
        for (size_t i = 1; i < num_str.size() / 2; ++i) {
            if (num_str[i] != num_str[0]) {
                one_period[i] = num_str[i];
            } else {
                one_period.resize(i);
                break;
            }
        }
        // проверка наличия периодичности
        for (size_t i = 0; i < num_str.size(); ++i) {
            if (num_str[i] != one_period[i % one_period.size()]) {
                one_period[i] = num_str[i];
                return {};
            }
        }
        return one_period;
    }

    void OrdinaryFraction::Shorten() {
        uint32_t gcd = GetGCD(numerator, denumerator);
        numerator /= gcd;
        denumerator /= gcd;
    }

    OrdinaryFraction DecimalToOrdinary(double value) {
        OrdinaryFraction ord_fraction;
        ord_fraction.integer = value;

        string one_period_str = GetTextPeriodFraction(value);
        if (!one_period_str.empty()) {
            // Объяснение числа 9 -> https://intmag24.ru/dlya-shkolnikov/beskonechnye-periodicheskie-drobi/?ysclid=lp89t7sfx595650252
            constexpr char kMagicNine = '9';
            string denumenator_str(one_period_str.size(), kMagicNine);
            ord_fraction.denumerator = stoi(denumenator_str);
            ord_fraction.numerator = stoi(one_period_str);
        } else {
            ord_fraction.denumerator = pow(10, GetDigitsNumAfterPoint(value));
            ord_fraction.numerator = GetValueAfterPoint(value);
        }
        ord_fraction.Shorten();
        return ord_fraction;
    }
} // namespace math
