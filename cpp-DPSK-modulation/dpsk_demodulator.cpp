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
    DPSKDemodulator::DPSKDemodulator(int sampling_frequency, int symbol_speed, int positionality)
        : SignalParameters(sampling_frequency, symbol_speed) {
        SetPositionality(positionality);
    }

    DPSKDemodulator& DPSKDemodulator::SetPositionality(int positionality) {
        if (positionality_ != positionality) {
            SignalParameters::SetPositionality(positionality);
            FillSymbolsBounds();
            FillSymbolsSequenceOnCircle();
        }
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
        return {cos_component * 2, sin_component * 2};
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
        return {cos_component * 2, sin_component * 2};
    }

    double DPSKDemodulator::ExtractPhaseValue(complex<double> inphase_quadrature_components) const {
        double phase_value = atan2(inphase_quadrature_components.imag(), inphase_quadrature_components.real());
        math::PhaseToRangeFrom0To2PI(phase_value);
        return phase_value;
    }

    void DPSKDemodulator::FillCosAndSinOscillation() {
        // частота дискретизации должна быть кратна несущей или промежуточной частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % symbol_speed_) {
            throw invalid_argument("The sampling frequency must be a multiple of the symbol speed so that there is an integer number of samples in one period."s);
        }
        const uint16_t kNumSamplesInSymbol = sampling_frequency_ / symbol_speed_;
        cos_oscillation_.resize(kNumSamplesInSymbol);
        sin_oscillation_.resize(kNumSamplesInSymbol);

        const double kCyclicFrequencyCoefficient = 2 * M_PI * carrier_frequency_ * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации
        for (uint16_t sample_id = 0; sample_id < kNumSamplesInSymbol; ++sample_id) {
            cos_oscillation_[sample_id] = amplitude_ * cos(kCyclicFrequencyCoefficient * sample_id + phase_);
            sin_oscillation_[sample_id] = amplitude_ * sin(kCyclicFrequencyCoefficient * sample_id + phase_);
        }
        phase_ += kCyclicFrequencyCoefficient * kNumSamplesInSymbol;
        math::PhaseToRangeFrom0To2PI(phase_);
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

        for (size_t str = 0; str < IQ_components.size(); ++str) {
            for (size_t col = 0; col < IQ_components.size(); ++col) {
                double value = 0;
                for (size_t i = 0; i < cos_oscillation_.size(); ++i) {
                    value += IQ_components[str]->at(i) * IQ_components[col]->at(i);
                }
                decorrelation_matrix_.put(str, col, value * 2 / cos_oscillation_.size());
            }
        }
        decorrelation_matrix_.invert();
    }

    const Matrix<double>& DPSKDemodulator::GetDecorrelationMatrix() const noexcept {
        return decorrelation_matrix_;
    }

    std::complex<double> DPSKDemodulator::Decorrelation(std::complex<double> IQ_components) {
        Matrix<double> components(2, 1);
        components.put(0, 0, IQ_components.real());
        components.put(1, 0, IQ_components.imag());
        components = decorrelation_matrix_ * components;
        return {components.get(0, 0), components.get(1, 0)};
    }

    vector<uint32_t> DPSKDemodulator::Demodulation(const vector<double>& samples) {
        if (symbol_speed_ == 0 || sampling_frequency_ % symbol_speed_) {
            throw runtime_error(to_string(symbol_speed_) + " isn't multiples to "s + to_string(sampling_frequency_));
        }
        const uint32_t kNumSamplesPerSymbol = sampling_frequency_ / symbol_speed_;

        // "хвосты" должны буферизоваться в синхронизаторе
        if (samples.size() % kNumSamplesPerSymbol) {
            throw invalid_argument("Samples number is not multiple number of samples per symbol"s);
        }
        vector<uint32_t> demodulated_symbols(samples.size() / kNumSamplesPerSymbol - 1);

        for (size_t i = 0; i < samples.size() - kNumSamplesPerSymbol; i += kNumSamplesPerSymbol) {
            vector<double>::const_iterator first_symbol_begin_it = samples.begin() + i;
            vector<double>::const_iterator first_symbol_end_it = samples.begin() + i + kNumSamplesPerSymbol;
            complex<double> first_symbol_IQ_components = ExtractInPhaseAndQuadratureComponentsSymbol(first_symbol_begin_it, first_symbol_end_it);

            if (sampling_frequency_ % carrier_frequency_) {
                FillCosAndSinOscillation();
            }
            vector<double>::const_iterator second_symbol_begin_it = samples.begin() + i + kNumSamplesPerSymbol;
            vector<double>::const_iterator second_symbol_end_it = samples.begin() + i + 2 * kNumSamplesPerSymbol;
            complex<double> second_symbol_IQ_components = ExtractInPhaseAndQuadratureComponentsSymbol(second_symbol_begin_it, second_symbol_end_it);

            if (sampling_frequency_ % carrier_frequency_) {
                first_symbol_IQ_components = Decorrelation(first_symbol_IQ_components);
                second_symbol_IQ_components = Decorrelation(second_symbol_IQ_components);
            }
            double phase_defference = ExtractPhaseValue(second_symbol_IQ_components) - ExtractPhaseValue(first_symbol_IQ_components);
            math::PhaseToRangeFrom0To2PI(phase_defference);
            demodulated_symbols[i / kNumSamplesPerSymbol] = DefineSymbol(phase_defference);
        }
        return demodulated_symbols;
    }
} // namespace dpsk_demod
