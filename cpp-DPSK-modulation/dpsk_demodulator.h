#pragma once

#include <cstdint>
#include <vector>
#include <complex>

namespace dpsk_demod {
    struct InPhaseAndQuadratureComponents {
        /// Извлечь синфазную и квадратурную составляющие символа (элементарного сиганала). Сложность: O(N)
        std::complex<double> ExtractInPhaseAndQuadratureComponentsSymbol(const std::vector<double>& one_symbol_samples) const;

        // один период косинуса и синуса
        std::vector<double> cos_oscillation;
        std::vector<double> sin_oscillation;
    };

    class DPSKDemodulator {
    public:
        /// По умолчанию используется двухпозиционная ОФМ. Сложность: O(positionality)
        DPSKDemodulator(int positionality = 2);

        /// Установить позиционность модуляции. Сложность: O(positionality)
        DPSKDemodulator& SetPositionality(int positionality);

        /// Получить текущее количество позиций фаз. Сложность: O(1)
        uint16_t GetPositionality() const noexcept;

        /// Установить значение несущей частоты. Сложность: O(1)
        DPSKDemodulator& SetCarrierFrequency(int carrier_frequency);

        /// Получить значение несущей частоты. Сложность: O(1)
        uint32_t GetCarrierFrequency() const noexcept;

        /// Установить значение промежуточной частоты (на случай не кратности частоты дискретизации несущей). Сложность: O(1)
        DPSKDemodulator& SetIntermediateFrequency(int intermediate_frequency);

        /// Получить значение промежуточной частоты. Сложность: O(1)
        uint32_t GetIntermediateFrequency() const noexcept;

        /// Установить значение частоты дискретизации. Сложность: O(1)
        DPSKDemodulator& SetSamplingFrequency(int sampling_frequency);

        /// Получить значение частоты дискретизации. Сложность: O(1)
        uint32_t GetSamplingFrequency() const noexcept;

        /// Получить косинусную и синусную составляющие при заданных ранее частотах. Сложность: O(1)
        const InPhaseAndQuadratureComponents& GetInPhaseAndQuadratureComponents() const noexcept;

        /// Демодуляция последовательности отсчетов. Сложность: O(???)
        std::vector<bool> Demodulation(const std::vector<double>& samples);

    private:
        /// Сгенерировать один период косинуса и синуса при заданных параметрах. Сложность(sampling_frequency / carrier_frequency)
        void FillCosAndSinOscillation();

    private:
        uint16_t positionality_ = 0u; // позиционность ОФМ
        double amplitude_ = 1.0; // амплитуда колебания
        double phase_ = 0; // текущая фаза, радианы
        InPhaseAndQuadratureComponents IpQ_components_;

        uint32_t carrier_frequency_ = 0u; // несущая частота, Гц
        double carrier_cyclic_frequency_ = 0.0; // циклическая несущая частота, радианы

        uint32_t sampling_frequency_ = 0u; // частота дискретизации, Гц
        double time_step_between_samples_ = 0.0; // шаг дискретизации во временной области, с

        uint32_t intermediate_frequency_ = 1000u; // промежуточная частота, Гц
        double intermediate_cyclic_frequency_ = 0.0; // циклическая промежуточная частота, радианы
    };
} // namespace dpsk_demod
