#include "math_operations.h"
#include "gray_code.h"

#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

ostream& operator<<(ostream& out, uint8_t value) {
    out << static_cast<int>(value);
    return out;
}

template <typename Type>
ostream& operator<<(ostream& out, const vector<vector<Type>>& matrix) {
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            out << matrix[i][j];
        }
        out << endl;
    }
    return out;
}

namespace math {
    namespace tests {
        void TestIsPowerOfTwo() {
            assert(IsPowerOfTwo(1));
            assert(IsPowerOfTwo(2));
            assert(IsPowerOfTwo(16));
            assert(IsPowerOfTwo(4096));
            assert(!IsPowerOfTwo(-2));
            assert(!IsPowerOfTwo(5));
            assert(!IsPowerOfTwo(24));
            assert(!IsPowerOfTwo(4097));
            cerr << "math::TestIsPowerOfTwo has passed"s << endl;
        }

        void TestThrowsFromExtractNumBitsFormValue(int value) {
            try {
                ExtractNumBitsFormValue(value);
                assert(false);
            } catch (const invalid_argument& /*except*/) {
                // число должно быть положительным
            }
        }

        void TestExtractNumBitsFormValue() {
            TestThrowsFromExtractNumBitsFormValue(0);
            TestThrowsFromExtractNumBitsFormValue(-7);
            assert(ExtractNumBitsFormValue(1) == 1);
            assert(ExtractNumBitsFormValue(15) == 4);
            assert(ExtractNumBitsFormValue(4096) == 13);
            cerr << "math::TestExtractNumBitsFormValue has passed"s << endl;
        }

        void TestConvertationBinToDec() {
            assert(ConvertationBinToDec({0}) == 0);
            assert(ConvertationBinToDec({1}) == 1);
            assert(ConvertationBinToDec({1,1}) == 3);
            assert(ConvertationBinToDec({1,0,1,1,0,0,1,0}) == 178);
            assert(ConvertationBinToDec({1,1,0,1,0,0,0,1,1,0,1,1,1,1,1,0,1,0,0,0,1,1,0,0,0,1}) == 54983217);
            try {
                ConvertationBinToDec(vector<bool>(255, 1));
                assert(false);
            } catch (const out_of_range& /*except*/) {
                // последовательность бит не должна превышать 254
            }
            cerr << "math::TestConvertationBitsToInt has passed"s << endl;
        }

        void RunAllTests() {
            TestIsPowerOfTwo();
            TestExtractNumBitsFormValue();
            TestConvertationBinToDec();
            cerr << "math::AllTests has passed"s << endl;
        }
    } // namespace tests
} // namespace math


namespace gray_code {
    namespace tests {
        void TestThrowsFromMakeGrayCodes(int num_codes) {
            try {
                MakeGrayCodes(num_codes);
                assert(false);
            } catch (const invalid_argument& /*except*/) {
                // количество кодов Грея должно являться степенью двойки
            }
        }

        void TestMakeGrayCodes() {
            TestThrowsFromMakeGrayCodes(-2);
            TestThrowsFromMakeGrayCodes(0);
            TestThrowsFromMakeGrayCodes(21589);
            { // 1 бит
                vector<vector<uint8_t>> expected {{0}};
                assert(MakeGrayCodes(1) == expected);
            }{ // 1 бит
                vector<vector<uint8_t>> expected {{0},
                                                  {1}};
                assert(MakeGrayCodes(2) == expected);
            }{ // 2 бита
                vector<vector<uint8_t>> expected {{0,0},
                                                  {0,1},
                                                  {1,1},
                                                  {1,0}};
                assert(MakeGrayCodes(4) == expected);
            }{ // 4 бита
                vector<vector<uint8_t>> expected {{0,0,0,0},
                                                  {0,0,0,1},
                                                  {0,0,1,1},
                                                  {0,0,1,0},
                                                  {0,1,1,0},
                                                  {0,1,1,1},
                                                  {0,1,0,1},
                                                  {0,1,0,0},
                                                  {1,1,0,0},
                                                  {1,1,0,1},
                                                  {1,1,1,1},
                                                  {1,1,1,0},
                                                  {1,0,1,0},
                                                  {1,0,1,1},
                                                  {1,0,0,1},
                                                  {1,0,0,0}};
                assert(MakeGrayCodes(16) == expected);
            }
            cerr << "gray_code::TestMakeGrayCodes has passed"s << endl;
        }

        void RunAllTests() {
            TestMakeGrayCodes();
            cerr << "gray_code::AllTests has passed"s << endl;
        }
    } // namespace tests
} // namespace gray_code
