#include "gray_code.h"

#include <stdexcept>
#include <string>
#include <cmath>

using namespace std;

namespace gray_code {
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

    vector<boost::dynamic_bitset<>> MakeGrayCodes(int num_codes) {
        // количество должно являться степенью двойки
        if (!IsPowerOfTwo(num_codes)) {
            throw invalid_argument("Number of codes is not a power of two."s);
        }
        vector<boost::dynamic_bitset<>> gray_codes(ExtractNumBitsFormValue(num_codes));

        return gray_codes;
    }


} // namespace gray_code
