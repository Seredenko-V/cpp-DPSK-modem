#pragma once

#include "domain.h"

#include <cstdint>
#include <vector>
#include <complex>

namespace cycle_synch {
    class PhaseSynchronizator {
    public:
        PhaseSynchronizator(int64_t sampling_freq, int64_t carrier_freq);

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

        uint32_t DetermClockSynchPos(const std::vector<double>& samples);

        domain::IteratorRange<std::vector<double>::const_iterator> PrepareRangeForDemodulation(const std::vector<double>& samples);

    private:
        /// Определить потенциальные позиции тактовой синхронизации
        std::vector<std::complex<double>> DetermPotentialPosOfSynch(domain::IteratorRange<std::vector<double>::const_iterator> range);
        std::vector<std::complex<double>> DetermPotentialPosOfSynch(std::vector<double>::const_iterator begin_range,
                                                                    std::vector<double>::const_iterator end_range);

        uint32_t ExtractSynchPos(std::vector<std::complex<double>>&& potential_pos_of_synch);

    private:
        // порог разности фаз, превышение которого говорит о возможной позиции тактовой синхронизации
        double samples_diff_threshold_ = 0.;
        uint32_t num_pos_for_determ_synch_ = 0u; // количество векторов для получения результирующего

        uint32_t carrier_freq_ = 0u; // несущая частота, Гц
        double cyclic_carrier_freq_ = 0.; // циклическая несущая частота, радианы

        uint32_t sampling_freq_ = 0u; // частота дискретизации, Гц
        double time_step_between_samples_ = 0.; // шаг дискретизации, с
    };

    namespace tests {
        void TestDetermClockSynchPos();
        void RunAllTests();
    } // namespace tests
} // namespace cycle_synch


