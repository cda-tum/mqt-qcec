/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
    template<class DDPackage = dd::Package<>>
    class LookaheadApplicationScheme final: public ApplicationScheme<qc::MatrixDD, DDPackage> {
    public:
        LookaheadApplicationScheme(TaskManager<qc::MatrixDD, DDPackage>& taskManager1, TaskManager<qc::MatrixDD, DDPackage>& taskManager2) noexcept:
            ApplicationScheme<qc::MatrixDD, DDPackage>(taskManager1, taskManager2) {}

        void setInternalState(qc::MatrixDD& state) noexcept {
            internalState = &state;
        }
        void setPackage(DDPackage* dd) noexcept {
            package = dd;
        }

        // in general, the lookup application scheme will apply a single operation of either circuit for every invocation.
        // manipulation of the state is handled directly by the application scheme. Thus, the return value is always {0,0}.
        std::pair<size_t, size_t> operator()() final {
            assert(internalState != nullptr);
            assert(package != nullptr);

            if (!cached1) {
                // cache the current operation
                op1 = this->taskManager1.getDD();
                package->incRef(op1);
                cached1 = true;
            }

            if (!cached2) {
                // cache the current operation
                op2 = this->taskManager2.getInverseDD();
                package->incRef(op2);
                cached2 = true;
            }

            // compute both possible applications and measure the resulting size
            auto       saved = *internalState;
            const auto dd1   = package->multiply(op1, saved);
            const auto size1 = package->size(dd1);
            const auto dd2   = package->multiply(saved, op2);
            const auto size2 = package->size(dd2);

            // greedily chose the smaller resulting decision diagram
            if (size1 <= size2) {
                assert(!this->taskManager1.finished());
                *internalState = dd1;
                package->decRef(op1);
                cached1 = false;
                this->taskManager1.advanceIterator();
            } else {
                assert(!this->taskManager2.finished());
                *internalState = dd2;
                package->decRef(op2);
                cached2 = false;
                this->taskManager2.advanceIterator();
            }

            // properly track reference counts
            package->incRef(*internalState);
            package->decRef(saved);
            package->garbageCollect();

            // no operations shall be applied by the outer loop in which the application scheme is invoked
            return {0U, 0U};
        }

    protected:
        qc::MatrixDD op1{};
        bool         cached1 = false;

        qc::MatrixDD op2{};
        bool         cached2 = false;

        // the lookahead application scheme maintains links to an internal state to manipulate and a package to use
        qc::MatrixDD* internalState{};
        DDPackage*    package{};
    };
} // namespace ec
