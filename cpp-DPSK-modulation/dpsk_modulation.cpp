#include "dpsk_modulation.h"
#include "math_operations.h"
#include "gray_code.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

namespace dpsk_mod {
    DPSKModulator::DPSKModulator(int positionality) {
        SetPositionality(positionality);
    }

    void DPSKModulator::SetPositionality(int positionality) {
        if (!math::IsPowerOfTwo(positionality)) {
            throw invalid_argument("Positionality is not a power of two."s);
        }
        positionality_ = positionality;
        FillPhaseDifferences();
    }

    uint8_t DPSKModulator::GetPositionality() const noexcept {
        return positionality_;
    }

    const PhaseDifferences& DPSKModulator::GetPhaseDifferences() const noexcept {
        return phase_differences_;
    }

    void DPSKModulator::FillPhaseDifferences() {
        phase_differences_.clear();
        static constexpr double kTotalAngle = 360; // количество градусов на окружности
        const double kStepPhase = kTotalAngle / positionality_;
        vector<vector<bool>> grey_codes = gray_code::MakeGrayCodes(positionality_);

        // перебор всех возможных сочетаний символов
        for (uint8_t left_symbol_id = 0; left_symbol_id < positionality_ - 1; ++left_symbol_id) {
            double current_phase = kStepPhase;

            for (uint8_t right_symbol_id = left_symbol_id + 1; right_symbol_id < positionality_; ++right_symbol_id) {
                pair<uint8_t, uint8_t> symbols_pair = make_pair(math::ConvertationBinToDec(grey_codes[left_symbol_id]), math::ConvertationBinToDec(grey_codes[right_symbol_id]));
                phase_differences_.emplace(move(symbols_pair), current_phase);
                current_phase += kStepPhase;
            }
        }
    }

    std::vector<double> DPSKModulator::Modulation(const std::vector<bool>& bits, uint8_t reference_symbol) const {
        return {};
    }
} // namespace dpsk_mod
