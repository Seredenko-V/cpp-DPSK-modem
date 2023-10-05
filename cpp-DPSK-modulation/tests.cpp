#include "math_operations.h"
#include "gray_code.h"
#include "dpsk_modulation.h"

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
                vector<vector<bool>> expected {{0}};
                assert(MakeGrayCodes(1) == expected);
            }{ // 1 бит
                vector<vector<bool>> expected {{0},
                                                  {1}};
                assert(MakeGrayCodes(2) == expected);
            }{ // 2 бита
                vector<vector<bool>> expected {{0,0},
                                                  {0,1},
                                                  {1,1},
                                                  {1,0}};
                assert(MakeGrayCodes(4) == expected);
            }{ // 4 бита
                vector<vector<bool>> expected {{0,0,0,0},
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

namespace dpsk_mod {
    namespace tests {
        ostream& operator<<(ostream& out, const PhaseDifferences& phase_differences) {
            for (const pair<const pair<uint8_t, uint8_t>, double>& one_pair_of_elements : phase_differences) {
                out << '{' << static_cast<int>(one_pair_of_elements.first.first) << ',' << static_cast<int>(one_pair_of_elements.first.second) << "}, ";
                out << one_pair_of_elements.second << endl;
            }
            return out;
        }

        void TestDefaultConstructor() {
            // по умолчанию используется двухпозиционная ОФМ
            DPSKModulator modulator;
            assert(modulator.GetPositionality() == 2);
            // разность фаз между 0 и 1 составляет 180 градусов (PI)
            PhaseDifferences expected_phase_differences{{{0,1}, 180}};
            assert(modulator.GetPhaseDifferences() == expected_phase_differences);
            cerr << "dpsk_mod::TestDefaultConstructor has passed"s << endl;
        }

        void TestSetPositionality() {
            DPSKModulator modulator(2);
            {
                modulator.SetPositionality(4);
                assert(modulator.GetPositionality() == 4);
                PhaseDifferences expected_phase_differences{
                    {{0,1}, 90},  // 00, 01 -> 90 градусов
                    {{0,3}, 180}, // 00, 11 -> 180 градусов
                    {{0,2}, 270}, // 00, 10 -> 270 градусов

                    {{1,3}, 90},  // 01, 11 -> 90 градусов
                    {{1,2}, 180}, // 01, 10 -> 180 градусов

                    {{3,2}, 90}   // 11, 10 -> 90 градусов
                };
                assert(modulator.GetPhaseDifferences() == expected_phase_differences);
            }{
                modulator.SetPositionality(8);
                assert(modulator.GetPositionality() == 8);
                PhaseDifferences expected_phase_differences{
                    {{0,1}, 45},  // 000, 001 -> 45 градусов
                    {{0,3}, 90},  // 000, 011 -> 90 градусов
                    {{0,2}, 135}, // 000, 010 -> 135 градусов
                    {{0,6}, 180}, // 000, 110 -> 180 градусов
                    {{0,7}, 225}, // 000, 111 -> 225 градусов
                    {{0,5}, 270}, // 000, 101 -> 270 градусов
                    {{0,4}, 315}, // 000, 100 -> 315 градусов

                    {{1,3}, 45},  // 001, 011 -> 45 градусов
                    {{1,2}, 90},  // 001, 010 -> 90 градусов
                    {{1,6}, 135}, // 001, 110 -> 135 градусов
                    {{1,7}, 180}, // 001, 111 -> 180 градусов
                    {{1,5}, 225}, // 001, 101 -> 225 градусов
                    {{1,4}, 270}, // 001, 100 -> 270 градусов

                    {{3,2}, 45},  // 011, 010 -> 45 градусов
                    {{3,6}, 90},  // 011, 110 -> 90 градусов
                    {{3,7}, 135}, // 011, 111 -> 135 градусов
                    {{3,5}, 180}, // 011, 101 -> 180 градусов
                    {{3,4}, 225}, // 011, 100 -> 225 градусов

                    {{2,6}, 45},  // 010, 110 -> 45 градусов
                    {{2,7}, 90},  // 010, 111 -> 90 градусов
                    {{2,5}, 135}, // 010, 101 -> 135 градусов
                    {{2,4}, 180}, // 010, 100 -> 180 градусов

                    {{6,7}, 45},  // 110, 111 -> 45 градусов
                    {{6,5}, 90},  // 110, 101 -> 90 градусов
                    {{6,4}, 135}, // 110, 100 -> 135 градусов

                    {{7,5}, 45},  // 111, 101 -> 45 градусов
                    {{7,4}, 90},  // 111, 100 -> 90 градусов

                    {{5,4}, 45}   // 101, 100 -> 45 градусов
                };
                assert(modulator.GetPhaseDifferences() == expected_phase_differences);
            }
            cerr << "dpsk_mod::TestSetPositionality has passed"s << endl;
        }

        void RunAllTests() {
            TestDefaultConstructor();
            TestSetPositionality();
            cerr << "dpsk_mod::AllTests has passed"s << endl;
        }
    } // namespace tests
} // namespace dpsk_mod
