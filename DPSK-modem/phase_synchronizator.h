#pragma once

#include <cstdint>
#include <vector>
#include <complex>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange() = default;
    explicit IteratorRange(Iterator begin_range, Iterator end_range)
        : begin_range_(begin_range), end_range_(end_range) {
    }
    Iterator begin() const {
        return begin_range_;
    }
    Iterator end() const {
        return end_range_;
    }
    size_t size() const {
        return distance(begin_range_, end_range_);
    }
private:
    Iterator begin_range_;
    Iterator end_range_;
};

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

        IteratorRange<std::vector<double>::const_iterator> PrepareRangeForDemodulation(const std::vector<double>& samples);

    private:
        /// Определить потенциальные позиции тактовой синхронизации
        std::vector<std::complex<double>> DetermPotentialPosOfSynch(IteratorRange<std::vector<double>::const_iterator> range);
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


