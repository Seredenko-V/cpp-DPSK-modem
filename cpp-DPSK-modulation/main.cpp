#include "math_operations.h"
#include "gray_code.h"
#include "dpsk_modulation.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

using namespace std;

int main([[gnu::unused]] int argc, [[gnu::unused]] char *argv[]) {
    math::tests::RunAllTests();
    gray_code::tests::RunAllTests();
    dpsk_mod::tests::RunAllTests();

    {
        dpsk_mod::DPSKModulator modulator;
        modulator.SetCarrierFrequency(1000u).SetSamplingFrequency(50'000u);
        vector<double> mod_signal(50);
        double phase = 0;
        modulator.ModulationOneSymbol(mod_signal.begin(), mod_signal.end(), 0, phase);
        ofstream fout("test_sin.txt");
        for (double sample : mod_signal) {
            fout << sample << endl;
        }
    }{
        dpsk_mod::DPSKModulator modulator;
        modulator.SetCarrierFrequency(1000u).SetSamplingFrequency(50'000u).SetModulationFunction(dpsk_mod::Cos);
        vector<double> mod_signal(50);
        double phase = 0;
        modulator.ModulationOneSymbol(mod_signal.begin(), mod_signal.end(), 0, phase);
        ofstream fout("test_cos.txt");
        for (double sample : mod_signal) {
            fout << sample << endl;
        }
    }
    return 0;
}
