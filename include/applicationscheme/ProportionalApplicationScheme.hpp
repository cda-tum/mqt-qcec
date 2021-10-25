/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_PROPORTIONALAPPLICATIONSCHEME_HPP
#define QCEC_PROPORTIONALAPPLICATIONSCHEME_HPP

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDType>
    class ProportionalApplicationScheme: public ApplicationScheme<DDType> {
    public:
        ProportionalApplicationScheme(const TaskManager<DDType>& taskManager1, const TaskManager<DDType>& taskManager2):
            ApplicationScheme<DDType>(taskManager1, taskManager2) {
            const auto size1 = taskManager1.getCircuit()->size();
            const auto size2 = taskManager2.getCircuit()->size();
            firstIsLarger    = size1 > size2;
            const auto max   = firstIsLarger ? size1 : size2;
            const auto min   = firstIsLarger ? size2 : size1;
            gateRatio        = std::max((max + min / 2U) / min, static_cast<std::size_t>(1U));
        }

        std::pair<size_t, size_t> operator()() override {
            if (firstIsLarger) {
                return {gateRatio, 1U};
            } else {
                return {1U, gateRatio};
            }
        }

    protected:
        std::size_t gateRatio     = 1U;
        bool        firstIsLarger = false;
    };
} // namespace ec

#endif //QCEC_PROPORTIONALAPPLICATIONSCHEME_HPP
