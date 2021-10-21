/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "alternating/DDAlternatingChecker.hpp"

namespace ec {

    EquivalenceCriterion DDAlternatingChecker::run() {
        const auto start = std::chrono::steady_clock::now();

        // prepare first task
        task1 = AlternatingTask(qc1, LEFT);

        // prepare second task
        task2 = AlternatingTask(qc2, RIGHT);

        // initialize functionality
        functionality = createInitialMatrix();

        // run specific scheme
        if (scheme == AlternatingScheme::CostFunction) {
            runCostFunctionCheck();
        } else if (scheme == AlternatingScheme::Lookahead) {
            runLookaheadCheck();
        }

        // finish first circuit
        while (!task1.finished()) {
            advanceAlternatingScheme(task1);
        }

        // finish second circuit
        while (!task2.finished()) {
            advanceAlternatingScheme(task2);
        }

        // postprocess the result
        postprocess();

        // generate goal matrix
        auto goalMatrix = createGoalMatrix();

        // compare the result to the goal matrix
        auto equivalence = equals(functionality, goalMatrix);

        maxActiveNodes = dd->mUniqueTable.getMaxActiveNodes();

        const auto end = std::chrono::steady_clock::now();
        runtime        = std::chrono::duration<double>(end - start).count();
        return equivalence;
    }

    qc::MatrixDD DDAlternatingChecker::createInitialMatrix() {
        const auto nqubits = qc1.getNqubits();

        // create the full identity matrix
        auto e = dd->makeIdent(nqubits);
        dd->incRef(e);

        // check whether any qubit acts as ancillary and is actually acted upon in both of the circuits
        // at the moment this is just to be on the safe side. It might be fine to also start with the
        // reduced matrix for every ancillary without any restriction
        std::vector<bool> ancillary(nqubits);
        for (auto q = static_cast<dd::Qubit>(nqubits - 1); q >= 0; --q) {
            if (qc1.logicalQubitIsAncillary(q) && qc2.logicalQubitIsAncillary(q)) {
                bool found1  = false;
                bool isidle1 = false;
                for (const auto& in1: qc1.initialLayout) {
                    if (in1.second == q) {
                        found1  = true;
                        isidle1 = qc1.isIdleQubit(in1.first);
                        break;
                    }
                }
                bool found2  = false;
                bool isidle2 = false;
                for (const auto& in2: qc2.initialLayout) {
                    if (in2.second == q) {
                        found2  = true;
                        isidle2 = qc2.isIdleQubit(in2.first);
                        break;
                    }
                }

                // qubit only really exists or is acted on in one of the circuits
                if ((found1 ^ found2) || (isidle1 ^ isidle2)) {
                    ancillary[q] = true;
                }
            }
        }

        // reduce the ancillary qubit contributions
        e = dd->reduceAncillae(e, ancillary);
        return e;
    }

    qc::MatrixDD DDAlternatingChecker::createGoalMatrix() {
        const auto nqubits = qc1.getNqubits();

        // create the full identity matrix
        auto goalMatrix = dd->makeIdent(nqubits);
        dd->incRef(goalMatrix);

        goalMatrix = dd->reduceGarbage(goalMatrix, qc1.garbage, LEFT);
        goalMatrix = dd->reduceGarbage(goalMatrix, qc2.garbage, RIGHT);
        goalMatrix = dd->reduceAncillae(goalMatrix, qc1.ancillary, LEFT);
        goalMatrix = dd->reduceAncillae(goalMatrix, qc2.ancillary, RIGHT);

        return goalMatrix;
    }

    void DDAlternatingChecker::advanceAlternatingScheme(DDAlternatingChecker::AlternatingTask& task) {
        applyGate(*task.iterator, functionality, task.permutation, task.direction);
        ++task.iterator;

        applyPotentialSwaps(task);
    }

    void DDAlternatingChecker::applyPotentialSwaps(DDAlternatingChecker::AlternatingTask& task) {
        // swiftly apply any SWAP operation as these merely modify the underlying permutation
        while (!task.finished() && (*task.iterator)->getType() == qc::SWAP) {
            applyGate(*task.iterator, functionality, task.permutation, task.direction);
            ++task.iterator;
        }
    }

    void DDAlternatingChecker::runCostFunctionCheck() {
        while (!task1.finished() && !task2.finished()) {
            applyPotentialSwaps(task1);
            applyPotentialSwaps(task2);

            if (!task1.finished() && !task2.finished()) {
                const auto cost1 = costFunction(*task1.iterator, LEFT);
                const auto cost2 = costFunction(*task2.iterator, RIGHT);

                // TODO: it might make sense to explore whether gate fusion improves performance

                for (std::size_t i = 0; i < cost2 && !task1.finished(); ++i) {
                    advanceAlternatingScheme(task1);
                }

                for (std::size_t i = 0; i < cost1 && !task2.finished(); ++i) {
                    advanceAlternatingScheme(task2);
                }
            }
        }
    }

    void DDAlternatingChecker::runLookaheadCheck() {
        qc::MatrixDD left{}, right{}, saved{};
        bool         cachedLeft = false, cachedRight = false;

        while (!task1.finished() && !task2.finished()) {
            if (!cachedLeft) {
                left = (*task1.iterator)->getDD(dd, task1.permutation);
                dd->incRef(left);
                ++task1.iterator;
                cachedLeft = true;
            }

            if (!cachedRight) {
                right = (*task2.iterator)->getInverseDD(dd, task2.permutation);
                dd->incRef(right);
                ++task2.iterator;
                cachedRight = true;
            }

            saved          = functionality;
            auto lookLeft  = dd->multiply(left, saved);
            auto lookRight = dd->multiply(saved, right);

            auto nc1 = dd->size(lookLeft);
            auto nc2 = dd->size(lookRight);

            if (nc1 <= nc2) {
                functionality = lookLeft;
                dd->decRef(left);
                cachedLeft = false;
            } else {
                functionality = lookRight;
                dd->decRef(right);
                cachedRight = false;
            }
            dd->incRef(functionality);
            dd->decRef(saved);
            dd->garbageCollect();
        }

        if (cachedLeft) {
            saved         = functionality;
            functionality = dd->multiply(left, saved);
            dd->incRef(functionality);
            dd->decRef(saved);
            dd->decRef(left);
            dd->garbageCollect();
        }

        if (cachedRight) {
            saved         = functionality;
            functionality = dd->multiply(saved, right);
            dd->incRef(functionality);
            dd->decRef(saved);
            dd->decRef(right);
            dd->garbageCollect();
        }
    }

    void DDAlternatingChecker::postprocess() {
        qc::QuantumComputation::changePermutation(functionality, task1.permutation, qc1.outputPermutation, dd, LEFT);
        qc::QuantumComputation::changePermutation(functionality, task2.permutation, qc2.outputPermutation, dd, RIGHT);
        functionality = dd->reduceGarbage(functionality, qc1.garbage, LEFT);
        functionality = dd->reduceGarbage(functionality, qc2.garbage, RIGHT);
        functionality = dd->reduceAncillae(functionality, qc1.ancillary, LEFT);
        functionality = dd->reduceAncillae(functionality, qc2.ancillary, RIGHT);
    }

} // namespace ec
