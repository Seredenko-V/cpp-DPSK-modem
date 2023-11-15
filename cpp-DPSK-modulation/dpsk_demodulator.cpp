#include "dpsk_demodulator.h"
#include "gray_code.h"
#include "math_operations.h"

#include <cassert>
#include <functional>


// для отладки
#include <iostream>
#include <fstream>

using namespace std;

namespace dpsk_demod {
    DPSKDemodulator::DPSKDemodulator(int positionality) {
        SetPositionality(positionality);
    }

    DPSKDemodulator& DPSKDemodulator::SetPositionality(int positionality) {
        SignalParameters::SetPositionality(positionality);
        FillSymbolsBounds();
        FillSymbolsSequenceOnCircle();
        return *this;
    }

    DPSKDemodulator& DPSKDemodulator::SetCarrierFrequency(int carrier_frequency) {
        SignalParameters::SetCarrierFrequency(carrier_frequency);
        FillCosAndSinOscillation();
        if (sampling_frequency_ % carrier_frequency_) {
            CreateDecorrelationMatrix();
        }
        return *this;
    }

    complex<double> DPSKDemodulator::ExtractInPhaseAndQuadratureComponentsSymbol(const vector<double>& one_symbol_samples) const {
        if (one_symbol_samples.size() != cos_oscillation_.size()) {
            throw logic_error("Samples number of symbol is not equal samples number cos and sin oscillation"s);
        }
        double cos_component = 0.0;
        double sin_component = 0.0;
        // скалярное произведение с косинусным и синусным колебанием
        for (size_t sample_id = 0; sample_id < one_symbol_samples.size(); ++sample_id) {
            cos_component += one_symbol_samples[sample_id] * cos_oscillation_[sample_id];
            sin_component += one_symbol_samples[sample_id] * sin_oscillation_[sample_id];
        }
        // нормировка
        cos_component /= one_symbol_samples.size();
        sin_component /= one_symbol_samples.size();
        return {cos_component, sin_component};
    }

    complex<double> DPSKDemodulator::ExtractInPhaseAndQuadratureComponentsSymbol(vector<double>::const_iterator left_bound, vector<double>::const_iterator right_bound) const {
        const size_t kNumSamplesPerSymbol = distance(left_bound, right_bound);
        if (kNumSamplesPerSymbol != cos_oscillation_.size()) {
            throw logic_error("Samples number of symbol is not equal samples number cos and sin oscillation"s);
        }
        double cos_component = 0.0;
        double sin_component = 0.0;
        // скалярное произведение с косинусным и синусным колебанием
        for (vector<double>::const_iterator it_signal = left_bound, it_cos = cos_oscillation_.begin(), it_sin = sin_oscillation_.begin();
             it_signal != right_bound; ++it_signal, ++it_cos, ++it_sin) {
            cos_component += *it_signal * *it_cos;
            sin_component += *it_signal * *it_sin;
        }
        // нормировка
        cos_component /= kNumSamplesPerSymbol;
        sin_component /= kNumSamplesPerSymbol;
        return {cos_component, sin_component};
    }

    double DPSKDemodulator::ExtractPhaseValue(complex<double> inphase_quadrature_components) const {
        double phase_value = atan2(inphase_quadrature_components.imag(), inphase_quadrature_components.real());
        math::PhaseToRangeFrom0To2PI(phase_value);
        return phase_value;
    }

    void DPSKDemodulator::FillCosAndSinOscillation() {
//        const uint32_t kUsedCarrierFrequency = sampling_frequency_ % carrier_frequency_ ? intermediate_frequency_ : carrier_frequency_;
        // частота дискретизации должна быть кратна несущей или промежуточной частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % symbol_speed_) {
            throw invalid_argument("The sampling frequency must be a multiple of the symbol speed so that there is an integer number of samples in one period."s);
        }
        const uint16_t kNumSamplesInSymbol = sampling_frequency_ / symbol_speed_;
        cos_oscillation_.resize(kNumSamplesInSymbol);
        sin_oscillation_.resize(kNumSamplesInSymbol);

        const double kCyclicFrequencyCoefficient = 2 * M_PI * carrier_frequency_ * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации
//        ofstream only_cos("only_cos.txt"s);
//        ofstream only_sin("only_sin.txt"s);
        for (uint16_t sample_id = 0; sample_id < kNumSamplesInSymbol; ++sample_id) {
            cos_oscillation_[sample_id] = amplitude_ * cos(kCyclicFrequencyCoefficient * sample_id + phase_);
//            only_cos << cos_oscillation_[sample_id] << endl;
            sin_oscillation_[sample_id] = amplitude_ * sin(kCyclicFrequencyCoefficient * sample_id + phase_);
//            only_sin << sin_oscillation_[sample_id] << endl;
        }
    }

    void DPSKDemodulator::FillSymbolsBounds() {
        const double kStepPhase = 2 * M_PI / positionality_;
        bounds_symbols_.resize(positionality_);
        bounds_symbols_[0] = kStepPhase / 2 + phase_shift_; // отступ от позиции сивола на окружности
        for (uint16_t i = 1; i < positionality_; ++i) {
            bounds_symbols_[i] = bounds_symbols_[i - 1] + kStepPhase;
        }
    }

    const vector<double>& DPSKDemodulator::GetBoundsSymbols() const noexcept {
        return bounds_symbols_;
    }

    void DPSKDemodulator::FillSymbolsSequenceOnCircle() {
        symbols_sequence_on_circle_.resize(positionality_);
        vector<vector<bool>> grey_codes = gray_code::MakeGrayCodes(positionality_);
        for (uint16_t i = 0; i < positionality_; ++i) {
            symbols_sequence_on_circle_[i] = math::ConvertationBinToDec(grey_codes[i]);
        }
    }

    const vector<uint32_t>& DPSKDemodulator::GetSymbolsSequenceOnCircle() const noexcept {
        return symbols_sequence_on_circle_;
    }

    uint32_t DPSKDemodulator::DefineSymbol(double phase_difference) const noexcept {
        math::PhaseToRangeFrom0To2PI(phase_difference);
        assert(!bounds_symbols_.empty());
        // lower_bound вернет итератор, чтобы получить индекс для обращения к вектору символов нужно использовать distance.
        // Это сделает сложность O(N*N*log2(N)). Сейчас просто O(N)
        for (size_t i = 0; i < bounds_symbols_.size() - 1; ++i) {
            // принадлежность интервалу [bounds_symbols_[i], bounds_symbols_[i + 1])
            if (!less<double>()(phase_difference, bounds_symbols_[i]) && less<double>()(phase_difference, bounds_symbols_[i + 1])) {
                return symbols_sequence_on_circle_[i + 1];
            }
        }
        return symbols_sequence_on_circle_.front(); // последний интервал значений
    }

    DPSKDemodulator& DPSKDemodulator::SetPhaseShift(double phase_shift) {
        SignalParameters::SetPhaseShift(phase_shift);
        FillSymbolsBounds();
        return *this;
    }

    void DPSKDemodulator::CreateDecorrelationMatrix() {
        vector<const vector<double>*> IQ_components{&cos_oscillation_, &sin_oscillation_};
        decorrelation_matrix_ = Matrix<double>(IQ_components.size(), IQ_components.size());
//        decorrelation_matrix_.resize(IQ_components.size(), vector<double>(IQ_components.size()));

        for (size_t str = 0; str < IQ_components.size(); ++str) {
            for (size_t col = 0; col < IQ_components.size(); ++col) {
                double value = 0;
                for (size_t i = 0; i < cos_oscillation_.size(); ++i) {
                    value += IQ_components[str]->at(i) * IQ_components[col]->at(i);
                }
//                decorrelation_matrix_[str][col] = value * 2 / cos_oscillation_.size();
                decorrelation_matrix_.put(str, col, value * 2 / cos_oscillation_.size());
            }
        }
    }

    const Matrix<double>& DPSKDemodulator::GetDecorrelationMatrix() const noexcept {
        return decorrelation_matrix_;
    }

    vector<uint32_t> DPSKDemodulator::Demodulation(const vector<double>& samples) {
        uint32_t used_carrier_frequency = sampling_frequency_ % carrier_frequency_ ? intermediate_frequency_ : carrier_frequency_;
        // частота дискретизации должна быть кратна несущей или промежуточной частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % used_carrier_frequency || used_carrier_frequency == 0) {
            used_carrier_frequency = FindNearestMultiple(carrier_frequency_, sampling_frequency_, math::MultipleValue::LESS);
        }
        const uint32_t kNumSamplesPerSymbol = sampling_frequency_ / used_carrier_frequency;

        // "хвосты" должны буферизоваться в синхронизаторе
        if (samples.size() % kNumSamplesPerSymbol) {
            throw invalid_argument("Samples number is not multiple number of samples per symbol"s);
        }
        vector<uint32_t> demodulated_symbols(samples.size() / kNumSamplesPerSymbol - 1);

        for (size_t i = 0; i < samples.size() - kNumSamplesPerSymbol; i += kNumSamplesPerSymbol) {
            vector<double>::const_iterator first_symbol_begin_it = samples.begin() + i;
            vector<double>::const_iterator first_symbol_end_it = samples.begin() + i + kNumSamplesPerSymbol;
            complex<double> first_symbol_IQ_components = ExtractInPhaseAndQuadratureComponentsSymbol(first_symbol_begin_it, first_symbol_end_it);

            vector<double>::const_iterator second_symbol_begin_it = samples.begin() + i + kNumSamplesPerSymbol;
            vector<double>::const_iterator second_symbol_end_it = samples.begin() + i + 2 * kNumSamplesPerSymbol;
            complex<double> second_symbol_IQ_components = ExtractInPhaseAndQuadratureComponentsSymbol(second_symbol_begin_it, second_symbol_end_it);

            double phase_defference = ExtractPhaseValue(second_symbol_IQ_components) - ExtractPhaseValue(first_symbol_IQ_components);
            math::PhaseToRangeFrom0To2PI(phase_defference);
            demodulated_symbols[i / kNumSamplesPerSymbol] = DefineSymbol(phase_defference);
        }
        return demodulated_symbols;
    }
} // namespace dpsk_demod
