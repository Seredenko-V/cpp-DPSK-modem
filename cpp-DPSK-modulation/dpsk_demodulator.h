#pragma once
#include "signal_parameters.h"

#include <cstdint>
#include <vector>
#include <complex>

namespace dpsk_demod {
    class DPSKDemodulator : public SignalParameters {
    public:
        /// По умолчанию используется двухпозиционная ОФМ. Сложность: O(2 * (positionality * log2(positionality)))
        DPSKDemodulator(int positionality = 2);

        /// Установить позиционность модуляции. Сложность: O(2 * (positionality * log2(positionality)))
        DPSKDemodulator& SetPositionality(int positionality);

        /// Извлечь синфазную и квадратурную составляющие символа (элементарного сиганала). Сложность: O(N)
        std::complex<double> ExtractInPhaseAndQuadratureComponentsSymbol(const std::vector<double>& one_symbol_samples) const;

        /// Извлечь значение фазы (угла) из синфазной и квадратурной составляющих элементарного сигнала. Сложность: O(????)
        double ExtractPhaseValue(std::complex<double> inphase_quadrature_components) const;

        /// Демодуляция последовательности отсчетов. Сложность: O(???)
        std::vector<bool> Demodulation(const std::vector<double>& samples);

//    private:
        /// Сгенерировать один период косинуса и синуса при заданных параметрах. Сложность(sampling_frequency / carrier_frequency)
        void FillCosAndSinOscillation();

        const std::vector<double>& GetBoundsSymbols() const noexcept;

    private:
        /// Заполнить границы (сектора) символов на огружности
        void FillSymbolsBounds();

    private:
        // один период косинуса и синуса
        std::vector<double> cos_oscillation_;
        std::vector<double> sin_oscillation_;
        std::vector<double> bounds_symbols_; // границы диапазонов разностей фаз между символами
    };

    namespace tests {
        void TestExtractInPhaseAndQuadratureComponentsSymbol();
        void TestExtractPhaseValue();
        void TestFillSymbolsBounds();
        void RunAllTests();
    } // namespace tests
} // namespace dpsk_demod
