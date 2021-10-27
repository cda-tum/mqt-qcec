/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "applicationscheme/GateCostApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    void GateCostApplicationScheme<DDType>::populateLUT(const std::function<std::size_t(GateCostLUTKeyType)>& costFunction, const qc::QuantumComputation* qc) {
        for (const auto& op: *qc) {
            const auto               type      = op->getType();
            const auto               nControls = op->getNcontrols();
            const GateCostLUTKeyType key       = {type, nControls};
            const auto               cost      = costFunction(key);
            gateCostLUT.emplace(key, cost);
        }
    }

    template<class DDType>
    std::pair<size_t, size_t> GateCostApplicationScheme<DDType>::operator()() {
        const auto& op   = this->taskManager1();
        auto        key  = GateCostLUTKeyType{op->getType(), op->getNcontrols()};
        std::size_t cost = 1U;
        if (auto it = gateCostLUT.find(key); it != gateCostLUT.end()) {
            cost = it->second;
        }
        return std::pair{1U, cost};
    }

    template<class DDType>
    GateCostApplicationScheme<DDType>::GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const std::function<size_t(GateCostLUTKeyType)>& costFunction):
        ApplicationScheme<DDType>(taskManager1, taskManager2) {
        populateLUT(costFunction, taskManager1.getCircuit());
        populateLUT(costFunction, taskManager2.getCircuit());
    }

    template<class DDType>
    GateCostApplicationScheme<DDType>::GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, [[maybe_unused]] const std::string& filename):
        ApplicationScheme<DDType>(taskManager1, taskManager2) {
        // TODO: Provide a constructor that allows to read in a LUT from a file
    }

    template class GateCostApplicationScheme<qc::VectorDD>;
    template class GateCostApplicationScheme<qc::MatrixDD>;
} // namespace ec
