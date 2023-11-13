#pragma once
#include "signal_parameters.h"

#include <cstdint>
#include <vector>
#include <complex>

namespace dpsk_demod {
    class DPSKDemodulator : public SignalParameters {
    public:
        // По умолчанию используется двухпозиционная ОФМ. Сложность: O(2 * positionality * log2(positionality))
        DPSKDemodulator(int positionality = 2);

        /// Установить позиционность модуляции. Сложность: O(2 * (positionality * log2(positionality)))
        DPSKDemodulator& SetPositionality(int positionality) override;

        /// Извлечь синфазную и квадратурную составляющие символа (элементарного сиганала). Сложность: O(N)
        std::complex<double> ExtractInPhaseAndQuadratureComponentsSymbol(const std::vector<double>& one_symbol_samples) const;
        std::complex<double> ExtractInPhaseAndQuadratureComponentsSymbol(std::vector<double>::const_iterator left_bound, std::vector<double>::const_iterator right_bound) const;

        /// Извлечь значение фазы (угла) из синфазной и квадратурной составляющих элементарного сигнала. Сложность: O(1)
        double ExtractPhaseValue(std::complex<double> inphase_quadrature_components) const;

        /// Демодуляция последовательности отсчетов. Сложность: O(N)
        std::vector<uint32_t> Demodulation(const std::vector<double>& samples);

//    private:
        /// Сгенерировать один период косинуса и синуса при заданных параметрах. Сложность(sampling_frequency / carrier_frequency)
        void FillCosAndSinOscillation();

        /// Получить значения границ диапазонов разностей фаз между символами. Сложность: O(1)
        const std::vector<double>& GetBoundsSymbols() const noexcept;

        /// Получить последовательность символов на окружности в соответствии с кодом Грея. Сложность: O(1)
        const std::vector<uint32_t>& GetSymbolsSequenceOnCircle() const noexcept;

        /// Определить символ на основании значения разности фаз между символами. Сложность: O(positionality)
        uint32_t DefineSymbol(double phase_difference) const noexcept;

        /// Установить значение дополнительного фазового сдвига в РАДИАНАХ. Сложность: O(positionality)
        DPSKDemodulator& SetPhaseShift(double phase_shift) override;

    private:
        /// Заполнить границы (сектора) символов на огружности. Сложность: O(positionality)
        void FillSymbolsBounds();

        /// Заполнить последовательность символов на окружности от 0 до 2*PI в соответсвии с кодом Грея. Сложность: O(2 * positionality * log2(positionality))
        void FillSymbolsSequenceOnCircle();

    private:
        // один период косинуса и синуса
        std::vector<double> cos_oscillation_;
        std::vector<double> sin_oscillation_;
        std::vector<double> bounds_symbols_; // границы диапазонов разностей фаз между символами
        std::vector<uint32_t> symbols_sequence_on_circle_; // полследовательность символов на окружности в соответствии с кодом Грея
    };

    namespace tests {
        void TestExtractInPhaseAndQuadratureComponentsSymbol();
        void TestExtractPhaseValue();
        void TestFillSymbolsBounds();
        void TestFillSymbolsSequenceOnCircle();
        void TestDefineSymbol();
        void TestDemodulation();
        void TestSetPhaseShift();
        void TestDemodulationWithPhaseShift();
        void RunAllTests();
    } // namespace tests
} // namespace dpsk_demod
