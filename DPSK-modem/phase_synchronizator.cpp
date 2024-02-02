#include "phase_synchronizator.h"
#include "math_operations.h"

#include <stdexcept>
#include <string>
#include <numeric>
#include <cmath>

using namespace std;

namespace cycle_synch {
    PhaseSynchronizator::PhaseSynchronizator(int64_t sampling_freq, int64_t carrier_freq, int64_t buffer_capacity) {
        SetCarrierFreq(carrier_freq);
        SetSamplingFreq(sampling_freq);
        num_samples_per_period_ = sampling_freq_ / carrier_freq_;
        SetBufferCapacity(buffer_capacity);
    }

    void PhaseSynchronizator::SetBufferCapacity(int64_t capacity) {
        received_samples_.set_capacity(capacity);
    }

    uint32_t PhaseSynchronizator::GetBufferCapacity() const noexcept {
        return received_samples_.capacity();
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

    uint32_t PhaseSynchronizator::ExtractSynchPos(const vector<complex<double>>& potential_pos_of_synch) {
        complex<double> result = accumulate(potential_pos_of_synch.begin(), potential_pos_of_synch.end(), complex<double>(0,0));
        double arg = atan2(result.imag(), result.real());
        math::PhaseToRangeFrom0To2PI(arg);
        arg = arg * num_samples_per_period_ / (2 * M_PI);
        return arg;
    }

    uint32_t PhaseSynchronizator::DetermClockSynchPos(const vector<double>& samples) {
        if (samples.size() < num_samples_per_period_) { // меньше одного периода
            throw invalid_argument("Number of samples is less than one period elementary signal.\n"s);
        }
        std::vector<std::complex<double>> potential_positions_synch = DetermPotentialPosOfSynch(samples.begin(), samples.end());
        return ExtractSynchPos(potential_positions_synch);
    }

    domain::IteratorRange<It> PhaseSynchronizator::PrepareRangeForDemodulation(const vector<double>& samples) {
        static uint32_t size_prev_iter = 0u; // кол-во отсчетов "хвоста" из предыдущей итерации
        if (received_samples_.capacity() <= samples.size()) {
            SetBufferCapacity(samples.size() + num_samples_per_period_);
        }
        received_samples_.resize(size_prev_iter + samples.size()); // оставляем отсчеты "хвоста"

        copy(samples.begin(), samples.end(), received_samples_.begin() + size_prev_iter);
        //uint32_t clock_synch_pos = DetermClockSynchPos(received_samples_.begin(), received_samples_.end()); // ошибка


        // определяем позицию последнего отсчета "целого" символа / начала "хвоста" следующего

        return {};
    }
} // namespace cycle_synch
