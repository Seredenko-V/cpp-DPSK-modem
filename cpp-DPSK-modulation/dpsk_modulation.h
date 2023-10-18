#pragma once

#include <cstdint>
#include <vector>
#include <map>

/// Модуляция одного периода. reference_bit - опорный бит
//std::vector<double> ModulationElementrySignal(const std::vector<uint8_t>& bits, uint8_t reference_bit);

namespace dpsk_mod {
    //using PhaseDifferences = std::map<std::pair<uint16_t, uint16_t>, double>;

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

        /// Модуляция одного символа
        void ModulationOneSymbol(std::vector<double>::iterator begin_samples, std::vector<double>::iterator end_samples, uint16_t current_symbol, double& phase);

        /// Модуляция последовательности бит.
        /// Если количество бит не кратно установленной позиционности, то дописываются нулевые биты до ближайшей степени двойки
        std::vector<double> Modulation(const std::vector<bool>& bits);

        /// Модуляция последовательности бит с указанием нужной позиционности. reference_symbol - опорный бит
    //    std::vector<double> Modulation(const std::vector<bool>& bits, uint8_t reference_symbol, uint8_t positionality);

        // в дальнейшем появятся перегрузки для записи в принимаемый по НЕ константной ссылке контейнер
        // и для приёма пары итераторов, указывающих на контейнер с битами

    private:
        /// Заполнить возможные сочетания разностей фаз между двумя символами
        void FillPhaseDifferences();

    private:
        std::map<uint16_t, double>  phase_differences_; // разности фаз между символами
        uint16_t positionality_ = 0; // позиционность ОФМ
        uint32_t carrier_frequency_ = 0; // несущая частота
        uint32_t sampling_frequency_ = 0; // частота дискретизации
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

