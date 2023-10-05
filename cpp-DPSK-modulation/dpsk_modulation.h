#pragma once

#include <cstdint>
#include <vector>
#include <map>

/// Модуляция одного периода. reference_bit - опорный бит
//std::vector<double> ModulationElementrySignal(const std::vector<uint8_t>& bits, uint8_t reference_bit);

namespace dpsk_mod {
    using PhaseDifferences = std::map<std::pair<uint8_t, uint8_t>, double>;

    /// Модулятор относительной фазовой модуляции (ОФМ) любой позиционности, являющейся степенью двойки
    class DPSKModulator {
    public:
        /// По умолчанию используется двухпозиционная ОФМ
        DPSKModulator(int positionality = 2);

        /// Установить позиционность модуляции
        void SetPositionality(int positionality);

        /// Получить текущее количество позиций фаз
        uint8_t GetPositionality() const noexcept;

        /// Установить значение несущей частоты
        void SetCarrierFrequency(int carrier_frequency);

        /// Получить значение несущей частоты
        uint32_t GetCarrierFrequency() const noexcept;

        /// Установить значение частоты дискретизации
        void SetSamplingFrequency(int sampling_frequency);

        /// Получить значение частоты дискретизации
        uint32_t GetSamplingFrequency() const noexcept;

        /// Получить набор возможных сочетаний разностей фаз между двумя символами
        const PhaseDifferences& GetPhaseDifferences() const noexcept;

        /// Модуляция последовательности бит. reference_symbol - опорный бит
        std::vector<double> Modulation(const std::vector<bool>& bits, uint8_t reference_symbol) const;

        /// Модуляция последовательности бит с указанием нужной позиционности. reference_symbol - опорный бит
    //    std::vector<double> Modulation(const std::vector<bool>& bits, uint8_t reference_symbol, uint8_t positionality);

        // в дальнейшем появятся перегрузки для записи в принимаемый по НЕ константной ссылке контейнер
        // и для приёма пары итераторов, указывающих на контейнер с битами

    private:
        /// Заполнить возможные сочетания разностей фаз между двумя символами
        void FillPhaseDifferences();

    private:
        PhaseDifferences phase_differences_; // разности фаз
        uint8_t positionality_ = 0; // позиционность ОФМ
        uint32_t carrier_frequency_ = 0; // несущая частота
        uint32_t sampling_frequency_ = 0; // частота дискретизации
    };

    namespace tests {
        void TestDefaultConstructor();
        void TestSetPositionality();
        void RunAllTests();
    } // namespace tests
} // namespace dpsk_mod

