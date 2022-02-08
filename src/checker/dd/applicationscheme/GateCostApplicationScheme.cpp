/*
* This file is part of JKQ QCEC library which is released under the MIT license.
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
        switch (gate) {
            case qc::I:
                return 1;

            case qc::X:
                if (nc <= 1U) {
                    return 1U;
                } else {
                    return 2U * (nc - 2U) * (2U * LegacyIBMCostFunction({qc::Phase, 0}) + 2 * LegacyIBMCostFunction({qc::U2, 0}) + 3U * LegacyIBMCostFunction({qc::X, 1})) + 6U * LegacyIBMCostFunction({qc::X, 1}) + 8U * LegacyIBMCostFunction({qc::U3, 0});
                }

            case qc::U3:
            case qc::U2:
            case qc::V:
            case qc::Vdag:
            case qc::RX:
            case qc::RY:
            case qc::H:
            case qc::SX:
            case qc::SXdag:
                if (nc == 0U) {
                    return 1U;
                }
                if (nc == 1U) {
                    return 2U * LegacyIBMCostFunction({qc::X, 1}) + 4U * LegacyIBMCostFunction({qc::U3, 0});
                } else {
                    return 2U * LegacyIBMCostFunction({qc::X, nc}) + 4U * LegacyIBMCostFunction({qc::U3, 0}); // heuristic
                }
            case qc::Phase:
            case qc::S:
            case qc::Sdag:
            case qc::T:
            case qc::Tdag:
            case qc::RZ:
                if (nc == 0U) {
                    return 1U;
                }
                if (nc == 1U) {
                    return 2U * LegacyIBMCostFunction({qc::X, 1}) + 3U * LegacyIBMCostFunction({qc::Phase, 0});
                } else {
                    return 2U * LegacyIBMCostFunction({qc::X, nc}) + 3U * LegacyIBMCostFunction({qc::U3, 0}); // heuristic
                }
            case qc::Y:
            case qc::Z:
                if (nc == 0U) {
                    return 1U;
                } else {
                    return LegacyIBMCostFunction({qc::X, nc}) + 2U * LegacyIBMCostFunction({qc::U3, 0});
                }
            case qc::SWAP:
                return LegacyIBMCostFunction({qc::X, nc}) + 2U * LegacyIBMCostFunction({qc::X, 1});

            case qc::iSWAP:
                return LegacyIBMCostFunction({qc::SWAP, nc}) + 2U * LegacyIBMCostFunction({qc::S, nc - 1U}) + LegacyIBMCostFunction({qc::Z, nc});

            case qc::Peres:
            case qc::Peresdag:
                return LegacyIBMCostFunction({qc::X, nc}) + LegacyIBMCostFunction({qc::X, nc - 1U});

            case qc::Compound: // this assumes that compound operations only arise from single qubit fusion
            case qc::Measure:
            case qc::Barrier:
            case qc::ShowProbabilities:
            case qc::Snapshot:
                return 1U; // these operations are assumed to incur no cost, but to advance the procedure 1 is used

            default:
                std::cerr << "No cost for gate " << std::to_string(gate) << " -> assuming cost of 1!" << std::endl;
                return 1U;
        }
    }
} // namespace ec
