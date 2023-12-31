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
    DPSKModulator::DPSKModulator(int sampling_frequency, int symbol_speed, int positionality)
        : SignalParameters(sampling_frequency, symbol_speed) {
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

    DPSKModulator& DPSKModulator::SetPhaseShift(double phase_shift) {
        SignalParameters::SetPhaseShift(phase_shift);
        FillPhaseShifts();
        return *this;
    }

    DPSKModulator& DPSKModulator::SetIntermediateFrequency(int intermediate_frequency) {
        if (intermediate_frequency <= 0) {
            throw invalid_argument("Value "s + to_string(intermediate_frequency) + " of intermediate frequency isn't positive"s);
        }
        if (4 * static_cast<uint32_t>(intermediate_frequency) > sampling_frequency_) {
            throw invalid_argument("4 * "s + to_string(intermediate_frequency) + " > "s + to_string(sampling_frequency_) + ". Nyquist's theorem does not hold"s);
        }
        intermediate_frequency_ = intermediate_frequency;
        intermediate_cyclic_frequency_ = 2 * M_PI * intermediate_frequency_;
        return *this;
    }

    uint32_t DPSKModulator::GetIntermediateFrequency() const noexcept {
        return intermediate_frequency_;
    }

    void DPSKModulator::FillPhaseShifts() {
        phase_shifts_.clear();
        static constexpr double kTotalAngle = 2 * M_PI; // радиан на окружности
        const double kStepPhase = kTotalAngle / positionality_;
        vector<vector<bool>> grey_codes = gray_code::MakeGrayCodes(positionality_);
        double current_phase = 0;
        for (uint16_t i = 0; i < positionality_; ++i) {
            assert(current_phase < kTotalAngle);
            phase_shifts_.emplace(math::ConvertationBinToDec(grey_codes[i]), current_phase + phase_shift_);
            current_phase += kStepPhase;
        }
    }

    void DPSKModulator::ModulationOneSymbol(vector<double>::iterator begin_samples, vector<double>::iterator end_samples, uint16_t current_symbol, double& phase) const {
        const double kFixedCoefficient = carrier_cyclic_frequency_ * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации
        const double kPhaseDifferent = phase_shifts_.find(current_symbol)->second;
        int count = 0;
        phase += math::DegreesToRadians(kPhaseDifferent);
        for (vector<double>::iterator it = begin_samples; it != end_samples; ++it) {
            *it = amplitude_ * mod_function_(kFixedCoefficient * count++ + phase);
        }
    }

    void DPSKModulator::ClassicalModulation(const vector<uint32_t>& symbols, vector<double>& modulated_signal, uint16_t num_samples_in_symbol) {
        const double kCyclicFrequencyCoefficient = carrier_cyclic_frequency_ * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации

        for (size_t symbol_id = 0; symbol_id < symbols.size(); ++symbol_id) {
            phase_ += phase_shifts_.find(symbols[symbol_id])->second;
            math::PhaseToRangeFrom0To2PI(phase_);
            for (uint16_t sample_id = 0; sample_id < num_samples_in_symbol; ++sample_id) {
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
            phase_ += phase_shifts_.find(symbols[symbol_id])->second;
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

    uint32_t DPSKModulator::DetermineUsedCarrierFreq() const {
        uint32_t used_carrier_frequency = sampling_frequency_ % carrier_frequency_ ? intermediate_frequency_ : carrier_frequency_;
        // частота дискретизации должна быть кратна несущей или промежуточной частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % used_carrier_frequency || used_carrier_frequency == 0) {
            used_carrier_frequency = FindNearestMultiple(carrier_frequency_, sampling_frequency_, math::MultipleValue::LESS);
        }
        return used_carrier_frequency;
    }

    vector<uint32_t> DPSKModulator::ExtractSymbolsFromBits(const vector<bool>& bits, PresencePivotSymbol is_presence) const {
        const uint32_t kNumBitsPerSymbol = log2(positionality_); // количество бит в одном символе
        vector<uint32_t> symbols = math::ConvertationBitsToDecValues(bits, kNumBitsPerSymbol);
        if (is_presence == PresencePivotSymbol::WITHOUT_PIVOT) {
            symbols.insert(symbols.begin(), 0); // добавление опорного символа за O(N)
        }
        return symbols;
    }

    // Можно сделать в любом случае сложность O(N), путем написания перегрузки ConvertationBitsToDecValues, где будет добавляться опорный символ
    vector<double> DPSKModulator::Modulation(const vector<bool>& bits, PresencePivotSymbol is_presence) {
        const uint32_t kUsedCarrierFrequency = DetermineUsedCarrierFreq();
        const vector<uint32_t> kSymbols = ExtractSymbolsFromBits(bits, is_presence);
        const uint16_t kNumSpamlesInElementarySignal = sampling_frequency_ / kUsedCarrierFrequency; // количество отсчетов в одном модулированном символе

        vector<double> modulated_signal(kNumSpamlesInElementarySignal * kSymbols.size());
        if (kUsedCarrierFrequency == carrier_frequency_) {
            ClassicalModulation(kSymbols, modulated_signal, kNumSpamlesInElementarySignal);
        } else {
            ModulationWithUseIntermediateFreq(kSymbols, modulated_signal, kNumSpamlesInElementarySignal);
        }
        return modulated_signal;
    }

    vector<double> DPSKModulator::Modulation(const vector<bool>& bits, int positionality, PresencePivotSymbol is_presence) {
        SetPositionality(positionality);
        return Modulation(bits, is_presence);
    }

    vector<complex<double>> DPSKModulator::ComplexModulation(const std::vector<bool>& bits, PresencePivotSymbol is_presence) {
        const vector<uint32_t> kSymbols = ExtractSymbolsFromBits(bits, is_presence);
        vector<complex<double>> modulated_signal(kSymbols.size());
        for (size_t symbol_id = 0; symbol_id < kSymbols.size(); ++symbol_id) {
            phase_ += phase_shifts_.find(kSymbols[symbol_id])->second;
            math::PhaseToRangeFrom0To2PI(phase_);
            modulated_signal[symbol_id] = complex<double>(cos(phase_), sin(phase_));
        }
        return modulated_signal;
    }
} // namespace dpsk_mod
