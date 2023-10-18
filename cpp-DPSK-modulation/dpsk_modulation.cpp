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
            FillPhaseDifferences();
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
        return phase_differences_;
    }

    void DPSKModulator::FillPhaseDifferences() {
        phase_differences_.clear();
        static constexpr double kTotalAngle = 360; // количество градусов на окружности
        const double kStepPhase = kTotalAngle / positionality_;
        vector<vector<bool>> grey_codes = gray_code::MakeGrayCodes(positionality_);
        double current_phase = 0;
        for (uint16_t i = 0; i < positionality_; ++i) {
            assert(current_phase < kTotalAngle);
            phase_differences_.emplace(math::ConvertationBinToDec(grey_codes[i]), current_phase);
            current_phase += kStepPhase;
        }
    }

    void DPSKModulator::ModulationOneSymbol(vector<double>::iterator begin_samples, vector<double>::iterator end_samples, uint16_t current_symbol, double& phase) {
        const double kCyclicFrequency = 2 * M_PI * carrier_frequency_; // циклическая частота
        const double kTimeStepBetweenSamples = 1.0 / sampling_frequency_; // шаг дискретизации во временной области
        const double kFixedCoefficient = kCyclicFrequency * kTimeStepBetweenSamples; // коэффициент, не изменяющийся в процессе дискретизации
        const double kPhaseDifferent = phase_differences_.find(current_symbol)->second;
        int count = 0;
//        phase += 2 * M_PI * current_symbol / positionality_; // возникают трудности при количестве позиции ОФМ больше 2
        phase += math::DegreesToRadians(kPhaseDifferent);
        for (vector<double>::iterator it = begin_samples; it != end_samples; ++it) {
            *it = amplitude_ * sin(kFixedCoefficient * count++ + phase);
        }
    }

    vector<double> DPSKModulator::Modulation(const vector<bool>& bits) {
        // частота дискретизации должна быть кратна несущей частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % carrier_frequency_) {
            throw invalid_argument("The sampling frequency must be a multiple of the carrier frequency so that there is an integer number of samples in one period."s);
        }
        const uint32_t kNumBitsInOneSymbol = log2(positionality_); // количество бит в одном символе
        vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, kNumBitsInOneSymbol);
        const uint16_t kNumSpamlesInElementarySignal = sampling_frequency_ / carrier_frequency_; // количество отсчетов в одном модулированном символе
        vector<double> modulated_signal(kNumSpamlesInElementarySignal * symbols.size());

        for (size_t symbol_id = 0; symbol_id < symbols.size(); ++symbol_id) {
            vector<double>::iterator left_bound = modulated_signal.begin() + symbol_id * kNumSpamlesInElementarySignal;
            vector<double>::iterator right_bound = modulated_signal.begin() + (symbol_id + 1) * kNumSpamlesInElementarySignal;
            ModulationOneSymbol(left_bound, right_bound, symbols[symbol_id], phase_);
        }
        return modulated_signal;
    }
} // namespace dpsk_mod
