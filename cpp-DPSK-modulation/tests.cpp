#include "math_operations.h"
#include "gray_code.h"
#include "dpsk_modulator.h"
#include "dpsk_demodulator.h"

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

        void TestDegreesToRadians() {
            assert(IsSameDouble(DegreesToRadians(270), 3 * M_PI / 2));
            assert(IsSameDouble(DegreesToRadians(180), M_PI));
            assert(IsSameDouble(DegreesToRadians(90), M_PI / 2));
            assert(IsSameDouble(DegreesToRadians(45), M_PI / 4));
            cerr << "math::TestDegreesToRadians has passed"s << endl;
        }

        void TestPhaseToRangeFrom0To2PI() {
            { // прибавление 2 * M_PI
                double phase = -M_PI / 2;
                PhaseToRangeFrom0To2PI(phase);
                assert(IsSameDouble(phase, 3 * M_PI / 2));
            }{ // вычитание 2 * M_PI
                double phase = 8 * M_PI / 2;
                PhaseToRangeFrom0To2PI(phase);
                assert(IsSameDouble(phase, 0));
            }{ // без изменений
                double phase = M_PI;
                PhaseToRangeFrom0To2PI(phase);
                assert(IsSameDouble(phase, M_PI));
            }{ // без изменений
                double phase = 0;
                PhaseToRangeFrom0To2PI(phase);
                assert(IsSameDouble(phase, 0));
            }
            cerr << "math::TestPhaseToRangeFrom0To2PI has passed"s << endl;
        }

        void TestFindNearestMultiple() {
            assert(FindNearestMultiple(1800, 19200, MultipleValue::MORE) == 1920);
            assert(FindNearestMultiple(1800, 19200, MultipleValue::LESS) == 1600);
            assert(FindNearestMultiple(15'000, 179'105, MultipleValue::MORE) == 35'821);
            assert(FindNearestMultiple(15'000, 179'105, MultipleValue::LESS) == 1585);
            assert(FindNearestMultiple(1900, 24'167, MultipleValue::MORE) == 2197);
            assert(FindNearestMultiple(1900, 24'167, MultipleValue::LESS) == 1859);
            assert(FindNearestMultiple(42, 100, MultipleValue::MORE) == 50);
            assert(FindNearestMultiple(99, 100, MultipleValue::LESS) == 50);
            assert(FindNearestMultiple(100, 100, MultipleValue::MORE) == 100);
            try {
                FindNearestMultiple(101, 100, MultipleValue::MORE);
                assert(false);
            } catch (const invalid_argument& /* except */) {}
            cerr << "math::TestFindNearestMultiple has passed"s << endl;
        }

        void TestGetGCD() {
            assert(GetGCD(41, 7) == 1);
            assert(GetGCD(0, 0) == 1);
            assert(GetGCD(15, 5) == 5);
            assert(GetGCD(12, 9) == 3);
            cerr << "math::TestGetGCD has passed"s << endl;
        }

        void TestGetValueAfterPoint() {
            // до 10 знака после запятой
            assert(GetValueAfterPoint(52151.124) == 124);
            assert(GetValueAfterPoint(0.1) == 1);
            assert(GetValueAfterPoint(5.0) == 0);
            assert(GetValueAfterPoint(9) == 0);
            assert(GetValueAfterPoint(3.115632) == 115632);
            assert(GetValueAfterPoint(3.000126) == 126);
            assert(GetValueAfterPoint(0.000001) == 1);
            assert(GetValueAfterPoint(0.0000000009) == 9);
            // с указанием нужного количества цифр после запятой
            assert(GetValueAfterPoint(3.115632, 3) == 116); // с округлением
            assert(GetValueAfterPoint(3.0000000000000018, 16) == 18);
            cerr << "math::TestGetValueAfterPoint has passed"s << endl;
        }

        void TestGetDigitsNumAfterPoint() {
            assert(GetDigitsNumAfterPoint(52151.124) == 3);
            assert(GetDigitsNumAfterPoint(2.000000) == 0);
            assert(GetDigitsNumAfterPoint(7) == 0);
            assert(GetDigitsNumAfterPoint(2.123456) == 6);
            assert(GetDigitsNumAfterPoint(2.000006) == 6);
            assert(GetDigitsNumAfterPoint(5.2) == 1);
            assert(GetDigitsNumAfterPoint(7.12345670) == 7);
            cerr << "math::TestGetDigitsNumAfterPoint has passed"s << endl;
        }

        void TestDecimalToOrdinary() {
            {
                OrdinaryFraction fract{1,1,2};
                assert(DecimalToOrdinary(1.5) == fract);
                assert(DecimalToOrdinary(3.0 / 2.0) == fract);
            }{
                OrdinaryFraction fract{0,3,4};
                assert(DecimalToOrdinary(0.75) == fract);
                assert(DecimalToOrdinary(9.0 / 12.0) == fract);
            }
            // чистые периодические дроби
            {
                OrdinaryFraction fract{0,1,7};
                assert(DecimalToOrdinary(1.0 / 7) == fract);
            }{
                OrdinaryFraction fract{0,1,3};
                assert(DecimalToOrdinary(1.0 / 3) == fract);
            }{
                OrdinaryFraction fract{0,5,11};
                assert(DecimalToOrdinary(5.0 / 11) == fract);
            }
            cerr << "math::TestDecimalToOrdinary has passed"s << endl;
        }

        void RunAllTests() {
            TestIsPowerOfTwo();
            TestExtractNumBitsFormValue();
            TestConvertationBinToDec();
            TestConvertationBitsToDecValues();
            TestIsSameDouble();
            TestDegreesToRadians();
            TestPhaseToRangeFrom0To2PI();
            TestFindNearestMultiple();
            TestGetGCD();
            TestGetValueAfterPoint();
            TestGetDigitsNumAfterPoint();
            TestDecimalToOrdinary();
            cerr << ">>> math::AllTests has passed <<<"s << endl;
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
            out << fixed;
            out.precision(20);
            for (const Type& element : vec) {
                out << element << endl;
            }
            return out;
        }

        vector<double> ReadSamplesFromFile(const string& name_file) {
            ifstream fin(name_file);
            assert(fin.is_open());
            vector<double> samples(1);
            while (fin >> *(samples.end() - 1)) {
                samples.emplace_back();
            }
            samples.pop_back();
            return samples;
        }

        vector<bool> ReadBitsFromFile(const string& name_file) {
            ifstream fin(name_file);
            assert(fin.is_open());
            vector<bool> bits;
            int tmp_value = 0;
            while (fin >> tmp_value) {
                bits.emplace_back(tmp_value);
            }
            return bits;
        }

        /// Запись временнЫх отсчетов (в секундах) модулированного сигнала
        void WriteSignalTime(string&& name_file, uint32_t num_samples_in_signal, uint32_t sampling_frequency) {
            ofstream time(name_file);
            for (uint32_t i = 0; i < num_samples_in_signal; ++i) {
                time << static_cast<double>(i) / sampling_frequency << endl;
            }
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

        void TestSetPhaseShift() {
            DPSKModulator modulator;
            { // сдвиг ОФМ-2 на 90 градусов
                modulator.SetPhaseShift(M_PI / 2);
                map<uint16_t, double> expected_phase_shifts{
                     {0, 90},
                     {1, 270}
                };
                assert(modulator.GetPhaseShifts() == expected_phase_shifts);
            }{ // сдвиг ОФМ-4 на 45 градусов
                modulator.SetPositionality(4).SetPhaseShift(M_PI / 4);
                map<uint16_t, double> expected_phase_shifts{
                     {0, 45},   // 00 -> 0 градусов
                     {1, 135},  // 01 -> 135 градусов
                     {3, 225},  // 11 -> 225 градусов
                     {2, 315}   // 10 -> 315 градусов
                };
                assert(modulator.GetPhaseShifts() == expected_phase_shifts);
            }
            cerr << "dpsk_mod::TestSetPhaseShift has passed"s << endl;
        }


        void CheckModulationMode(const vector<bool>& bits, DPSKModulator& modulator, const string& name_benchmark_file, const string& name_out_file) {
            vector<double> expected_mod_signal = ReadSamplesFromFile(name_benchmark_file);
            vector<double> real_mod_signal = modulator.Modulation(bits);
            ofstream fout(name_out_file);
            assert(fout.is_open());
            fout << real_mod_signal;
            assert(math::IsSameContainersWithDouble(real_mod_signal, expected_mod_signal));
        }

        const string kNamesPrefixOfBenchmarkFiles = "../files-with-sample-for-tests/samples_"s;
        const string kNamesPrefixOfOutputFiles = "real_"s;

        void TestClassicalModulation() {
            DPSKModulator modulator;
            constexpr uint32_t kCarrierFrequency = 1200u;
            constexpr uint32_t kSamplingFrequency = 19'200u;
            modulator.SetSamplingFrequency(kSamplingFrequency).SetCarrierFrequency(kCarrierFrequency);
            {
                const string kNameFile = "pos2.txt"s;
                modulator.SetPositionality(2).SetPhase(0);
                vector<bool> bits{1,1,1,1,1,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{ // проверка увеличения количества бит до кратности количеству бит в одном символе
                modulator.SetPositionality(4);
                vector<bool> bits{1, 0,1, 0,0, 1,1};
                vector<double> real_mod_signal = modulator.Modulation(bits);
                assert(real_mod_signal.size() == 5 * kSamplingFrequency / kCarrierFrequency);
            }{
                const string kNameFile = "pos4.txt"s;
                modulator.SetPositionality(4).SetPhase(0);
                vector<bool> bits{1,0, 1,1, 0,1, 0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{
                const string kNameFile = "pos8.txt"s;
                modulator.SetPositionality(8).SetPhase(0);
                vector<bool> bits{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }
            cerr << "dpsk_mod::TestClassicalModulation has passed"s << endl;
        }



        void TestModulationWithUseIntermediateFreq() {
            DPSKModulator modulator;
            constexpr uint32_t kCarrierFrequency = 1800u;
            constexpr uint32_t kIntermediateFrequency = 1200u;
            constexpr uint32_t kSamplingFrequency = 19'200u;
            modulator.SetSamplingFrequency(kSamplingFrequency).SetCarrierFrequency(kCarrierFrequency).SetIntermediateFrequency(kIntermediateFrequency);
            {
                const string kNameFile = "pos2_intermediate.txt"s;
                modulator.SetPositionality(2).SetPhase(0);
                vector<bool> bits{1,1,1,1,1,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{
                const string kNameFileOut = "pos2_intermediate_big.txt"s;
                const string kNameFileIn = "../files-with-sample-for-tests/send_bits_19200_1200_2pos.txt"s;
                modulator.SetPositionality(2).SetPhase(0);
                vector<bool> bits = ReadBitsFromFile(kNameFileIn);
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFileOut, kNamesPrefixOfOutputFiles + kNameFileOut);
            }{
                const string kNameFile = "pos4_intermediate.txt"s;
                modulator.SetPositionality(4).SetPhase(0);
                vector<bool> bits{1,0, 1,1, 0,1, 0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{
                const string kNameFile = "pos8_intermediate.txt"s;
                modulator.SetPositionality(8).SetPhase(0);
                vector<bool> bits{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }
            cerr << "dpsk_mod::TestModulationWithUseIntermediateFreq has passed"s << endl;
        }

        void TestConstellationShift() {
            DPSKModulator modulator;
            constexpr uint32_t kCarrierFrequency = 1800u;
            constexpr uint32_t kIntermediateFrequency = 1200u;
            constexpr uint32_t kSamplingFrequency = 19'200u;
            // без использования промежуточной частоты
            modulator.SetSamplingFrequency(kSamplingFrequency).SetCarrierFrequency(kIntermediateFrequency);
            {
                const string kNameFile = "pos2_shift_90.txt"s;
                modulator.SetPositionality(2).SetPhase(90);
                vector<bool> bits{1,1,1,1,1,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{ // по часовой стрелке (отрицательное значение фазы)
                const string kNameFile = "pos2_shift_minus_90.txt"s;
                modulator.SetPositionality(2).SetPhase(-90);
                vector<bool> bits{1,1,1,1,1,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{
                const string kNameFile = "pos4_shift_45.txt"s;
                modulator.SetPositionality(4).SetPhase(45);
                vector<bool> bits{1,0, 1,1, 0,1, 0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{
                const string kNameFile = "pos8_shift_22.5.txt"s;
                modulator.SetPositionality(8).SetPhase(22.5);
                vector<bool> bits{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }
            // с использованием промежуточной частоты
            modulator.SetCarrierFrequency(kCarrierFrequency).SetIntermediateFrequency(kIntermediateFrequency);
            {
                const string kNameFile = "pos2_shift_90_intermediate.txt"s;
                modulator.SetPositionality(2).SetPhase(90);
                vector<bool> bits{1,1,1,1,1,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{
                const string kNameFile = "pos4_shift_45_intermediate.txt"s;
                modulator.SetPositionality(4).SetPhase(45);
                vector<bool> bits{1,0, 1,1, 0,1, 0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }{
                const string kNameFile = "pos8_shift_22.5_intermediate.txt"s;
                modulator.SetPositionality(8).SetPhase(22.5);
                vector<bool> bits{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                CheckModulationMode(bits, modulator, kNamesPrefixOfBenchmarkFiles + kNameFile, kNamesPrefixOfOutputFiles + kNameFile);
            }
            cerr << "dpsk_mod::TestConstellationShift has passed"s << endl;
        }

        void RunAllTests() {
            TestDefaultConstructor();
            TestSetPositionality();
            TestSetPhaseShift();
            TestClassicalModulation();
            TestModulationWithUseIntermediateFreq();
            TestConstellationShift();
            cerr << ">>> dpsk_mod::AllTests has passed <<<"s << endl;
        }
    } // namespace tests
} // namespace dpsk_mod

namespace dpsk_demod {
    namespace tests {
        bool IsSameComplexDouble(const complex<double>& lhs, const complex<double>& rhs) {
            return math::IsSameDouble(lhs.real(), rhs.real()) && math::IsSameDouble(lhs.imag(), rhs.imag());
        }

        void TestExtractInPhaseAndQuadratureComponentsSymbol() {
            static constexpr uint32_t kSamplingFrequency = 50'000u;
            static constexpr uint32_t kCarrierFrequency = 1'000u;
            dpsk_mod::DPSKModulator modulator;
            modulator.SetSamplingFrequency(kSamplingFrequency).SetCarrierFrequency(kCarrierFrequency);
            DPSKDemodulator demodulator;
            demodulator.SetSamplingFrequency(kSamplingFrequency).SetSymbolSpeed(kCarrierFrequency).SetCarrierFrequency(kCarrierFrequency);
            modulator.SetModulationFunction(dpsk_mod::Sin);
            // ОФМ-2 без сдвига созвездия
            {
                vector<double> mod_bit = modulator.Modulation({0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI / 2) / 2, sin(M_PI / 2) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(3 * M_PI / 2) / 2, sin(3 * M_PI / 2) / 2}));
            }
            modulator.SetModulationFunction(dpsk_mod::Cos).SetPhase(0);
            {
                vector<double> mod_bit = modulator.Modulation({0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(0) / 2, sin(0) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI) / 2, sin(M_PI) / 2}));
            }
            // ОФМ-4 без сдвига созвездия
            modulator.SetPositionality(4).SetPhase(0);
            demodulator.SetPositionality(4).SetPhase(0);
            modulator.SetModulationFunction(dpsk_mod::Sin);
            { // модулирующая функция - синус
                vector<double> mod_bit = modulator.Modulation({1,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit.begin(), mod_bit.end()), inphase_quadrature_components));
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(0) / 2, sin(0) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI) / 2, sin(M_PI) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({0,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(3 * M_PI / 2) / 2, sin(3 * M_PI / 2) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(3 * M_PI / 2) / 2, sin(3 * M_PI / 2) / 2}));
            }
            modulator.SetPhase(0);
            demodulator.SetPhase(0);
            modulator.SetModulationFunction(dpsk_mod::Cos);
            { // модулирующая функция - косинус
                vector<double> mod_bit = modulator.Modulation({1,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(3 * M_PI / 2) / 2, sin(3 * M_PI / 2) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI / 2) / 2, sin(M_PI / 2) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({0,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI) / 2, sin(M_PI) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI) / 2, sin(M_PI) / 2}));
            }
            modulator.SetPhase(M_PI / 2);
            demodulator.SetPhase(0);
            { // со сдвигом созвездия на 90 градусов
                modulator.SetModulationFunction(dpsk_mod::Sin);
                vector<double> mod_bit = modulator.Modulation({1,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI / 2) / 2, sin(M_PI / 2) / 2}));
            }
            // ОФМ-8 без сдвига созвездия
            modulator.SetPositionality(8).SetPhase(0);
            demodulator.SetPositionality(8).SetPhase(0);
            modulator.SetModulationFunction(dpsk_mod::Sin);
            { // модулирующая функция - синус
                vector<double> mod_bit = modulator.Modulation({1,0,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(0) / 2, sin(0) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1,0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(7 * M_PI / 4) / 2, sin(7 * M_PI / 4) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1,1,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI) / 2, sin(M_PI) / 2}));
            }
            modulator.SetPhase(0);
            demodulator.SetPhase(0);
            modulator.SetModulationFunction(dpsk_mod::Cos);
            { // модулирующая функция - косинус
                vector<double> mod_bit = modulator.Modulation({1,0,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit.begin(), mod_bit.end()), inphase_quadrature_components));
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(3 * M_PI / 2) / 2, sin(3 * M_PI / 2) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1,0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit.begin(), mod_bit.end()), inphase_quadrature_components));
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(5 * M_PI / 4) / 2, sin(5 * M_PI / 4) / 2}));
            }{
                vector<double> mod_bit = modulator.Modulation({1,1,1}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                complex<double> inphase_quadrature_components = demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit);
                assert(IsSameComplexDouble(demodulator.ExtractInPhaseAndQuadratureComponentsSymbol(mod_bit.begin(), mod_bit.end()), inphase_quadrature_components));
                assert(IsSameComplexDouble(inphase_quadrature_components, {cos(M_PI / 2) / 2, sin(M_PI / 2) / 2}));
            }
            cerr << "dpsk_demod::TestExtractInPhaseAndQuadratureComponentsSymbol has passed"s << endl;
        }

        void TestExtractPhaseValue() {
            using namespace math;
            DPSKDemodulator demodulator;
            demodulator.SetSamplingFrequency(50'000u).SetSymbolSpeed(1000u).SetCarrierFrequency(1000u);
            // ОФМ-4
            assert(IsSameDouble(demodulator.ExtractPhaseValue({0.5, 0}), 0));
            assert(IsSameDouble(demodulator.ExtractPhaseValue({-0.5, 0}), M_PI));
            assert(IsSameDouble(demodulator.ExtractPhaseValue({0, 0.5}), M_PI / 2));
            assert(IsSameDouble(demodulator.ExtractPhaseValue({0, -0.5}), -M_PI / 2 + 2 * M_PI));
            // ОФМ-8
            assert(IsSameDouble(demodulator.ExtractPhaseValue({cos(M_PI / 4) / 2, sin(M_PI / 4) / 2}), M_PI / 4));
            assert(IsSameDouble(demodulator.ExtractPhaseValue({cos(3 * M_PI / 4) / 2, sin(3 * M_PI / 4) / 2}), 3 * M_PI / 4));
            assert(IsSameDouble(demodulator.ExtractPhaseValue({cos(-3 * M_PI / 4) / 2, sin(-3 * M_PI / 4) / 2}), -3 * M_PI / 4 + 2 * M_PI));
            assert(IsSameDouble(demodulator.ExtractPhaseValue({cos(-M_PI / 4) / 2, sin(-M_PI / 4) / 2}), -M_PI / 4 + 2 * M_PI));
            cerr << "dpsk_demod::TestExtractPhaseValue has passed"s << endl;
        }

        void TestFillSymbolsBounds() {
            { // ОФМ-2
                DPSKDemodulator demodulator(2);
                vector<double> expected_bounds{
                    M_PI / 2,
                    3 * M_PI / 2
                };
                assert(math::IsSameContainersWithDouble(expected_bounds, demodulator.GetBoundsSymbols()));
            }{ // ОФМ-8
                DPSKDemodulator demodulator(8);
                vector<double> expected_bounds{
                    M_PI / 8,
                    3 * M_PI / 8,
                    5 * M_PI / 8,
                    7 * M_PI / 8,
                    9 * M_PI / 8,
                    11 * M_PI / 8,
                    13 * M_PI / 8,
                    15 * M_PI / 8,
                };
                assert(math::IsSameContainersWithDouble(expected_bounds, demodulator.GetBoundsSymbols()));
            }
            cerr << "dpsk_demod::TestFillSymbolsBounds has passed"s << endl;
        }

        void TestFillSymbolsSequenceOnCircle() {
            { // ОФМ-2
                DPSKDemodulator demodulator(2);
                vector<uint32_t> expected_symbols{0, 1};
                assert(expected_symbols == demodulator.GetSymbolsSequenceOnCircle());
            }{ // ОФМ-8
                DPSKDemodulator demodulator(8);
                vector<uint32_t> expected_symbols{0,1,3,2,6,7,5,4};
                assert(expected_symbols == demodulator.GetSymbolsSequenceOnCircle());
            }
            cerr << "dpsk_demod::TestFillSymbolsSequenceOnCircle has passed"s << endl;
        }

        void TestDefineSymbol() {
            { // ОФМ-2
                DPSKDemodulator demodulator(2);
                assert(demodulator.DefineSymbol(0) == 0);
                assert(demodulator.DefineSymbol(M_PI / 4) == 0);
                assert(demodulator.DefineSymbol(M_PI / 2) == 1); // на границе в пользу следующего символа
                assert(demodulator.DefineSymbol(M_PI) == 1);
                assert(demodulator.DefineSymbol(3 * M_PI / 4) == 1);
                assert(demodulator.DefineSymbol(3 * M_PI / 2) == 0); // на границе в пользу следующего символа
            }{ // ОФМ-8
                DPSKDemodulator demodulator(8);
                assert(demodulator.DefineSymbol(0) == 0);
                assert(demodulator.DefineSymbol(3 * M_PI / 16) == 1);
                assert(demodulator.DefineSymbol(24 * M_PI / 16) == 5);
                assert(demodulator.DefineSymbol(2 * M_PI) == 0);
                assert(demodulator.DefineSymbol(31 * M_PI / 16) == 0);
            }
            cerr << "dpsk_demod::TestDefineSymbol has passed"s << endl;
        }

        void TestDemodulation() {
            dpsk_mod::DPSKModulator modulator;
            modulator.SetSamplingFrequency(50'000u).SetCarrierFrequency(1000u);
            DPSKDemodulator demodulator;
            demodulator.SetSamplingFrequency(50'000u).SetSymbolSpeed(1000u).SetCarrierFrequency(1000u);
            { // ОФМ-2 без сдвига созвездия
                vector<bool> bits{0,1,1,1,0,1};
                vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 1);
                vector<double> mod_bits = modulator.Modulation(bits);
                vector<uint32_t> demod_bits = demodulator.Demodulation(mod_bits);
                assert(symbols == demod_bits);
            }
            // ОФМ-4 без сдвига созвездия
            modulator.SetPositionality(4);
            demodulator.SetPositionality(4);
            { // без опорного символа в последовательности бит
                vector<bool> bits{1,0, 1,1, 0,1, 0,0};
                vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 2);
                vector<double> mod_bits = modulator.Modulation(bits);
                vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                assert(symbols == demod_symbols);
            }{ // с опорным символом в последовательности бит
                vector<bool> bits_without_pivot_symbol{1,0, 1,1, 0,1, 0,0};
                vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits_without_pivot_symbol, 2);
                vector<double> mod_bits = modulator.Modulation({0,0, 1,0, 1,1, 0,1, 0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                assert(symbols == demod_symbols);
            }
            // ОФМ-8 без сдвига созвездия
            modulator.SetPositionality(8);
            demodulator.SetPositionality(8);
            { // без опорного символа в последовательности бит
                vector<bool> bits{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 3);
                vector<double> mod_bits = modulator.Modulation(bits);
                vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                assert(symbols == demod_symbols);
            }{ // с опорным символом в последовательности бит
                vector<bool> bits_without_pivot_symbol{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits_without_pivot_symbol, 3);
                vector<double> mod_bits = modulator.Modulation({0,0,0, 1,0,1, 1,0,0, 1,1,1, 0,0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                assert(symbols == demod_symbols);
            }
            cerr << "dpsk_demod::TestDemodulation has passed"s << endl;
        }

        void TestSetPhaseShift() {
            DPSKDemodulator demodulator;
            { // сдвиг ОФМ-2 на 90 градусов
                demodulator.SetPhaseShift(M_PI / 2);
                vector<double> expected_phase_bounds{
                     M_PI,
                     2 * M_PI
                };
                assert(math::IsSameContainersWithDouble(demodulator.GetBoundsSymbols(), expected_phase_bounds));
            }{ // сдвиг ОФМ-4 на 45 градусов
                demodulator.SetPositionality(4).SetPhaseShift(M_PI / 4);
                vector<double> expected_phase_bounds{
                     M_PI / 2,
                     M_PI,
                     3 * M_PI / 2,
                     2 * M_PI
                };
                assert(math::IsSameContainersWithDouble(demodulator.GetBoundsSymbols(), expected_phase_bounds));
            }
            cerr << "dpsk_demod::TestSetPhaseShift has passed"s << endl;
        }

        void TestDemodulationWithPhaseShift() {
            static constexpr uint32_t kSamplingFrequency = 50'000u;
            static constexpr uint32_t kCarrierFrequency = 1'000u;
            dpsk_mod::DPSKModulator modulator;
            modulator.SetSamplingFrequency(kSamplingFrequency).SetCarrierFrequency(kCarrierFrequency);
            DPSKDemodulator demodulator;
            demodulator.SetSamplingFrequency(kSamplingFrequency).SetSymbolSpeed(kCarrierFrequency).SetCarrierFrequency(kCarrierFrequency);
            { // ОФМ-2
                modulator.SetPhaseShift(M_PI / 2);
                demodulator.SetPhaseShift(M_PI / 2);
                vector<bool> bits{0,1,1,1,0,1};
                vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 1);
                vector<double> mod_bits = modulator.Modulation(bits);
                vector<uint32_t> demod_bits = demodulator.Demodulation(mod_bits);
                assert(symbols == demod_bits);
            }{ // ОФМ-8
                modulator.SetPositionality(8).SetPhaseShift(M_PI / 8);
                demodulator.SetPositionality(8).SetPhaseShift(M_PI / 8);
                vector<bool> bits{1,0,1, 1,0,0, 1,1,1, 0,0,0};
                vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 3);
                vector<double> mod_bits = modulator.Modulation(bits);
                vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                assert(symbols == demod_symbols);
            }
            cerr << "dpsk_demod::TestDemodulationWithPhaseShift has passed"s << endl;
        }

        void TestDemodulationWithDecorrelationMatrix() {
            { // частота дискретизации 19200 Гц, несущая 1800 Гц, скорость 1200 символов/с
                constexpr uint32_t kSamplingFrequency = 19'200u;
                constexpr uint32_t kSymbolSpeed = 1'200u;
                constexpr uint32_t kCarrierFrequency = 1'800u;
                dpsk_mod::DPSKModulator modulator;
                modulator.SetSamplingFrequency(kSamplingFrequency).SetCarrierFrequency(kCarrierFrequency).SetIntermediateFrequency(kSymbolSpeed);
                DPSKDemodulator demodulator;
                demodulator.SetSamplingFrequency(kSamplingFrequency).SetSymbolSpeed(kSymbolSpeed).SetCarrierFrequency(kCarrierFrequency);
                { // ОФМ-2 без сдвига созвездия
                    vector<bool> bits{0,1,1,1,0,1};
                    vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 1);
                    vector<double> mod_bits = modulator.Modulation(bits);
                    ofstream fout("test_mod.txt"s);
                    fout << mod_bits;
                    fout.close();
                    vector<uint32_t> demod_bits = demodulator.Demodulation(mod_bits);
                    assert(symbols == demod_bits);
                }
                // ОФМ-4 без сдвига созвездия
                modulator.SetPositionality(4);
                demodulator.SetPositionality(4);
                { // без опорного символа в последовательности бит
                    vector<bool> bits{1,0, 1,1, 0,1, 0,0};
                    vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 2);
                    vector<double> mod_bits = modulator.Modulation(bits);
                    vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                    assert(symbols == demod_symbols);
                }{ // с опорным символом в последовательности бит
                    vector<bool> bits_without_pivot_symbol{1,0, 1,1, 0,1, 0,0};
                    vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits_without_pivot_symbol, 2);
                    vector<double> mod_bits = modulator.Modulation({0,0, 1,0, 1,1, 0,1, 0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                    vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                    assert(symbols == demod_symbols);
                }
            }{ // частота дискретизации 50000 Гц, несущая 3750 Гц, скорость 1000 символов/с
                constexpr uint32_t kSamplingFrequency = 50'000u;
                constexpr uint32_t kSymbolSpeed = 1'000u;
                constexpr uint32_t kCarrierFrequency = 3'750u;
                dpsk_mod::DPSKModulator modulator;
                modulator.SetSamplingFrequency(kSamplingFrequency).SetCarrierFrequency(kCarrierFrequency).SetIntermediateFrequency(kSymbolSpeed);
                DPSKDemodulator demodulator;
                demodulator.SetSamplingFrequency(kSamplingFrequency).SetSymbolSpeed(kSymbolSpeed).SetCarrierFrequency(kCarrierFrequency);
                { // ОФМ-2 без сдвига созвездия
                    vector<bool> bits{0,1,1,1,0,1};
                    vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 1);
                    vector<double> mod_bits = modulator.Modulation(bits);
                    ofstream fout("test_mod.txt"s);
                    fout << mod_bits;
                    fout.close();
                    vector<uint32_t> demod_bits = demodulator.Demodulation(mod_bits);
                    assert(symbols == demod_bits);
                }
                // ОФМ-4 без сдвига созвездия
                modulator.SetPositionality(4);
                demodulator.SetPositionality(4);
                { // без опорного символа в последовательности бит
                    vector<bool> bits{1,0, 1,1, 0,1, 0,0};
                    vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, 2);
                    vector<double> mod_bits = modulator.Modulation(bits);
                    vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                    assert(symbols == demod_symbols);
                }{ // с опорным символом в последовательности бит
                    vector<bool> bits_without_pivot_symbol{1,0, 1,1, 0,1, 0,0};
                    vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits_without_pivot_symbol, 2);
                    vector<double> mod_bits = modulator.Modulation({0,0, 1,0, 1,1, 0,1, 0,0}, dpsk_mod::PresencePivotSymbol::WITH_PIVOT);
                    vector<uint32_t> demod_symbols = demodulator.Demodulation(mod_bits);
                    assert(symbols == demod_symbols);
                }
            }
            cerr << "dpsk_demod::TestDemodulationWithDecorrelationMatrix has passed"s << endl;
        }

        void RunAllTests() {
            TestExtractInPhaseAndQuadratureComponentsSymbol();
            TestExtractPhaseValue();
            TestFillSymbolsBounds();
            TestFillSymbolsSequenceOnCircle();
            TestDefineSymbol();
            TestDemodulation();
            TestSetPhaseShift();
            TestDemodulationWithPhaseShift();
            TestDemodulationWithDecorrelationMatrix();
            cerr << ">>> dpsk_demod::AllTests has passed <<<"s << endl;
        }
    } // namespace tests
} // namespace dpsk_demod
