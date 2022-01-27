/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    class OneToOneApplicationScheme final: public ApplicationScheme<DDType> {
    public:
        OneToOneApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2) noexcept:
            ApplicationScheme<DDType>(taskManager1, taskManager2) {}

        std::pair<size_t, size_t> operator()() final {
            return {1U, 1U};
        }
    };
} // namespace ec
