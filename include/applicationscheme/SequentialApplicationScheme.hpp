/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    class SequentialApplicationScheme: public ApplicationScheme<DDType> {
    public:
        SequentialApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2):
            ApplicationScheme<DDType>(taskManager1, taskManager2) {
            ngates1 = taskManager1.getCircuit()->getNops();
            ngates2 = taskManager2.getCircuit()->getNops();
        }

        std::pair<size_t, size_t> operator()() override {
            return {ngates1, ngates2};
        }

    protected:
        std::size_t ngates1{};
        std::size_t ngates2{};
    };
} // namespace ec
