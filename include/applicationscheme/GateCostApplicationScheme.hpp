/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"
#include "dd/Definitions.hpp"
#include "operations/OpType.hpp"

#include <functional>
#include <unordered_map>
#include <utility>

namespace std {
    template<>
    struct hash<std::pair<qc::OpType, dd::QubitCount>> {
        size_t operator()(pair<qc::OpType, dd::QubitCount> const& key) const noexcept {
            size_t h1 = hash<decltype(key.first)>{}(key.first);
            size_t h2 = hash<decltype(key.second)>{}(key.second);
            return h1 ^ (h2 << 1);
        }
    };
} // namespace std

namespace ec {
    using GateCostLUTKeyType = std::pair<qc::OpType, dd::QubitCount>;
    using GateCostLUT        = std::unordered_map<GateCostLUTKeyType, std::size_t>;
    using CostFunction       = std::function<std::size_t(const GateCostLUTKeyType&)>;

    template<class DDType>
    class GateCostApplicationScheme: public ApplicationScheme<DDType> {
    public:
        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2) noexcept:
            ApplicationScheme<DDType>(taskManager1, taskManager2) {}

        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const CostFunction& costFunction);

        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const std::string& filename);

        std::pair<size_t, size_t> operator()() override;

    protected:
        GateCostLUT gateCostLUT{};

        void populateLUT(const CostFunction& costFunction, const qc::QuantumComputation* qc);

        // read gate cost LUT from file
        // very simple file format:
        // each line consists of
        // <identifier> <controls> <cost>
        void populateLUT(const std::string& filename);
        void populateLUT(std::istream& is);
    };

    inline std::size_t LegacyIBMCostFunction(const GateCostLUTKeyType& key) noexcept {
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
