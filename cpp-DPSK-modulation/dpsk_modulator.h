#pragma once
#include "signal_parameters.h"

#include <cstdint>
#include <vector>
#include <map>
#include <complex>
#include <functional>
#include <cmath>

namespace dpsk_mod {
    static std::function<double(double)> Sin = (double(*)(double))&std::sin;
    static std::function<double(double)> Cos = (double(*)(double))&std::cos;

    /// Присуствует ли опорный символ в последовательности бит
    enum class PresencePivotSymbol {
        WITH_PIVOT,
        WITHOUT_PIVOT
    };

    /// Модулятор относительной фазовой модуляции (ОФМ) любой позиционности, являющейся степенью двойки
    /// https://ru.dsplib.org/content/signal_qpsk/signal_qpsk.html
    class DPSKModulator : public SignalParameters {
    public:
        /// По умолчанию используется двухпозиционная ОФМ. Сложность: O(2 * positionality * log2(positionality))
        DPSKModulator(int sampling_frequency, int symbol_speed, int positionality = 2);

        /// Установить позиционность модуляции. Сложность: O(2 * (positionality * log2(positionality)))
        DPSKModulator& SetPositionality(int positionality) override;

        /// Установить модулирующую и ортогональную ей функции: cos/sin. Сложность: O(1)
        DPSKModulator& SetModulationFunction(std::function<double(double)> mod_function);

        /// Получить словарь символов с соответствующими сдвигами по фазе <символ, фазовый сдвиг>. Сложность: O(1)
        const std::map<uint16_t, double>& GetPhaseShifts() const noexcept;

        /// Установить значение дополнительного фазового сдвига в РАДИАНАХ. Сложность: O(positionality)
        DPSKModulator& SetPhaseShift(double phase_shift) override;

        /// Модуляция последовательности бит.
        /// Сложность: O(N) - если опорный символ содержится в bits
        /// Сложность: O(2*N) - если опорный символ НЕ содержится в bits и его нужно добавить в начало
        /// Если количество бит не кратно установленной позиционности, то дописываются нулевые биты до ближайшей степени двойки
        std::vector<double> Modulation(const std::vector<bool>& bits, PresencePivotSymbol is_presence = PresencePivotSymbol::WITHOUT_PIVOT);

        /// Модуляция последовательности бит с указанием нужной позиционности. Сложность: O(positionality + N)
        std::vector<double> Modulation(const std::vector<bool>& bits, int positionality, PresencePivotSymbol is_presence = PresencePivotSymbol::WITHOUT_PIVOT);

        /// Модуляция одного символа. Сложность: O(sampling_frequency / carrier_frequency)
        void ModulationOneSymbol(std::vector<double>::iterator begin_samples, std::vector<double>::iterator end_samples, uint16_t current_symbol, double& phase) const;

    private:
        /// Заполнить словарь со значениями сдвигов фаз. Сложность: O(positionality)
        void FillPhaseShifts();

        /// Модуляция без изпользования промежуточной частоты. Частота дискретизации кратна несущей частоте. Сложность: O(N)
        void ClassicalModulation(const std::vector<uint32_t>& symbols, std::vector<double>& modulated_signal, uint16_t num_samples_in_symbol);

        /// Модуляция с изпользованием промежуточной частоты. Частота дискретизации НЕ кратна несущей частоте. Сложность: O(N)
        void ModulationWithUseIntermediateFreq(const std::vector<uint32_t>& symbols, std::vector<double>& modulated_signal, uint16_t num_samples_in_symbol);

    private:
        std::map<uint16_t, double>  phase_shifts_; // фазовые сдвиги в градусах, соответствующие символу
        std::function<double(double)> mod_function_ = Sin;
        std::function<double(double)> ortogonal_mod_function_ = Cos;
    };

    namespace tests {
        void TestDefaultConstructor();
        void TestSetPositionality(); // установление позиционности с заполнением словаря разностей фаз
        void TestSetPhaseShift(); // установление доп. фазового сдвига между символами
        void TestClassicalModulation(); // модуляция без переноса на промежуточную несущую
        void TestModulationWithUseIntermediateFreq(); // модуляция с переносом на промежуточную несущую
        void TestConstellationShift(); // сдвиг созвездия
        void RunAllTests();
    } // namespace tests
} // namespace dpsk_mod

