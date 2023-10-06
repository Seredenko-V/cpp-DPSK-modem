#include "math_operations.h"

#include <cmath>
#include <string>
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

    uint32_t ConvertationBinToDec(const std::vector<bool>& bits) {
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

    bool IsSameDouble(double lhs, double rhs, double delta) {
        return abs(lhs - rhs) <= delta;
    }
} // namespace math
