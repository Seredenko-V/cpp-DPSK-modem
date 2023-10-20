#pragma once

#include <cstdint>
#include <vector>
#include <map>

namespace dpsk_mod {
    /// Модулятор относительной фазовой модуляции (ОФМ) любой позиционности, являющейся степенью двойки
    class DPSKModulator {
    public:
        /// По умолчанию используется двухпозиционная ОФМ
        DPSKModulator(int positionality = 2);

        /// Установить позиционность модуляции
        DPSKModulator& SetPositionality(int positionality);

        /// Получить текущее количество позиций фаз
        uint16_t GetPositionality() const noexcept;

        /// Установить значение несущей частоты
        DPSKModulator& SetCarrierFrequency(int carrier_frequency);

        /// Получить значение несущей частоты
        uint32_t GetCarrierFrequency() const noexcept;

        /// Установить значение промежуточной частоты (на случай не кратности частоты дискретизации несущей)
        DPSKModulator& SetIntermediateFrequency(int intermediate_frequency);

        /// Получить значение промежуточной частоты
        uint32_t GetIntermediateFrequency() const noexcept;

        /// Установить значение частоты дискретизации
        DPSKModulator& SetSamplingFrequency(int sampling_frequency);

        /// Получить значение частоты дискретизации
        uint32_t GetSamplingFrequency() const noexcept;

        /// Установить текущее значение фазы
        DPSKModulator& SetPhase(double new_phase);

        /// Получить текущее значение фазы
        double GetPhase() const noexcept;

        /// Получить словарь символов с соответствующими сдвигами по фазе <символ, фазовый сдвиг>
        const std::map<uint16_t, double>& GetPhaseShifts() const noexcept;

        /// Модуляция последовательности бит.
        /// Если количество бит не кратно установленной позиционности, то дописываются нулевые биты до ближайшей степени двойки
        std::vector<double> Modulation(const std::vector<bool>& bits);

        /// Модуляция последовательности бит с указанием нужной позиционности
        std::vector<double> Modulation(const std::vector<bool>& bits, int positionality);

        // в дальнейшем появятся перегрузки для записи в принимаемый по НЕ константной ссылке контейнер
        // и для приёма пары итераторов, указывающих на контейнер с битами

    private:
        /// Заполнить словарь со значениями сдвигов фаз
        void FillPhaseShifts();

        /// Модуляция одного символа
        void ModulationOneSymbol(std::vector<double>::iterator begin_samples, std::vector<double>::iterator end_samples, uint16_t current_symbol, double& phase);

        /// Модуляция без изпользования промежуточной частоты. Частота дискретизации кратна несущей частоте. Сложность: O(N)
        void ClassicalModulation(const std::vector<uint32_t>& symbols, std::vector<double>& modulated_signal, uint16_t num_samples_in_symbol);

        /// Модуляция с изпользованием промежуточной частоты. Частота дискретизации НЕ кратна несущей частоте. Сложность: O(N)
        void ModulationWithUseIntermediateFreq(const std::vector<uint32_t>& symbols, std::vector<double>& modulated_signal, uint16_t num_samples_in_symbol);

    private:
        std::map<uint16_t, double>  phase_shifts_; // фазовые сдвиги, соответствующие символу
        uint16_t positionality_ = 0u; // позиционность ОФМ
        uint32_t carrier_frequency_ = 0u; // несущая частота
        uint32_t sampling_frequency_ = 0u; // частота дискретизации
        uint32_t intermediate_carrier_frequency_ = 1000u; // промежуточная частота, Гц
        double amplitude_ = 1.0; // амплитуда колебания
        double phase_ = 0; // текущая фаза
    };

    namespace tests {
        void TestDefaultConstructor();
        void TestSetPositionality(); // установление позиционности с заполнением словаря разностей фаз
        void TestModulationOnlyBits(); // перегрузка, принимающая только вектор бит
        void RunAllTests();
    } // namespace tests
} // namespace dpsk_mod

