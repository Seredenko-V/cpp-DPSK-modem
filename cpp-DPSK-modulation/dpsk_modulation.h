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

    /// Модулятор относительной фазовой модуляции (ОФМ) любой позиционности, являющейся степенью двойки
    /// https://ru.dsplib.org/content/signal_qpsk/signal_qpsk.html
    class DPSKModulator : public SignalParameters {
    public:
        /// По умолчанию используется двухпозиционная ОФМ. Сложность: O(positionality)
        DPSKModulator(int positionality = 2);

        /// Установить позиционность модуляции. Сложность: O(positionality)
        DPSKModulator& SetPositionality(int positionality);

        /// Установить модулирующую и ортогональную ей функции: cos/sin. Сложность: O(1)
        DPSKModulator& SetModulationFunction(std::function<double(double)> mod_function);

        /// Получить словарь символов с соответствующими сдвигами по фазе <символ, фазовый сдвиг>. Сложность: O(1)
        const std::map<uint16_t, double>& GetPhaseShifts() const noexcept;

        /// Модуляция последовательности бит. Сложность: O(N)
        /// Если количество бит не кратно установленной позиционности, то дописываются нулевые биты до ближайшей степени двойки
        std::vector<double> Modulation(const std::vector<bool>& bits);

        /// Модуляция последовательности бит с указанием нужной позиционности. Сложность: O(positionality + N)
        std::vector<double> Modulation(const std::vector<bool>& bits, int positionality);

        // в дальнейшем появятся перегрузки для записи в принимаемый по НЕ константной ссылке контейнер
        // и для приёма пары итераторов, указывающих на контейнер с битами

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
        void TestClassicalModulation(); // модуляция без переноса на промежуточную несущую
        void TestModulationWithUseIntermediateFreq(); // модуляция с переносом на промежуточную несущую
        void TestConstellationShift(); // сдвиг созвездия
        void RunAllTests();
    } // namespace tests
} // namespace dpsk_mod

