/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_APPLICATIONSCHEME_HPP
#define QCEC_APPLICATIONSCHEME_HPP

#include "QuantumComputation.hpp"
#include "TaskManager.hpp"

namespace ec {
    // Abstract base class for describing an application scheme
    template<class DDType>
    class ApplicationScheme {
    public:
        ApplicationScheme(const TaskManager<DDType>& taskManager1, const TaskManager<DDType>& taskManager2):
            taskManager1(taskManager1), taskManager2(taskManager2){};

        virtual ~ApplicationScheme() = default;

        // get how many gates from either circuit shall be applied next
        virtual std::pair<std::size_t, std::size_t> operator()() = 0;

    protected:
        const TaskManager<DDType>& taskManager1;
        const TaskManager<DDType>& taskManager2;
    };
} // namespace ec
#endif //QCEC_APPLICATIONSCHEME_HPP
