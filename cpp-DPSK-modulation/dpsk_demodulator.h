#pragma once
#include "signal_parameters.h"

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

    class DPSKDemodulator : public SignalParameters {
    public:
        /// По умолчанию используется двухпозиционная ОФМ. Сложность: O(2 * (positionality * log2(positionality)))
        DPSKDemodulator(int positionality = 2);

        /// Установить позиционность модуляции. Сложность: O(2 * (positionality * log2(positionality)))
        DPSKDemodulator& SetPositionality(int positionality);

        /// Получить косинусную и синусную составляющие при заданных ранее частотах. Сложность: O(1)
        const InPhaseAndQuadratureComponents& GetInPhaseAndQuadratureComponents() const noexcept;

        /// Демодуляция последовательности отсчетов. Сложность: O(???)
        std::vector<bool> Demodulation(const std::vector<double>& samples);

    private:
        /// Сгенерировать один период косинуса и синуса при заданных параметрах. Сложность(sampling_frequency / carrier_frequency)
        void FillCosAndSinOscillation();

    private:
        InPhaseAndQuadratureComponents IpQ_components_;


    };
} // namespace dpsk_demod
