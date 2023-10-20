#include "dpsk_modulation.h"
#include "math_operations.h"
#include "gray_code.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cassert>

#include <iostream>

using namespace std;

namespace dpsk_mod {
    DPSKModulator::DPSKModulator(int positionality) {
        SetPositionality(positionality);
    }

    DPSKModulator& DPSKModulator::SetPositionality(int positionality) {
        // чтобы не заполнять повторно словарь с разностью фаз
        if (positionality_ != positionality) {
            if (!math::IsPowerOfTwo(positionality)) {
                throw invalid_argument("Positionality is not a power of two."s);
            }
            positionality_ = positionality;
            FillPhaseShifts();
        }
        return *this;
    }

    uint16_t DPSKModulator::GetPositionality() const noexcept {
        return positionality_;
    }

    DPSKModulator& DPSKModulator::SetCarrierFrequency(int carrier_frequency) {
        carrier_frequency_ = carrier_frequency;
        return *this;
    }

    uint32_t DPSKModulator::GetCarrierFrequency() const noexcept {
        return carrier_frequency_;
    }

    DPSKModulator& DPSKModulator::SetSamplingFrequency(int sampling_frequency) {
        sampling_frequency_ = sampling_frequency;
        return *this;
    }

    uint32_t DPSKModulator::GetSamplingFrequency() const noexcept {
        return sampling_frequency_;
    }

    DPSKModulator& DPSKModulator::SetPhase(double new_phase) {
        phase_ = new_phase;
        return *this;
    }

    double DPSKModulator::GetPhase() const noexcept {
        return phase_;
    }

    const map<uint16_t, double>& DPSKModulator::GetPhaseShifts() const noexcept {
        return phase_shifts_;
    }

    void DPSKModulator::FillPhaseShifts() {
        phase_shifts_.clear();
        static constexpr double kTotalAngle = 360; // количество градусов на окружности
        const double kStepPhase = kTotalAngle / positionality_;
        vector<vector<bool>> grey_codes = gray_code::MakeGrayCodes(positionality_);
        double current_phase = 0;
        for (uint16_t i = 0; i < positionality_; ++i) {
            assert(current_phase < kTotalAngle);
            phase_shifts_.emplace(math::ConvertationBinToDec(grey_codes[i]), current_phase);
            current_phase += kStepPhase;
        }
    }

    void DPSKModulator::ModulationOneSymbol(vector<double>::iterator begin_samples, vector<double>::iterator end_samples, uint16_t current_symbol, double& phase) {
        const double kCyclicFrequency = 2 * M_PI * carrier_frequency_; // циклическая частота
        const double kTimeStepBetweenSamples = 1.0 / sampling_frequency_; // шаг дискретизации во временной области
        const double kFixedCoefficient = kCyclicFrequency * kTimeStepBetweenSamples; // коэффициент, не изменяющийся в процессе дискретизации
        const double kPhaseDifferent = phase_shifts_.find(current_symbol)->second;
        int count = 0;
//        phase += 2 * M_PI * current_symbol / positionality_; // возникают трудности при количестве позиции ОФМ больше 2
        phase += math::DegreesToRadians(kPhaseDifferent);
        for (vector<double>::iterator it = begin_samples; it != end_samples; ++it) {
            *it = amplitude_ * sin(kFixedCoefficient * count++ + phase);
        }
    }

    void DPSKModulator::ClassicalModulation(const vector<uint32_t>& symbols, vector<double>& modulated_signal, uint16_t num_samples_in_symbol) {
        const double kCyclicFrequency = 2 * M_PI * carrier_frequency_; // циклическая частота
        const double kTimeStepBetweenSamples = 1.0 / sampling_frequency_; // шаг дискретизации во временной области
        const double kCyclicFrequencyCoefficient = kCyclicFrequency * kTimeStepBetweenSamples; // коэффициент, не изменяющийся в процессе дискретизации

        for (size_t symbol_id = 0; symbol_id < symbols.size(); ++symbol_id) {
            phase_ += math::DegreesToRadians(phase_shifts_.find(symbols[symbol_id])->second);
            for (uint16_t sample_id = 0; sample_id < num_samples_in_symbol; ++sample_id) {
                modulated_signal[sample_id + symbol_id * num_samples_in_symbol] = amplitude_ * sin(kCyclicFrequencyCoefficient * sample_id + phase_);
            }
        }
    }

    void DPSKModulator::ModulationWithUseIntermediateFreq(const vector<uint32_t>& symbols, vector<double>& modulated_signal, uint16_t num_samples_in_symbol) {
        const double kTimeStepBetweenSamples = 1.0 / sampling_frequency_; // шаг дискретизации во временной области
        const double kCyclicFrequency = 2 * M_PI * carrier_frequency_; // циклическая частота
        const double kIntermediateCyclicFrequency = 2 * M_PI * intermediate_carrier_frequency_; // циклическая промежуточная частота

        const double kDiffrerenceCyclicFrequency = kIntermediateCyclicFrequency - kCyclicFrequency;
        const double kDiffrerenceCyclicFrequencyCoefficient = kDiffrerenceCyclicFrequency * kTimeStepBetweenSamples;
        const double kIntermediateCyclicFrequencyCoefficient = kIntermediateCyclicFrequency * kTimeStepBetweenSamples;

        for (size_t symbol_id = 0; symbol_id < symbols.size(); ++symbol_id) {
            phase_ += math::DegreesToRadians(phase_shifts_.find(symbols[symbol_id])->second);
            for (uint16_t sample_id = 0; sample_id < num_samples_in_symbol; ++sample_id) {
                double sample_of_signal_on_intermediate_frequency = amplitude_ * sin(kIntermediateCyclicFrequencyCoefficient * sample_id + phase_);
                double sample_of_orthogonal_signal = amplitude_ * -cos(kIntermediateCyclicFrequencyCoefficient * sample_id + phase_);
//                double sample_of_needed_signal = sample_of_signal_on_intermediate_frequency * cos(kDiffrerenceCyclicFrequencyCoefficient * sample_id) +
//                        sample_of_orthogonal_signal * sin(kDiffrerenceCyclicFrequencyCoefficient * sample_id);
                modulated_signal[sample_id + symbol_id * num_samples_in_symbol] = sample_of_signal_on_intermediate_frequency *
                        cos(kDiffrerenceCyclicFrequencyCoefficient * sample_id) + sample_of_orthogonal_signal * sin(kDiffrerenceCyclicFrequencyCoefficient * sample_id);
            }
        }
    }

    vector<double> DPSKModulator::Modulation(const vector<bool>& bits) {
        // частота дискретизации должна быть кратна несущей частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % carrier_frequency_ /*&& intermediate_frequency_ == 0*/) {
            throw invalid_argument("The sampling frequency must be a multiple of the carrier frequency so that there is an integer number of samples in one period."s);
        }
        const uint32_t kNumBitsInOneSymbol = log2(positionality_); // количество бит в одном символе
        vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, kNumBitsInOneSymbol);
        const uint16_t kNumSpamlesInElementarySignal = sampling_frequency_ / carrier_frequency_; // количество отсчетов в одном модулированном символе
        vector<double> modulated_signal(kNumSpamlesInElementarySignal * symbols.size());

//        for (size_t symbol_id = 0; symbol_id < symbols.size(); ++symbol_id) {
//            vector<double>::iterator left_bound = modulated_signal.begin() + symbol_id * kNumSpamlesInElementarySignal;
//            vector<double>::iterator right_bound = modulated_signal.begin() + (symbol_id + 1) * kNumSpamlesInElementarySignal;
//            ModulationOneSymbol(left_bound, right_bound, symbols[symbol_id], phase_);
//        }

        //ClassicalModulation(symbols, modulated_signal, kNumSpamlesInElementarySignal);
        ModulationWithUseIntermediateFreq(symbols, modulated_signal, kNumSpamlesInElementarySignal);
        return modulated_signal;
    }

//    vector<double> DPSKModulator::Modulation(const vector<bool>& bits) {

//    }

    vector<double> DPSKModulator::Modulation(const vector<bool>& bits, int positionality) {
        SetPositionality(positionality);
        return Modulation(bits);
    }
} // namespace dpsk_mod
