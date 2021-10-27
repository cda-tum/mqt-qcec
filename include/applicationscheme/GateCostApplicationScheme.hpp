/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_GATECOSTAPPLICATIONSCHEME_HPP
#define QCEC_GATECOSTAPPLICATIONSCHEME_HPP

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

    template<class DDType>
    class GateCostApplicationScheme: public ApplicationScheme<DDType> {
    public:
        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2):
            ApplicationScheme<DDType>(taskManager1, taskManager2) {}

        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, GateCostLUT& gateCostLUT):
            ApplicationScheme<DDType>(taskManager1, taskManager2), gateCostLUT(gateCostLUT) {}

        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const std::function<std::size_t(GateCostLUTKeyType)>& costFunction):
            ApplicationScheme<DDType>(taskManager1, taskManager2) {
            generateLUT(costFunction, taskManager1.getCircuit());
            generateLUT(costFunction, taskManager2.getCircuit());
        }

        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const std::string& filename):
            ApplicationScheme<DDType>(taskManager1, taskManager2) {
            // TODO: Provide a constructor that allows to read in a LUT from a file
        }

        std::pair<size_t, size_t> operator()() override {
            const auto  op   = this->taskManager1();
            auto        key  = GateCostLUTKeyType{op->getType(), op->getNcontrols()};
            std::size_t cost = 1U;
            if (auto it = gateCostLUT.find(key); it != gateCostLUT.end()) {
                cost = it->second;
            }
            return std::pair{1U, cost};
        }

    protected:
        GateCostLUT gateCostLUT{};

        void generateLUT(const std::function<std::size_t(GateCostLUTKeyType)>& costFunction, qc::QuantumComputation* qc) {
            for (const auto& op: *qc) {
                const auto               type      = op->getType();
                const auto               nControls = op->getNcontrols();
                const GateCostLUTKeyType key       = {type, nControls};
                const auto               cost      = costFunction(key);
                gateCostLUT.emplace(key, cost);
            }
        }
    };
} // namespace ec

#endif //QCEC_GATECOSTAPPLICATIONSCHEME_HPP
