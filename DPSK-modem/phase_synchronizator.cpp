#include "phase_synchronizator.h"
#include "math_operations.h"

#include <stdexcept>
#include <string>
#include <numeric>
#include <cmath>

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

void PhaseSynchronizator::SetPhaseDiffThreshold(double new_threshold) {
    if (new_threshold < 0 || new_threshold <= math::EPSILON) {
        throw invalid_argument("Value threshold is negative or too small. Your value is "s + to_string(new_threshold) + ".\n");
    }
    samples_diff_threshold_ = new_threshold;
}


double PhaseSynchronizator::GetPhaseDiffThreshold() const noexcept {
    return samples_diff_threshold_;
}

void PhaseSynchronizator::SetNumPosForDetermSynch(int64_t num_pos_for_determ_synch) {
    if (num_pos_for_determ_synch <= 0) {
        throw invalid_argument("Number positions for determinate synchronization is not positive. Your value is "s
                               + to_string(num_pos_for_determ_synch) + ".\n");
    }
    num_pos_for_determ_synch_ = num_pos_for_determ_synch;
}

uint32_t PhaseSynchronizator::GetNumPosForDetermSynch() const noexcept {
    return num_pos_for_determ_synch_;
}

uint32_t PhaseSynchronizator::DetermClockSynchPos(const vector<double>& samples) {
    uint32_t one_period = sampling_freq_ / carrier_freq_;
    if (samples.size() < one_period) { // меньше одного периода
        throw invalid_argument("Number of samples is less than one period elementary signal.\n"s);
    }
    double last_theory_sample = samples[0];
    double prev_last_theory_sample = samples[1];

    vector<complex<double>> potential_positions_synch(num_pos_for_determ_synch_);
    uint32_t counter_deviations = 0u;

    for (size_t i = 0; i < samples.size(); ++i) {
        double new_theory_sample = 2 * cos(cyclic_carrier_freq_ * time_step_between_samples_) * prev_last_theory_sample - last_theory_sample;
        last_theory_sample = prev_last_theory_sample;
        prev_last_theory_sample = new_theory_sample;

        if (std::abs(new_theory_sample - samples[i]) >= samples_diff_threshold_) {
            potential_positions_synch[counter_deviations++] = cos(2 * M_PI * i / one_period) + sin(2 * M_PI * i / one_period);
            // если набор потенциальных позиций синхронизации заполнен
            if (counter_deviations == num_pos_for_determ_synch_ - 1) {
                break;
            }
        }
    }
    // если набор потенциальных позиций синхронизации не был полностью заполнен
    if (counter_deviations < num_pos_for_determ_synch_ - 1) {
        potential_positions_synch.resize(counter_deviations);
    }

    return ExtractSynchPos(move(potential_positions_synch));
}

uint32_t PhaseSynchronizator::ExtractSynchPos(vector<complex<double>>&& potential_pos_of_synch) {
    complex<double> result = accumulate(potential_pos_of_synch.begin(), potential_pos_of_synch.end(), complex<double>(0,0));
    double arg = atan2(result.imag(), result.real());
    return static_cast<uint32_t>(arg * (sampling_freq_ / carrier_freq_) / (2 * M_PI));
}
