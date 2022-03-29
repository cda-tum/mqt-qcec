/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDType, class DDPackage = dd::Package<>>
    class OneToOneApplicationScheme final: public ApplicationScheme<DDType, DDPackage> {
    public:
        OneToOneApplicationScheme(TaskManager<DDType, DDPackage>& taskManager1, TaskManager<DDType, DDPackage>& taskManager2) noexcept:
            ApplicationScheme<DDType, DDPackage>(taskManager1, taskManager2) {}

        std::pair<size_t, size_t> operator()() final {
            return {1U, 1U};
        }
    };
} // namespace ec
