/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    class ProportionalApplicationScheme final: public ApplicationScheme<DDType> {
    public:
        ProportionalApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2):
            ApplicationScheme<DDType>(taskManager1, taskManager2),
            gateRatio(computeGateRatio()) {}

        std::pair<size_t, size_t> operator()() noexcept final {
            return {1U, gateRatio};
        }

    protected:
        [[nodiscard]] std::size_t computeGateRatio() const noexcept {
            const std::size_t size1 = this->taskManager1.getCircuit()->size();
            const std::size_t size2 = this->taskManager2.getCircuit()->size();
            return std::max((size2 + size1 / 2U) / size1, static_cast<std::size_t>(1U));
        }

        const std::size_t gateRatio;
    };
} // namespace ec
