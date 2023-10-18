#include "dpsk_modulation.h"
#include "math_operations.h"
#include "gray_code.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>
#include <cmath>

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

    const PhaseDifferences& DPSKModulator::GetPhaseDifferences() const noexcept {
        return phase_differences_;
    }

    /// Получить разность фаз между двумя символами
    double DPSKModulator::GetDifferentPhaseBetweenSymbols(uint16_t left, uint16_t right) const {
        if (left == right) {
            return 0;
        }
        PhaseDifferences::const_iterator ptr_different = phase_differences_.find(make_pair(left, right));
        // поиск "в обратном направлении"
        if (ptr_different == phase_differences_.end()) {
            ptr_different = phase_differences_.find(make_pair(right, left));
        }
        // такого сдвига нет
        if (ptr_different == phase_differences_.end()) {
            throw invalid_argument("Phase different between \""s + to_string(left) + "\" and \""s + to_string(right) + "\" symbols not found"s);
        }
        return ptr_different->second;
    }

    void DPSKModulator::FillPhaseDifferences() {
        phase_differences_.clear();
        static constexpr double kTotalAngle = 360; // количество градусов на окружности
        const double kStepPhase = kTotalAngle / positionality_;
        vector<vector<bool>> grey_codes = gray_code::MakeGrayCodes(positionality_);

        // перебор всех возможных сочетаний символов
        for (uint16_t left_symbol_id = 0; left_symbol_id < positionality_ - 1; ++left_symbol_id) {
            double current_phase = kStepPhase;
            for (uint16_t right_symbol_id = left_symbol_id + 1; right_symbol_id < positionality_; ++right_symbol_id) {
                pair<uint16_t, uint16_t> symbols_pair = make_pair(math::ConvertationBinToDec(grey_codes[left_symbol_id]), math::ConvertationBinToDec(grey_codes[right_symbol_id]));
                phase_differences_.emplace(move(symbols_pair), current_phase);
//                cout << '{' << left_symbol_id << ", "s << right_symbol_id << "} = "s << current_phase << endl;
                current_phase += kStepPhase;
            }
        }
    }

    void DPSKModulator::ModulationOneSymbol(vector<double>::iterator begin_samples, vector<double>::iterator end_samples,
                                            uint16_t reference_symbol, uint16_t current_symbol, double phase) const {
        const double kCyclicFrequency = 2 * M_PI * carrier_frequency_; // циклическая частота
        const double kTimeStepBetweenSamples = 1.0 / sampling_frequency_; // шаг дискретизации во временной области
        const double kFixedCoefficient = kCyclicFrequency * kTimeStepBetweenSamples; // коэффициент, не изменяющийся в процессе дискретизации
        const double kPhaseDifferent = phase_differences_.find(make_pair(reference_symbol, current_symbol))->second;
        int count = 0;
        for (vector<double>::iterator it = begin_samples; it != end_samples; ++it) {
            *it = amplitude_ * sin(kFixedCoefficient * count++ + phase + kPhaseDifferent/* текущая(последняя фаза) + разность фаз ОФМ */);
        }
    }

    vector<double> DPSKModulator::Modulation(const vector<bool>& bits, uint16_t reference_symbol, double phase) const {
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
            ModulationOneSymbol(left_bound, right_bound, reference_symbol, symbols[symbol_id], phase);
            reference_symbol = symbols[symbol_id];
        }

        return modulated_signal;
    }
} // namespace dpsk_mod
