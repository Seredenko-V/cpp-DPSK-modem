#include "phase_synchronizator.h"

#include <stdexcept>
#include <string>

using namespace std;

PhaseSynchronizator::PhaseSynchronizator(int64_t carrier_freq, int64_t sampling_freq) {
    SetCarrierFreq(carrier_freq);
    SetSamplingFreq(sampling_freq);
}

void PhaseSynchronizator::SetCarrierFreq(int64_t new_carrier_freq) {
    if (new_carrier_freq <= 0) {
        throw invalid_argument("Carrier frequency is not positive. Current value is "s + to_string(new_carrier_freq) + ".\n");
    }
    carrier_freq_ = new_carrier_freq;
    cyclic_carrier_freq_ = 2 * M_PI * carrier_freq_;
}


uint32_t PhaseSynchronizator::GetCarrierFreq() const noexcept {
    return carrier_freq_;
}

void PhaseSynchronizator::SetSamplingFreq(int64_t new_sampling_freq) {
    if (new_sampling_freq < static_cast<int64_t>(carrier_freq_) * 2) {
        throw invalid_argument(to_string(new_sampling_freq) + " < "s + "2 * "s + to_string(carrier_freq_) + ". Nyquist's theorem does not hold"s);
    }
    sampling_freq_ = new_sampling_freq;
    time_step_between_samples_ = 1.0 / sampling_freq_;
}

uint32_t PhaseSynchronizator::GetSamplingFreq() const noexcept {
    return sampling_freq_;
}
