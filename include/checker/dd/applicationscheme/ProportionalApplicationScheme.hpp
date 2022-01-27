/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    class ProportionalApplicationScheme final: public ApplicationScheme<DDType> {
    public:
        ProportionalApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2);

        std::pair<size_t, size_t> operator()() noexcept final;

    private:
        const std::size_t size1{};
        const std::size_t size2{};

        const bool        firstIsLarger = false;
        const std::size_t gateRatio     = 1U;

        [[nodiscard]] std::size_t computeGateRatio() const noexcept {
            const auto max = firstIsLarger ? size1 : size2;
            const auto min = firstIsLarger ? size2 : size1;
            return std::max((max + min / 2U) / min, static_cast<std::size_t>(1U));
        }
    };
} // namespace ec
