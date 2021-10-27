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
        ProportionalApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2);

        std::pair<size_t, size_t> operator()() override;

    protected:
        std::size_t gateRatio     = 1U;
        bool        firstIsLarger = false;
    };
} // namespace ec

#endif //QCEC_PROPORTIONALAPPLICATIONSCHEME_HPP
