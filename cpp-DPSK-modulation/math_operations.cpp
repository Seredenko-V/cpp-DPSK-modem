#include "math_operations.h"

#include <cmath>
#include <string>
#include <stdexcept>

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

    int ConvertationBitsToInt(const std::vector<uint8_t>& bits) noexcept {
        return bits[0];
    }
} // namespace math
