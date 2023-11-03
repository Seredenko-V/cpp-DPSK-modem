#include "dpsk_demodulator.h"

using namespace std;

namespace dpsk_demod {
    // ================================== Demodulation ==================================>
    const InPhaseAndQuadratureComponents& DPSKDemodulator::GetInPhaseAndQuadratureComponents() const noexcept {
        return IpQ_components_;
    }

    complex<double> InPhaseAndQuadratureComponents::ExtractInPhaseAndQuadratureComponentsSymbol(const vector<double>& one_symbol_samples) const {
        if (one_symbol_samples.size() != cos_oscillation.size()) {
            throw logic_error("Samples number of symbol is not equal samples number cos and sin oscillation"s);
        }
        double cos_component = 0.0;
        double sin_component = 0.0;
        // скалярное произведение с косинусным и синусным колебанием
        for (size_t sample_id = 0; sample_id < one_symbol_samples.size(); ++sample_id) {
            cos_component += one_symbol_samples[sample_id] * cos_oscillation[sample_id];
            sin_component += one_symbol_samples[sample_id] * sin_oscillation[sample_id];
        }
        // нормировка
        cos_component /= one_symbol_samples.size();
        sin_component /= one_symbol_samples.size();
        return {cos_component, sin_component};
    }

    void DPSKDemodulator::FillCosAndSinOscillation() {
        const uint32_t kUsedCarrierFrequency = sampling_frequency_ % carrier_frequency_ ? intermediate_frequency_ : carrier_frequency_;
        // частота дискретизации должна быть кратна несущей или промежуточной частоте, чтобы в одном периоде было целое количество отсчетов
        if (sampling_frequency_ % kUsedCarrierFrequency) {
            throw invalid_argument("The sampling frequency must be a multiple of the carrier frequency so that there is an integer number of samples in one period."s);
        }
        const uint16_t kNumSamplesInSymbol = sampling_frequency_ / kUsedCarrierFrequency;
        IpQ_components_.cos_oscillation.resize(kNumSamplesInSymbol);
        IpQ_components_.sin_oscillation.resize(kNumSamplesInSymbol);

        const double kCyclicFrequencyCoefficient = 2 * M_PI * kUsedCarrierFrequency * time_step_between_samples_; // коэффициент, не изменяющийся в процессе дискретизации
        for (uint16_t sample_id = 0; sample_id < kNumSamplesInSymbol; ++sample_id) {
            IpQ_components_.cos_oscillation[sample_id] = amplitude_ * cos(kCyclicFrequencyCoefficient * sample_id + phase_);
            IpQ_components_.sin_oscillation[sample_id] = amplitude_ * sin(kCyclicFrequencyCoefficient * sample_id + phase_);
        }
    }

    vector<bool> DPSKDemodulator::Demodulation(const vector<double>& samples) {


        return {};
    }
    // <================================= Demodulation ===================================

} // namespace dpsk_demod
