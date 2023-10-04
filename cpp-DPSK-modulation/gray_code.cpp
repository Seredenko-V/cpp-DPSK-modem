#include "gray_code.h"

#include <stdexcept>
#include <string>
#include <cmath>
#include <cassert>

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
        const int kLengthGrayCodes = log2(num_codes); // количество бит в кодах Грея
        vector<vector<uint8_t>> gray_codes(num_codes, vector<uint8_t>(kLengthGrayCodes, 0));
        gray_codes[1][kLengthGrayCodes - 1] = 1; // устанавливается второй код Грея 01. Первый - 00

        // по блокам, начиная с 3-го. Первые два - крайний случай
        for (int block = 2; block < num_codes; block *= 2) {
            const int kPosNewSeniorDigit = kLengthGrayCodes - log2(block) - 1; // позиция нового старшего разряда, в который записывается единица
            int offset_back = 1; // смещение назад по номерам кодов Грея

            // Обработка кодов внутри блока. Количество кодов в одном блоке равно удвоенному номеру блока (по кодам внутри одного блока)
            for (int code_id = block; code_id < block * 2; ++code_id) {
                gray_codes[code_id][kPosNewSeniorDigit] = 1;

                // Побитная обработка каждого кода (по самому коду)
                for (int bit_id = kPosNewSeniorDigit + 1; bit_id < kLengthGrayCodes; ++bit_id) {
                    assert(offset_back % 2); //смещение назад всегда должно быть нечетным
                    gray_codes[code_id][bit_id] = gray_codes[code_id - offset_back][bit_id];
                }
                offset_back += 2;
            }
        }
        return gray_codes;
    }


} // namespace gray_code
