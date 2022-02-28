/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
    class LookaheadApplicationScheme final: public ApplicationScheme<qc::MatrixDD> {
    public:
        LookaheadApplicationScheme(TaskManager<qc::MatrixDD>& taskManager1, TaskManager<qc::MatrixDD>& taskManager2) noexcept:
            ApplicationScheme<qc::MatrixDD>(taskManager1, taskManager2) {}

        void setInternalState(qc::MatrixDD& state) noexcept {
            internalState = &state;
        }
        void setPackage(dd::Package* dd) noexcept {
            package = dd;
        }

        // in general, the lookup application scheme will apply a single operation of either circuit for every invocation.
        // manipulation of the state is handled directly by the application scheme. Thus, the return value is always {0,0}.
        std::pair<size_t, size_t> operator()() final;

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
