#pragma once

#include <cstdint>

class SignalParameters {
public:
    SignalParameters(int sampling_frequency, int symbol_speed);

    /// Установить позиционность модуляции. Сложность: O(1)
    virtual SignalParameters& SetPositionality(int positionality);

    /// Получить текущее количество позиций фаз. Сложность: O(1)
    uint16_t GetPositionality() const noexcept;

    /// Установить значение амплитуды сигнала. Сложность: O(1)
    SignalParameters& SetAmplitude(double new_amplitude);

    /// Получить значение амплитуды сигнала. Сложность: O(1)
    double GetAmplitude() const noexcept;

    /// Установить текущее значение фазы. Позволяет сдвинуть сигнальное созвездие на заданный в РАДИАНАХ угол.
    /// Значение со знаком "+" - сдвиг по против часовой стрелки; со знаком "-" - по часовой стрелке
    SignalParameters& SetPhase(double new_phase) noexcept;

    /// Получить текущее значение фазы. Сложность: O(1)
    double GetPhase() const noexcept;

    /// Установить значение несущей частоты. Сложность: O(1)
    virtual SignalParameters& SetCarrierFrequency(int carrier_frequency);

    /// Получить значение несущей частоты. Сложность: O(1)
    uint32_t GetCarrierFrequency() const noexcept;

    /// Получить значение частоты дискретизации. Сложность: O(1)
    uint32_t GetSamplingFrequency() const noexcept;

    /// Установить значение дополнительного фазового сдвига в РАДИАНАХ. Сложность: O(1)
    virtual SignalParameters& SetPhaseShift(double phase_shift);

    /// Получить значение дополнительного фазового сдвига в РАДИАНАХ. Сложность: O(1)
    double GetPhaseShift() const noexcept;

    /// Получить значение символьной скорости. Сложность: O(1)
    uint32_t GetSymbolSpeed() const noexcept;

protected:
    uint16_t positionality_ = 0u; // позиционность
    double amplitude_ = 1.; // амплитуда колебания, В
    double phase_ = 0.; // текущая фаза, радианы
    double phase_shift_ = 0; // дополнительный фазовый сдвиг, радианы
    uint32_t symbol_speed_ = 0u; // символьная скорость [символ/с]

    uint32_t carrier_frequency_ = 0u; // несущая частота, Гц
    double carrier_cyclic_frequency_ = 0.; // циклическая несущая частота, радианы

    uint32_t sampling_frequency_ = 0u; // частота дискретизации, Гц
    double time_step_between_samples_ = 0.; // шаг дискретизации во временной области, с
};
