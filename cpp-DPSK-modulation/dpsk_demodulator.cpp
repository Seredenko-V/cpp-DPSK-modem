#include "dpsk_demodulator.h"
#include "gray_code.h"
#include "math_operations.h"

#include <cassert>
#include <functional>

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

    double DPSKDemodulator::ExtractPhaseValue(complex<double> inphase_quadrature_components) const {
        return atan2(inphase_quadrature_components.imag(), inphase_quadrature_components.real());
    }

    void DPSKDemodulator::FillCosAndSinOscillation() {
        const uint32_t kUsedCarrierFrequency = sampling_frequency_ % carrier_frequency_ ? intermediate_frequency_ : carrier_frequency_;
        // частота дискретизации должна быть кратна несущей или промежуточной частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % kUsedCarrierFrequency) {
            throw invalid_argument("The sampling frequency must be a multiple of the carrier frequency so that there is an integer number of samples in one period."s);
        }
        const uint16_t kNumSamplesInSymbol = sampling_frequency_ / kUsedCarrierFrequency;
        cos_oscillation_.resize(kNumSamplesInSymbol);
        sin_oscillation_.resize(kNumSamplesInSymbol);

        const double kCyclicFrequencyCoefficient = 2 * M_PI * kUsedCarrierFrequency * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации
        for (uint16_t sample_id = 0; sample_id < kNumSamplesInSymbol; ++sample_id) {
            cos_oscillation_[sample_id] = amplitude_ * cos(kCyclicFrequencyCoefficient * sample_id + phase_);
            sin_oscillation_[sample_id] = amplitude_ * sin(kCyclicFrequencyCoefficient * sample_id + phase_);
        }
    }

    void DPSKDemodulator::FillSymbolsBounds() {
        const double kStepPhase = 2 * M_PI / positionality_;
        bounds_symbols_.resize(positionality_, kStepPhase / 2);
        // значение bounds_symbols_[0] уже задано верно
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

    const vector<uint16_t>& DPSKDemodulator::GetSymbolsSequenceOnCircle() const noexcept {
        return symbols_sequence_on_circle_;
    }

    uint16_t DPSKDemodulator::DefineSymbol(double phase_difference) const noexcept {
        // чтобы фаза была в пределах [0, 2*PI)
        while (phase_difference >= 2 * M_PI) {
            phase_difference -= 2 * M_PI;
        }
        assert(!bounds_symbols_.empty());
        // lower_bound вернет итератор, чтобы получить индекс - нужно использовать distance, что сделает сложность O(N*N*log2(N))
        for (size_t i = 0; i < bounds_symbols_.size() - 1; ++i) {
            // принадлежность интервалу [bounds_symbols_[i], bounds_symbols_[i + 1])
            if (!less<double>()(phase_difference, bounds_symbols_[i]) && less<double>()(phase_difference, bounds_symbols_[i + 1])) {
                return symbols_sequence_on_circle_[i + 1];
            }
        }
        return symbols_sequence_on_circle_.front(); // последний интервал значений
    }


    vector<bool> DPSKDemodulator::Demodulation(const vector<double>& samples) {


        return {};
    }
    // <================================= Demodulation ===================================

} // namespace dpsk_demod
