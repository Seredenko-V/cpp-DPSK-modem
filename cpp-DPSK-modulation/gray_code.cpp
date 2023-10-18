#include "gray_code.h"
#include "math_operations.h"

#include <stdexcept>
#include <string>
#include <cmath>
#include <cassert>

using namespace std;

namespace gray_code {
    // Можно реализовать в виде класса с использованием паттерна "Singleton".
    // Это позволит уменьшить сложность алгоритма за счет кэширования уже сгенерированных кодов
    // и их последующего использования для генерации кодов больших разрядностей
    vector<vector<bool>> MakeGrayCodes(int num_codes) {
        // количество должно являться степенью двойки
        if (!math::IsPowerOfTwo(num_codes)) {
            throw invalid_argument("Number of codes is not a power of two."s);
        }
        // возвращается просто 0
        if (num_codes == 1) {
            return {{0}};
        }
        const int kLengthGrayCodes = log2(num_codes); // количество бит в кодах Грея
        vector<vector<bool>> gray_codes(num_codes, vector<bool>(kLengthGrayCodes, 0));

        // по блокам, начиная с 1-го. Первый - крайний случай
        for (int block = 1; block < num_codes; block *= 2) {
            const int kPosNewSeniorDigit = kLengthGrayCodes - log2(block) - 1; // позиция нового старшего разряда, в который записывается единица
            int offset_back = 1; // смещение назад по номерам кодов Грея

            // Обработка кодов внутри блока. Количество кодов в одном блоке равно удвоенному номеру блока (по кодам внутри одного блока)
            for (int code_id = block; code_id < block * 2; ++code_id) {
                gray_codes[code_id][kPosNewSeniorDigit] = 1;
                assert(offset_back % 2); //смещение назад всегда должно быть нечетным

                // Побитная обработка каждого кода (по самому коду)
                for (int bit_id = kPosNewSeniorDigit + 1; bit_id < kLengthGrayCodes; ++bit_id) {
                    gray_codes[code_id][bit_id] = gray_codes[code_id - offset_back][bit_id];
                }
                offset_back += 2;
            }
        }
        return gray_codes;
    }
} // namespace gray_code
