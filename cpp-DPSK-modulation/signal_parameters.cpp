#include "signal_parameters.h"
#include "math_operations.h"

#include <string>
#include <cmath>
#include <stdexcept>

using namespace std;

SignalParameters::SignalParameters(int sampling_frequency, int symbol_speed) {
    if (sampling_frequency <= 0) {
        throw invalid_argument("Value "s + to_string(sampling_frequency) + " of sampling frequency isn't positive"s);
    }
    if (symbol_speed <= 0) {
        throw invalid_argument("Value "s + to_string(symbol_speed) + " of symbol speed isn't positive"s);
    }
    // частота дискретизации должна быть кратна символьной скорости
    if (sampling_frequency % symbol_speed) {
        throw invalid_argument("Sampling frequency must be a multiple of the symbol speed"s);
    }
    sampling_frequency_ = sampling_frequency;
    time_step_between_samples_ = 1.0 / sampling_frequency_;
    symbol_speed_ = symbol_speed;
    carrier_frequency_ = symbol_speed;
}

SignalParameters& SignalParameters::SetPositionality(int positionality) {
    if (!math::IsPowerOfTwo(positionality)) {
        throw invalid_argument("Positionality is not a power of two."s);
    }
    positionality_ = positionality;
    return  *this;
}

uint16_t SignalParameters::GetPositionality() const noexcept {
    return positionality_;
}

SignalParameters& SignalParameters::SetAmplitude(double new_amplitude) {
    if (new_amplitude <= 0) {
        throw invalid_argument("Value "s + to_string(new_amplitude) + " of amplitude isn't positive"s);
    }
    amplitude_ = new_amplitude;
    return *this;
}

double SignalParameters::GetAmplitude() const noexcept {
    return amplitude_;
}

SignalParameters& SignalParameters::SetPhase(double new_phase) noexcept {
    phase_ = new_phase;
    return *this;
}

double SignalParameters::GetPhase() const noexcept {
    return phase_;
}

SignalParameters& SignalParameters::SetCarrierFrequency(int carrier_frequency) {
    if (carrier_frequency <= 0) {
        throw invalid_argument("Value "s + to_string(carrier_frequency) + " of carrier frequency isn't positive"s);
    }
    if (4 * static_cast<uint32_t>(carrier_frequency) > sampling_frequency_) {
        throw invalid_argument("4 * "s + to_string(carrier_frequency) + " > "s + to_string(sampling_frequency_) + ". Nyquist's theorem does not hold"s);
    }
    carrier_frequency_ = carrier_frequency;
    carrier_cyclic_frequency_ = 2 * M_PI * carrier_frequency_;
    return *this;
}

uint32_t SignalParameters::GetCarrierFrequency() const noexcept {
    return carrier_frequency_;
}

uint32_t SignalParameters::GetSamplingFrequency() const noexcept {
    return sampling_frequency_;
}

SignalParameters& SignalParameters::SetPhaseShift(double phase_shift) {
    phase_shift_ = phase_shift;
    return *this;
}

double SignalParameters::GetPhaseShift() const noexcept {
    return phase_shift_;
}

uint32_t SignalParameters::GetSymbolSpeed() const noexcept {
    return symbol_speed_;
}
