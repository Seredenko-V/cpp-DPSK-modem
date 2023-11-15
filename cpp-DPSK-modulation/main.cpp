#include "math_operations.h"
#include "gray_code.h"
#include "dpsk_modulator.h"
#include "dpsk_demodulator.h"

#include "matrix.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

using namespace std;

template <typename Type>
ostream& operator<<(ostream& out, const vector<Type>& vec) {
    out << fixed;
    out.precision(20);
    for (const Type& element : vec) {
        out << element << endl;
    }
    return out;
}

int main([[gnu::unused]] int argc, [[gnu::unused]] char *argv[]) {
//    math::tests::RunAllTests();
//    gray_code::tests::RunAllTests();
//    dpsk_mod::tests::RunAllTests();
    dpsk_demod::tests::RunAllTests();

//    {
//        dpsk_mod::DPSKModulator modulator;
//        modulator.SetCarrierFrequency(1000u).SetSamplingFrequency(50'000u);
//        vector<double> mod_signal(50);
//        double phase = 0;
//        modulator.ModulationOneSymbol(mod_signal.begin(), mod_signal.end(), 0, phase);
//        ofstream fout("test_sin.txt");
//        for (double sample : mod_signal) {
//            fout << sample << endl;
//        }
//    }{
//        dpsk_mod::DPSKModulator modulator;
//        modulator.SetCarrierFrequency(1000u).SetSamplingFrequency(50'000u).SetModulationFunction(dpsk_mod::Cos);
//        vector<double> mod_signal(50);
//        double phase = 0;
//        modulator.ModulationOneSymbol(mod_signal.begin(), mod_signal.end(), 0, phase);
//        ofstream fout("test_cos.txt");
//        for (double sample : mod_signal) {
//            fout << sample << endl;
//        }
//    }
//    {
//        dpsk_mod::DPSKModulator modulator;
//        modulator.SetCarrierFrequency(1000u).SetSamplingFrequency(50'000u);
//        vector<bool> bits{1,0,0,1,1,0,1};
//        vector<double> mod_signal = modulator.Modulation(bits);
//        ofstream fout("test.txt"s);
//        fout << mod_signal;
//    }
//    {
//        dpsk_mod::DPSKModulator modulator;
////        modulator.SetModulationFunction(dpsk_mod::Cos);
////        modulator.SetPositionality(4);
//        modulator.SetCarrierFrequency(1000).SetSamplingFrequency(50000u).SetPositionality(4);
//        vector<bool> bits{1,0, 1,1, 0,1, 0,0};
//        vector<double> mod_signal = modulator.Modulation(bits);
//        ofstream fout("test_mod.txt"s);
//        fout << mod_signal;
//    }




    dpsk_demod::DPSKDemodulator demodulator;
    demodulator.SetSamplingFrequency(19200).SetSymbolSpeed(1200).SetIntermediateFrequency(1200).SetCarrierFrequency(1800);


    const Matrix<double>& matrix = demodulator.GetDecorrelationMatrix();
    cout << matrix << endl;
    cout << endl;

//    Matrix<double> matr(3,3);
//    for (int i = 0; i < 3; ++i) {
//        for (int j = 0; j < 3; ++j) {
//            matr.put(i, j, j + i * 3);
//        }
//    }
//    matr.put(0,0, 1);
//    cout << matr << endl;
//    matr.invert();
//    cout << matr << endl;
//    cout << matr.get(2,2) << endl;
    return 0;
}
