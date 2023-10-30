#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <complex>

namespace dpsk_mod {
    struct InPhaseAndQuadratureComponents {
        /// Извлечь синфазную и квадратурную составляющие символа. Сложность: O(N)
        std::complex<double> ExtractInPhaseAndQuadratureComponentsSymbol(const std::vector<double>& one_symbol_samples) const;

        // один период косинуса и синуса
        std::vector<double> cos_oscillation;
        std::vector<double> sin_oscillation;
    };

    /// Модулятор относительной фазовой модуляции (ОФМ) любой позиционности, являющейся степенью двойки
    /// https://ru.dsplib.org/content/signal_qpsk/signal_qpsk.html
    class DPSKModulator {
    public:
        /// По умолчанию используется двухпозиционная ОФМ. Сложность: O(positionality)
        DPSKModulator(int positionality = 2);

        /// Установить позиционность модуляции. Сложность: O(positionality)
        DPSKModulator& SetPositionality(int positionality);

        /// Получить текущее количество позиций фаз. Сложность: O(1)
        uint16_t GetPositionality() const noexcept;

        /// Установить значение несущей частоты. Сложность: O(1)
        DPSKModulator& SetCarrierFrequency(int carrier_frequency);

        /// Получить значение несущей частоты. Сложность: O(1)
        uint32_t GetCarrierFrequency() const noexcept;

        /// Установить значение промежуточной частоты (на случай не кратности частоты дискретизации несущей). Сложность: O(1)
        DPSKModulator& SetIntermediateFrequency(int intermediate_frequency);

        /// Получить значение промежуточной частоты. Сложность: O(1)
        uint32_t GetIntermediateFrequency() const noexcept;

        /// Установить значение частоты дискретизации. Сложность: O(1)
        DPSKModulator& SetSamplingFrequency(int sampling_frequency);

        /// Получить значение частоты дискретизации. Сложность: O(1)
        uint32_t GetSamplingFrequency() const noexcept;

        /// Установить текущее значение фазы. Позволяет сдвинуть сигнальное созвездие на заданный в ГРАДУСАХ угол.
        /// Значение со знаком "+" - сдвиг по против часовой стрелки; со знаком "-" - по часовой стрелке
        DPSKModulator& SetPhase(double new_phase);

        /// Получить текущее значение фазы. Сложность: O(1)
        double GetPhase() const noexcept;

        /// Получить словарь символов с соответствующими сдвигами по фазе <символ, фазовый сдвиг>. Сложность: O(1)
        const std::map<uint16_t, double>& GetPhaseShifts() const noexcept;

        /// Модуляция последовательности бит. Сложность: O(N)
        /// Если количество бит не кратно установленной позиционности, то дописываются нулевые биты до ближайшей степени двойки
        std::vector<double> Modulation(const std::vector<bool>& bits);

        /// Модуляция последовательности бит с указанием нужной позиционности. Сложность: O(positionality + N)
        std::vector<double> Modulation(const std::vector<bool>& bits, int positionality);

        // в дальнейшем появятся перегрузки для записи в принимаемый по НЕ константной ссылке контейнер
        // и для приёма пары итераторов, указывающих на контейнер с битами

        /// Демодуляция последовательности отсчетов. Сложность: O(???)
        std::vector<bool> Demodulation(const std::vector<double>& samples);

    private:
        /// Заполнить словарь со значениями сдвигов фаз. Сложность: O(positionality)
        void FillPhaseShifts();

        /// Модуляция одного символа. Сложность: O(sampling_frequency / carrier_frequency)
        void ModulationOneSymbol(std::vector<double>::iterator begin_samples, std::vector<double>::iterator end_samples, uint16_t current_symbol, double& phase) const;

        /// Модуляция без изпользования промежуточной частоты. Частота дискретизации кратна несущей частоте. Сложность: O(N)
        void ClassicalModulation(const std::vector<uint32_t>& symbols, std::vector<double>& modulated_signal, uint16_t num_samples_in_symbol);

        /// Модуляция с изпользованием промежуточной частоты. Частота дискретизации НЕ кратна несущей частоте. Сложность: O(N)
        void ModulationWithUseIntermediateFreq(const std::vector<uint32_t>& symbols, std::vector<double>& modulated_signal, uint16_t num_samples_in_symbol);

        /// Сгенерировать один период косинуса и синуса при заданных параметрах. Сложность(sampling_frequency / carrier_frequency)
        void FillCosAndSinOscillation();


    private:
        std::map<uint16_t, double>  phase_shifts_; // фазовые сдвиги, соответствующие символу
        uint16_t positionality_ = 0u; // позиционность ОФМ
        double amplitude_ = 1.0; // амплитуда колебания
        double phase_ = 0; // текущая фаза
        InPhaseAndQuadratureComponents IpQ_components_;

        uint32_t carrier_frequency_ = 0u; // несущая частота, Гц
        double carrier_cyclic_frequency_ = 0.0; // циклическая несущая частота, радианы

        uint32_t sampling_frequency_ = 0u; // частота дискретизации, Гц
        double time_step_between_samples_ = 0.0; // шаг дискретизации во временной области, с

        uint32_t intermediate_frequency_ = 1000u; // промежуточная частота, Гц
        double intermediate_cyclic_frequency_ = 0.0; // циклическая промежуточная частота, радианы
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

