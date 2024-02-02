#pragma once

#include "domain.h"

#include <cstdint>
#include <vector>
#include <complex>

#include <boost/circular_buffer.hpp>

namespace cycle_synch {
    using It = std::vector<double>::const_iterator;

    class PhaseSynchronizator {
    public:
        PhaseSynchronizator(int64_t sampling_freq, int64_t carrier_freq, int64_t buffer_capacity = 10'000);

        void SetBufferCapacity(int64_t capacity);
        uint32_t GetBufferCapacity() const noexcept;

        void SetCarrierFreq(int64_t new_carrier_freq);
        uint32_t GetCarrierFreq() const noexcept;

        void SetSamplingFreq(int64_t new_sampling_freq);
        uint32_t GetSamplingFreq() const noexcept;

        void SetPhaseDiffThreshold(double new_threshold);
        double GetPhaseDiffThreshold() const noexcept;

        void SetNumPosForDetermSynch(int64_t num_pos_for_determ_synch);
        uint32_t GetNumPosForDetermSynch() const noexcept;

        /// Установить допустимую вероятность ошибки на бит, на основании которой будет определено значение порога
        /// разности фаз
        void SetAllowableBER(double BER);

        template <typename Iterator>
        uint32_t DetermClockSynchPos(Iterator begin_range, Iterator end_range);
        uint32_t DetermClockSynchPos(const std::vector<double>& samples);

        domain::IteratorRange<It> PrepareRangeForDemodulation(const std::vector<double>& samples);

    private:
        /// Определить потенциальные позиции тактовой синхронизации
        template <typename Iterator>
        std::vector<std::complex<double>> DetermPotentialPosOfSynch(Iterator begin_range, Iterator end_range);

        uint32_t ExtractSynchPos(const std::vector<std::complex<double>>& potential_pos_of_synch);

    private:
        // порог разности фаз, превышение которого говорит о возможной позиции тактовой синхронизации
        double samples_diff_threshold_ = 0.;
        uint32_t num_pos_for_determ_synch_ = 0u; // количество векторов для получения результирующего

        uint32_t carrier_freq_ = 0u; // несущая частота, Гц
        double cyclic_carrier_freq_ = 0.; // циклическая несущая частота, радианы

        uint32_t sampling_freq_ = 0u; // частота дискретизации, Гц
        double time_step_between_samples_ = 0.; // шаг дискретизации, с

        uint32_t num_samples_per_period_ = 0u;
        boost::circular_buffer<double> received_samples_;
    };


    template <typename Iterator>
    std::vector<std::complex<double>> PhaseSynchronizator::DetermPotentialPosOfSynch(Iterator begin_range, Iterator end_range) {
        std::vector<std::complex<double>> potential_positions_synch(num_pos_for_determ_synch_);
        uint32_t counter_deviations = 0u;

        for (Iterator it = begin_range + 2; it != end_range; ++it) {
            double new_theory_sample = 2 * cos(cyclic_carrier_freq_ * time_step_between_samples_) * *(it - 1) - *(it - 2);
            if (std::abs(new_theory_sample - *it) > samples_diff_threshold_) {
                const double current_pos_radian = 2 * M_PI * (distance(begin_range, it) % num_samples_per_period_) / num_samples_per_period_;
                potential_positions_synch[counter_deviations++] = std::complex<double>(cos(current_pos_radian), sin(current_pos_radian));
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
        return potential_positions_synch;
    }

    template <typename Iterator>
    uint32_t PhaseSynchronizator::DetermClockSynchPos(Iterator begin_range, Iterator end_range) {
        if (std::distance(begin_range, end_range) < num_samples_per_period_) { // меньше одного периода
            throw std::invalid_argument("Number of samples is less than one period elementary signal.\n");
        }
        std::vector<std::complex<double>> potential_positions_synch = DetermPotentialPosOfSynch(begin_range, end_range);
        return ExtractSynchPos(potential_positions_synch);
    }

    namespace tests {
        void TestDetermClockSynchPos();
        void RunAllTests();
    } // namespace tests
} // namespace cycle_synch


