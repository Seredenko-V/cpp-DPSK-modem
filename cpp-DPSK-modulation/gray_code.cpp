#include "gray_code.h"

#include <stdexcept>
#include <string>
#include <cmath>

#include <iostream>

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

    uint64_t Factorial(int32_t value) {
        // факториал не существует для отрицательных чисел
        if (value < 0) {
            throw out_of_range("Attempt to calculate the factorial of a negative number"s);
        }
        uint64_t result = 1;
        if (value <= 1) {
            return result;
        }
        for (uint16_t i = 2; i <= value; ++i) {
            result *= i;
        }
        return result;
    }

    uint32_t CalculateNumberCombinations(int32_t n, int32_t k) {
        if (n < k) {
            throw logic_error("k exceeds n"s);
        }
        if (n < 0) {
            throw out_of_range("n < 0"s);
        }
        if (k < 0) {
            throw out_of_range("k < 0"s);
        }
        if (n == 1) {
            return 1;
        }

        // чтобы избежать переполнения при вычислении факториала
        uint16_t difference_n_k = n - k;
        // сокращение дроби
        uint64_t numerator = 1;
        for (uint16_t value = difference_n_k + 1; value <= n; ++value) {
            numerator *= value;
        }
        return numerator / Factorial(k);
    }

    vector<vector<uint8_t>> MakeGrayCodes(int num_codes) {
        // количество должно являться степенью двойки
        if (!IsPowerOfTwo(num_codes)) {
            throw invalid_argument("Number of codes is not a power of two."s);
        }
        // возвращается просто 0
        if (num_codes == 1) {
            return {{0}};
        }
        const int kLengthGrayCodes = log2(num_codes);
        cout << "kLengthGrayCodes=" << kLengthGrayCodes << endl;
        vector<vector<uint8_t>> gray_codes(num_codes, vector<uint8_t>(ExtractNumBitsFormValue(kLengthGrayCodes), 0));
        gray_codes[1][kLengthGrayCodes - 1] = 1; // устанавливается второй код Грея 01. Первый - 00

//        for (int code_id = 2; code_id < num_codes; ++code_id) {
//            for (int bit_id = 0; bit_id < kLengthGrayCodes; ++bit_id) {

//            }
//        }
        return gray_codes;
    }


} // namespace gray_code
