#include "dpsk_modulator.h"
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
            SignalParameters::SetPositionality(positionality);
            FillPhaseShifts();
        }
        return *this;
    }

    DPSKModulator& DPSKModulator::SetModulationFunction(function<double(double)> mod_function) {
        mod_function_ = mod_function;
        double (*const* func)(double) = mod_function_.target<double(*)(double)>();

        if (func && *func == *Sin.target<double(*)(double)>()) {
            ortogonal_mod_function_ = Cos;
        } else {
            ortogonal_mod_function_ = Sin;
        }
        return *this;
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

    void DPSKModulator::ModulationOneSymbol(vector<double>::iterator begin_samples, vector<double>::iterator end_samples, uint16_t current_symbol, double& phase) const {
        const double kFixedCoefficient = carrier_cyclic_frequency_ * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации
        const double kPhaseDifferent = phase_shifts_.find(current_symbol)->second;
        int count = 0;
//        phase += 2 * M_PI * current_symbol / positionality_; // возникают трудности при количестве позиции ОФМ больше 2
        phase += math::DegreesToRadians(kPhaseDifferent);
        for (vector<double>::iterator it = begin_samples; it != end_samples; ++it) {
            *it = amplitude_ * mod_function_(kFixedCoefficient * count++ + phase);
        }
    }

    void DPSKModulator::ClassicalModulation(const vector<uint32_t>& symbols, vector<double>& modulated_signal, uint16_t num_samples_in_symbol) {
        const double kCyclicFrequencyCoefficient = carrier_cyclic_frequency_ * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации

        for (size_t symbol_id = 0; symbol_id < symbols.size(); ++symbol_id) {
            phase_ += math::DegreesToRadians(phase_shifts_.find(symbols[symbol_id])->second);
            math::PhaseToRangeFrom0To2PI(phase_);
            for (uint16_t sample_id = 0; sample_id < num_samples_in_symbol; ++sample_id) {
//                modulated_signal[sample_id + symbol_id * num_samples_in_symbol] = amplitude_ * mod_function_(kCyclicFrequencyCoefficient * sample_id + phase_);
                // минус phase_ т.к. вращение по окружности против часовой стрелки (сложение на окружности). Плюс - вращение по часовой (вычитание на окружности)
                modulated_signal[sample_id + symbol_id * num_samples_in_symbol] = amplitude_ * mod_function_(kCyclicFrequencyCoefficient * sample_id - phase_);
            }
        }
    }

    void DPSKModulator::ModulationWithUseIntermediateFreq(const vector<uint32_t>& symbols, vector<double>& modulated_signal, uint16_t num_samples_in_symbol) {
        const double kDiffrerenceCyclicFrequency = (intermediate_cyclic_frequency_ - carrier_cyclic_frequency_);
        const double kDiffrerenceCyclicFrequencyCoefficient = kDiffrerenceCyclicFrequency * time_step_between_samples_;
        const double kIntermediateCyclicFrequencyCoefficient = intermediate_cyclic_frequency_ * time_step_between_samples_;

        for (size_t symbol_id = 0; symbol_id < symbols.size(); ++symbol_id) {
            phase_ += math::DegreesToRadians(phase_shifts_.find(symbols[symbol_id])->second);
            math::PhaseToRangeFrom0To2PI(phase_);
            for (uint16_t sample_id = 0; sample_id < num_samples_in_symbol; ++sample_id) {
                size_t time_difference_step = sample_id + symbol_id * num_samples_in_symbol;
                // минус phase_ т.к. вращение по окружности против часовой стрелки (сложение на окружности). Плюс - вращение по часовой (вычитание на окружности)
                double sample_of_signal_on_intermediate_frequency = amplitude_ * mod_function_(kIntermediateCyclicFrequencyCoefficient * time_difference_step - phase_);
                double sample_of_orthogonal_signal = amplitude_ * ortogonal_mod_function_(kIntermediateCyclicFrequencyCoefficient * time_difference_step - phase_);
                modulated_signal[sample_id + symbol_id * num_samples_in_symbol] = sample_of_signal_on_intermediate_frequency *
                        ortogonal_mod_function_(kDiffrerenceCyclicFrequencyCoefficient * time_difference_step) - sample_of_orthogonal_signal
                        * mod_function_(kDiffrerenceCyclicFrequencyCoefficient * time_difference_step);
            }
        }
    }

    // Можно сделать в любом случае сложность O(N), путем написания перегрузки ConvertationBitsToDecValues, где будет добавляться опорный символ
    vector<double> DPSKModulator::Modulation(const vector<bool>& bits, PresencePivotSymbol is_presence) {
        const uint32_t kUsedCarrierFrequency = sampling_frequency_ % carrier_frequency_ ? intermediate_frequency_ : carrier_frequency_;
        // частота дискретизации должна быть кратна несущей или промежуточной частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % kUsedCarrierFrequency) {
            throw invalid_argument("The sampling frequency must be a multiple of the carrier frequency so that there is an integer number of samples in one period."s);
        }
        const uint32_t kNumBitsInOneSymbol = log2(positionality_); // количество бит в одном символе
        vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, kNumBitsInOneSymbol);
        if (is_presence == PresencePivotSymbol::WITHOUT_PIVOT) {
            symbols.insert(symbols.begin(), 0); // добавление опорного символа за O(N)
        }
        const uint16_t kNumSpamlesInElementarySignal = sampling_frequency_ / kUsedCarrierFrequency; // количество отсчетов в одном модулированном символе
        vector<double> modulated_signal(kNumSpamlesInElementarySignal * symbols.size());

        if (kUsedCarrierFrequency == carrier_frequency_) {
            ClassicalModulation(symbols, modulated_signal, kNumSpamlesInElementarySignal);
        } else {
            ModulationWithUseIntermediateFreq(symbols, modulated_signal, kNumSpamlesInElementarySignal);
        }
        return modulated_signal;
    }

    vector<double> DPSKModulator::Modulation(const vector<bool>& bits, int positionality, PresencePivotSymbol is_presence) {
        SetPositionality(positionality);
        return Modulation(bits, is_presence);
    }
} // namespace dpsk_mod
