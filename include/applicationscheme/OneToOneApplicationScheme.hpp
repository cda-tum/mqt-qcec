/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_ONETOONEAPPLICATIONSCHEME_HPP
#define QCEC_ONETOONEAPPLICATIONSCHEME_HPP

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    class OneToOneApplicationScheme: public ApplicationScheme<DDType> {
    public:
        OneToOneApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2):
            ApplicationScheme<DDType>(taskManager1, taskManager2) {}

        std::pair<size_t, size_t> operator()() override {
            return {1U, 1U};
        }
    };
} // namespace ec

#endif //QCEC_ONETOONEAPPLICATIONSCHEME_HPP
