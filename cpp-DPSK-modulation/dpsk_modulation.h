#pragma once

#include <cstdint>
#include <vector>
#include <map>

/// Модуляция одного периода. reference_bit - опорный бит
std::vector<double> ModulationElementrySignal(const std::vector<uint8_t>& bits, uint8_t reference_bit);


/// Модулятор относительной фазовой модуляции (ОФМ) любой позиционности, являющейся степенью двойки
class DPSKModulator {
public:
    DPSKModulator() = default;
    DPSKModulator(int positionality);

    /// Установить позиционность модуляции
    void SetPositionality(int positionality);

    /// Получить текущее количество позиций фаз
    uint8_t GetPositionality() const noexcept;

    /// Модуляция последовательности бит. reference_bit - опорный бит
    std::vector<double> Modulation(const std::vector<uint8_t>& bits, uint8_t reference_bit) const;

    /// Модуляция последовательности бит с указанием нужной позиционности. reference_bit - опорный бит
    std::vector<double> Modulation(const std::vector<uint8_t>& bits, uint8_t reference_bit, uint8_t positionality);

    // в дальнейшем появятся перегрузки для записи в принимаемый по НЕ константной ссылке контейнер
    // и для приёма пары итераторов, указывающих на контейнер с битами

private:
    std::map<std::pair<uint8_t, uint8_t>, double> phase_differences_; // разности фаз
    uint8_t positionality_ = 2; // позиционность ОФМ
};
