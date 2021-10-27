/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_LOOKAHEADAPPLICATIONSCHEME_HPP
#define QCEC_LOOKAHEADAPPLICATIONSCHEME_HPP

#include "ApplicationScheme.hpp"

namespace ec {
    class LookaheadApplicationScheme: public ApplicationScheme<qc::MatrixDD> {
    public:
        LookaheadApplicationScheme(TaskManager<qc::MatrixDD>& taskManager1, TaskManager<qc::MatrixDD>& taskManager2):
            ApplicationScheme<qc::MatrixDD>(taskManager1, taskManager2) {}

        void setInternalState(qc::MatrixDD& state) {
            internalState = &state;
        }
        void setPackage(dd::Package* dd) {
            package = dd;
        }

        // in general, the lookup application scheme will apply a single operation of either circuit for every invocation.
        // manipulation of the state is handled directly by the application scheme. Thus, the return value is always {0,0}.
        std::pair<size_t, size_t> operator()() override;

    protected:
        qc::MatrixDD op1{};
        bool         cached1 = false;

        qc::MatrixDD op2{};
        bool         cached2 = false;

        // the lookahead application scheme maintains links to an internal state to manipulate and a package to use
        qc::MatrixDD* internalState{};
        dd::Package*  package{};
    };
} // namespace ec

#endif //QCEC_LOOKAHEADAPPLICATIONSCHEME_HPP
