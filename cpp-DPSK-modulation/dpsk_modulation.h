#pragma once

#include <cstdint>
#include <vector>
#include <map>

/// Модуляция одного периода. reference_bit - опорный бит
//std::vector<double> ModulationElementrySignal(const std::vector<uint8_t>& bits, uint8_t reference_bit);

namespace dpsk_mod {
    using PhaseDifferences = std::map<std::pair<uint16_t, uint16_t>, double>;

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

        /// Получить набор возможных сочетаний разностей фаз между двумя символами
        const PhaseDifferences& GetPhaseDifferences() const noexcept;

        /// Получить разность фаз между двумя символами
        double GetDifferentPhaseBetweenSymbols(uint16_t left, uint16_t right) const;

        /// Модуляция одного символа
        void ModulationOneSymbol(std::vector<double>::iterator begin_samples, std::vector<double>::iterator end_samples,
                                 uint16_t reference_symbol, uint16_t current_symbol, double phase) const;

        /// Модуляция последовательности бит. reference_symbol - опорный бит.
        /// Если количество бит не кратно установленной позиционности, то дописываются нулевые биты до ближайшей степени двойки
        std::vector<double> Modulation(const std::vector<bool>& bits, uint16_t reference_symbol, double phase = 0) const;

        /// Модуляция последовательности бит с указанием нужной позиционности. reference_symbol - опорный бит
    //    std::vector<double> Modulation(const std::vector<bool>& bits, uint8_t reference_symbol, uint8_t positionality);

        // в дальнейшем появятся перегрузки для записи в принимаемый по НЕ константной ссылке контейнер
        // и для приёма пары итераторов, указывающих на контейнер с битами

    private:
        /// Заполнить возможные сочетания разностей фаз между двумя символами
        void FillPhaseDifferences();

    private:
        PhaseDifferences phase_differences_; // разности фаз между символами
        uint16_t positionality_ = 0; // позиционность ОФМ
        uint32_t carrier_frequency_ = 0; // несущая частота
        uint32_t sampling_frequency_ = 0; // частота дискретизации
        double amplitude_ = 1.0; // амплитуда колебания
    };

    namespace tests {
        void TestDefaultConstructor();
        void TestSetPositionality(); // установление позиционности с заполнением словаря разностей фаз
        void TestGetDifferentPhaseBetweenSymbols(); // разность фаз между двумя символами
        void TestModulationOnlyBits(); // перегрузка, принимающая только вектор бит и опорный символ
        void RunAllTests();
    } // namespace tests
} // namespace dpsk_mod

