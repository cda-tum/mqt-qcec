/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    void GateCostApplicationScheme<DDType>::populateLUT(const CostFunction& costFunction, const qc::QuantumComputation* qc) {
        for (const auto& op: *qc) {
            const auto type      = op->getType();
            const auto nControls = op->getNcontrols();
            const auto key       = GateCostLUTKeyType{type, nControls};
            if (const auto it = gateCostLUT.find(key); it == gateCostLUT.end()) {
                const auto cost = costFunction(key);
                gateCostLUT.emplace(key, cost);
            }
        }
    }

    template<class DDType>
    std::pair<size_t, size_t> GateCostApplicationScheme<DDType>::operator()() {
        if (gateCostLUT.empty())
            return {1U, 1U};

        const auto& op   = this->taskManager1();
        const auto  key  = GateCostLUTKeyType{op->getType(), op->getNcontrols()};
        std::size_t cost = 1U;
        if (auto it = gateCostLUT.find(key); it != gateCostLUT.end()) {
            cost = it->second;
        }
        return {1U, cost};
    }

    template<class DDType>
    GateCostApplicationScheme<DDType>::GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const CostFunction& costFunction):
        ApplicationScheme<DDType>(taskManager1, taskManager2) {
        populateLUT(costFunction, taskManager1.getCircuit());
        populateLUT(costFunction, taskManager2.getCircuit());
    }

    template<class DDType>
    GateCostApplicationScheme<DDType>::GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const std::string& filename):
        ApplicationScheme<DDType>(taskManager1, taskManager2) {
        populateLUT(filename);
    }
    template<class DDType>
    void GateCostApplicationScheme<DDType>::populateLUT(const std::string& filename) {
        std::ifstream ifs(filename);
        if (!ifs.good()) {
            throw std::runtime_error("Error opening LUT file: " + filename);
        }
        populateLUT(ifs);
    }
    template<class DDType>
    void GateCostApplicationScheme<DDType>::populateLUT(std::istream& is) {
        qc::OpType  opType    = qc::OpType::None;
        std::size_t nControls = 0U;
        std::size_t cost      = 1U;
        while (is.good() && (is >> opType >> nControls >> cost)) {
            gateCostLUT.emplace(std::pair{opType, nControls}, cost);
        }
    }

    template class GateCostApplicationScheme<qc::VectorDD>;
    template class GateCostApplicationScheme<qc::MatrixDD>;

    std::size_t LegacyIBMCostFunction(const GateCostLUTKeyType& key) noexcept {
        const auto [gate, nc] = key;

        if (nc == 0U) {
            switch (gate) {
                case qc::I:
                case qc::H:
                case qc::X:
                case qc::Y:
                case qc::Z:
                case qc::S:
                case qc::Sdag:
                case qc::T:
                case qc::Tdag:
                case qc::Phase:
                case qc::U2:
                case qc::U3:
                case qc::SX:
                case qc::SXdag:
                case qc::V:
                case qc::Vdag:
                case qc::RX:
                case qc::RY:
                case qc::RZ:
                // the following are merely placeholders so that the check can advance
                case qc::Compound:
                case qc::Measure:
                case qc::Reset:
                case qc::Snapshot:
                case qc::ShowProbabilities:
                case qc::Barrier:
                case qc::ClassicControlled:
                    return 1U;
                default:
                    break;
            }
        }

        // special treatment for CNOT
        if (gate == qc::X && nc == 1) {
            return 1U;
        }

        switch (gate) {
            case qc::X:
                return 2U * (nc - 2U) * (2U * LegacyIBMCostFunction({qc::Phase, 0}) + 2 * LegacyIBMCostFunction({qc::U2, 0}) + 3U * LegacyIBMCostFunction({qc::X, 1})) + 6U * LegacyIBMCostFunction({qc::X, 1}) + 8U * LegacyIBMCostFunction({qc::U3, 0});
            case qc::U3:
            case qc::U2:
            case qc::V:
            case qc::Vdag:
            case qc::RX:
            case qc::RY:
            case qc::H:
            case qc::SX:
            case qc::SXdag:
                return 2U * LegacyIBMCostFunction({qc::X, nc}) + 4U * LegacyIBMCostFunction({qc::U3, 0}); // heuristic
            case qc::Phase:
            case qc::S:
            case qc::Sdag:
            case qc::T:
            case qc::Tdag:
            case qc::RZ:
                return 2U * LegacyIBMCostFunction({qc::X, nc}) + 3U * LegacyIBMCostFunction({qc::Phase, 0}); // heuristic
            case qc::Y:
            case qc::Z:
                return LegacyIBMCostFunction({qc::X, nc}) + 2U * LegacyIBMCostFunction({qc::U3, 0});
            case qc::SWAP:
                return LegacyIBMCostFunction({qc::X, nc}) + 2U * LegacyIBMCostFunction({qc::X, 1});
            case qc::iSWAP:
                return 2U * LegacyIBMCostFunction({qc::X, nc + 1U}) + 2U * LegacyIBMCostFunction({qc::S, nc}) + 2U * LegacyIBMCostFunction({qc::H, nc});
            case qc::Peres:
            case qc::Peresdag:
                return LegacyIBMCostFunction({qc::X, nc + 1U}) + LegacyIBMCostFunction({qc::X, nc});
            default:
                return 1U;
        }
    }
} // namespace ec
