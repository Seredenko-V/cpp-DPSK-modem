#include "math_operations.h"
#include "gray_code.h"
#include "dpsk_modulation.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

ostream& operator<<(ostream& out, uint8_t value) {
    out << static_cast<int>(value);
    return out;
}

template <typename Type>
ostream& operator<<(ostream& out, const vector<Type>& vec) {
    for (const Type& element : vec) {
        out << element;
    }
    return out;
}

template <typename Type>
ostream& operator<<(ostream& out, const vector<vector<Type>>& matrix) {
    for (const vector<Type>& line : matrix) {
        out << line;
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

        void TestConvertationBitsToDecValues() {
            { // 1 бит в символе
                const vector<bool> kBits{1,0,1,1,1,0,0,1};
                const vector<uint32_t> kSymbols{1,0,1,1,1,0,0,1};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 1));
            }{
                const vector<bool> kBits{1};
                const vector<uint32_t> kSymbols{1};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 1));
            }
            { // 2 бита в символе
                const vector<bool> kBits{1,0, 1,1, 1,0, 0,1};
                const vector<uint32_t> kSymbols{2,3,2,1};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 2));
            }{ // 2 бита в символе + дописывание одного бита для кратности 2
                const vector<bool> kBits{1, 0,1, 1,1, 0,0, 1,0};
                const vector<uint32_t> kSymbols{1,1,3,0,2};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 2));
            }{ // 2 бита в символе + дописывание одного бита для кратности 2
                const vector<bool> kBits{1};
                const vector<uint32_t> kSymbols{1};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 2));
            }
            { // 3 бита в символе + дописывание одного бита для кратности 3
                const vector<bool> kBits{1, 0,1,1, 1,0,1};
                const vector<uint32_t> kSymbols{1,3,5};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 3));
            }{
                const vector<bool> kBits{1};
                const vector<uint32_t> kSymbols{1};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 3));
            }
            { // 7 бит в символе
                const vector<bool> kBits{1,0,1, 1,0,1,1,1,0,1};
                const vector<uint32_t> kSymbols{5,93};
                assert(kSymbols == ConvertationBitsToDecValues(kBits, 7));
            }
            cerr << "math::TestConvertationBitsToDecValues has passed"s << endl;
        }

        void TestIsSameDouble() {
            assert(IsSameDouble(0.000001, 0.000001));
            assert(IsSameDouble(0.000001, 0.0000012156236)); // по умолчанию значения после 6 символа после запятой не учитываются
            assert(!IsSameDouble(10.02353, 10.02352));
            assert(IsSameDouble(15.02353, 15.0299999, 1e-2));
            cerr << "math::TestIsSameDouble has passed"s << endl;
        }

        void RunAllTests() {
            TestIsPowerOfTwo();
            TestExtractNumBitsFormValue();
            TestConvertationBinToDec();
            TestConvertationBitsToDecValues();
            TestIsSameDouble();
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
        ostream& operator<<(ostream& out, const map<uint16_t, double>& phase_shifts) {
            for (const pair<const uint16_t, double>& one_pair_of_elements : phase_shifts) {
                out << '{' << one_pair_of_elements.first << ',' << one_pair_of_elements.second << "}\n";
            }
            return out;
        }

        template <typename Type>
        ostream& operator<<(ostream& out, const vector<Type>& vec) {
            for (const Type& element : vec) {
                out << element << endl;
            }
            return out;
        }

        void TestDefaultConstructor() {
            // по умолчанию используется двухпозиционная ОФМ
            DPSKModulator modulator;
            assert(modulator.GetPositionality() == 2);
            map<uint16_t, double> expected_phase_shifts{{0, 0}, {1, 180}};
            assert(modulator.GetPhaseShifts() == expected_phase_shifts);
            cerr << "dpsk_mod::TestDefaultConstructor has passed"s << endl;
        }

        void TestSetPositionality() {
            DPSKModulator modulator(2);
            {
                modulator.SetPositionality(4);
                assert(modulator.GetPositionality() == 4);
                 map<uint16_t, double> expected_phase_shifts{
                     {0, 0},   // 00 -> 0 градусов
                     {1, 90},  // 01 -> 90 градусов
                     {3, 180}, // 11 -> 180 градусов
                     {2, 270}  // 10 -> 270 градусов
                };
                assert(modulator.GetPhaseShifts() == expected_phase_shifts);
            }{
                modulator.SetPositionality(8);
                assert(modulator.GetPositionality() == 8);
                map<uint16_t, double> expected_phase_shifts{
                    {0, 0},   // 000 -> 0 градусов
                    {1, 45},  // 001 -> 45 градусов
                    {3, 90},  // 011 -> 90 градусов
                    {2, 135}, // 010 -> 135 градусов
                    {6, 180}, // 110 -> 180 градусов
                    {7, 225}, // 111 -> 225 градусов
                    {5, 270}, // 101 -> 270 градусов
                    {4, 315}, // 100 -> 315 градусов
                };
                assert(modulator.GetPhaseShifts() == expected_phase_shifts);
            }
            cerr << "dpsk_mod::TestSetPositionality has passed"s << endl;
        }

        void TestGetDifferentPhaseBetweenSymbols() {
            DPSKModulator modulator;
//            { // ОФМ-2
//                modulator.SetPositionality(2);
//                constexpr double expected_difference = 180; // 180 градусов
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 1) == expected_difference);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(1, 0) == expected_difference);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 0) == 0);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(1, 1) == 0);
//            }{ // ОФМ-4
//                modulator.SetPositionality(4);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 1) == 90);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(1, 0) == 90);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 3) == 180);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 2) == 270);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(2, 0) == 270);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(1, 2) == 180);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(3, 2) == 90);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(1, 1) == 0);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(2, 2) == 0);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(3, 3) == 0);
//            }{ // ОФМ-8
//                modulator.SetPositionality(8);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 1) == 45);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 4) == 315);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 6) == 180);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(1, 2) == 90);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(5, 4) == 45);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(7, 4) == 90);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(5, 5) == 0);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(6, 6) == 0);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(7, 7) == 0);
//            }{ // ОФМ-16
//                modulator.SetPositionality(16);
//                assert(math::IsSameDouble(modulator.GetDifferentPhaseBetweenSymbols(0, 1), 22.5));
//                assert(math::IsSameDouble(modulator.GetDifferentPhaseBetweenSymbols(0, 12), 180));
//                assert(math::IsSameDouble(modulator.GetDifferentPhaseBetweenSymbols(0, 8), 337.5));
//            }{ // ОФМ-64
//                modulator.SetPositionality(64);
//                assert(math::IsSameDouble(modulator.GetDifferentPhaseBetweenSymbols(0, 1), 5.625));
//                const vector<vector<bool>> kCodes = gray_code::MakeGrayCodes(64);
//                const uint32_t kDecValueInCenterCodes = math::ConvertationBinToDec(kCodes.at(kCodes.size() / 2));
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, kDecValueInCenterCodes) == 180);
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, 32) == 354.375);
//            }{ // ОФМ-1024
//                modulator.SetPositionality(1024);
//                assert(math::IsSameDouble(modulator.GetDifferentPhaseBetweenSymbols(0, 1), 0.3515625));
//                const vector<vector<bool>> kCodes = gray_code::MakeGrayCodes(1024);
//                const uint32_t kDecValueInCenterCodes = math::ConvertationBinToDec(kCodes.at(kCodes.size() / 2));
//                assert(modulator.GetDifferentPhaseBetweenSymbols(0, kDecValueInCenterCodes) == 180);
//                assert(math::IsSameDouble(modulator.GetDifferentPhaseBetweenSymbols(0, 512), 359.648438));
//            }
//            cerr << "dpsk_mod::TestGetDifferentPhaseBetweenSymbols has passed"s << endl;
        }

        void TestModulationOnlyBits() {
            DPSKModulator modulator;
            constexpr uint32_t kCarrierFrequency = 1000u;
            constexpr uint32_t kSamplingFrequency = 10'000u;
            {
                ofstream fin("benchmark_modulation.txt"s);
                vector<bool> bits{1,0,1};
                const int kSizeOnePeriod = kSamplingFrequency / kCarrierFrequency;
                vector<double> mod_signal(bits.size() * kSizeOnePeriod);

                vector<double> value_phase_DPSK(bits.size());
                value_phase_DPSK[0] = M_PI * bits[0];
                for (size_t i = 1; i < value_phase_DPSK.size(); i++)
                {
                    value_phase_DPSK[i] = M_PI * bits[i] + value_phase_DPSK[i - 1];
                }

                double phase_shift = 0;
                for (size_t i = 0; i < bits.size(); ++i) {
                    phase_shift += 2 * M_PI * bits[i] / 2;
                    for (int j = 0; j < kSizeOnePeriod; ++j) {
                        //cout << "value_phase_DPSK[i]= "s << value_phase_DPSK[i] << endl;
                        mod_signal[j + i * kSizeOnePeriod] = sin(2 * M_PI * kCarrierFrequency / kSamplingFrequency * j + phase_shift);
                        fin << mod_signal[j + i * kSizeOnePeriod] << endl;
                        //cout << mod_signal[j + i * kSizeOnePeriod] << endl;
                    }
                }
            }


            modulator.SetCarrierFrequency(kCarrierFrequency).SetSamplingFrequency(kSamplingFrequency);
            { // проверка увеличения количества бит до кратности количеству бит в одном символе
                modulator.SetPositionality(2);
                vector<double> expected_mod_signal;
                vector<bool> bits{1,0,1,0,0,1,1};
                vector<double> real_mod_signal = modulator.Modulation(bits);
                assert(real_mod_signal.size() == 7 * kSamplingFrequency / kCarrierFrequency);
            }{ // проверка увеличения количества бит до кратности количеству бит в одном символе
                modulator.SetPositionality(4);
                vector<double> expected_mod_signal;
                vector<bool> bits{1,0,1,0,0,1,1};
                vector<double> real_mod_signal = modulator.Modulation(bits);
                //cout << real_mod_signal << endl;
                assert(real_mod_signal.size() == 4 * kSamplingFrequency / kCarrierFrequency);
            }


            modulator.SetCarrierFrequency(kCarrierFrequency).SetSamplingFrequency(50'000);
            {
                modulator.SetPositionality(2);
                modulator.SetPhase(0);
                vector<bool> bits{1,1,1,1,1,0};
                vector<double> real_mod_signal = modulator.Modulation(bits);
                ofstream fout("modulated_signal.txt"s);
                fout << fixed;
                fout.precision(6);
                fout << real_mod_signal << endl;
            }

            {
                modulator.SetPositionality(4);
                modulator.SetPhase(0);
                vector<bool> bits{1,0,1,1,0,1,0,0};
                vector<double> real_mod_signal = modulator.Modulation(bits);
                ofstream fout("modulated_signal_pos4.txt"s);
                fout << fixed;
                fout.precision(6);
                fout << real_mod_signal << endl;
            }

            {
                modulator.SetPositionality(8);
                modulator.SetPhase(0);
                vector<bool> bits{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                vector<double> real_mod_signal = modulator.Modulation(bits);
                ofstream fout("modulated_signal_pos8.txt"s);
                fout << fixed;
                fout.precision(6);
                fout << real_mod_signal << endl;
            }
            cerr << "dpsk_mod::TestModulationOnlyBits has passed"s << endl;
        }

        void RunAllTests() {
            TestDefaultConstructor();
            TestSetPositionality();
            TestGetDifferentPhaseBetweenSymbols();
            TestModulationOnlyBits();
            cerr << "dpsk_mod::AllTests has passed"s << endl;
        }
    } // namespace tests
} // namespace dpsk_mod
