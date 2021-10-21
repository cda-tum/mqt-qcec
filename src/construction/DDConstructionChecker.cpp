/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "construction/DDConstructionChecker.hpp"

namespace ec {
    EquivalenceCriterion DDConstructionChecker::run() {
        const auto start = std::chrono::steady_clock::now();

        // prepare initial matrix for first circuit
        setupConstructionTask(qc1, task1);

        // prepare initial matrix for second circuit
        setupConstructionTask(qc2, task2);

        while (!task1.finished() && !task2.finished()) {
            applyPotentialSwaps(task1);
            applyPotentialSwaps(task2);

            if (!task1.finished() && !task2.finished()) {
                const auto cost1 = costFunction(*task1.iterator, LEFT);
                const auto cost2 = costFunction(*task2.iterator, RIGHT);

                // TODO: it might make sense to explore whether gate fusion improves performance

                for (std::size_t i = 0; i < cost2 && !task1.finished(); ++i) {
                    advanceConstruction(task1);
                }

                for (std::size_t i = 0; i < cost1 && !task2.finished(); ++i) {
                    advanceConstruction(task2);
                }
            }
        }

        // finish first circuit
        while (!task1.finished()) {
            advanceConstruction(task1);
        }
        postprocess(task1);

        // finish second circuit
        while (!task2.finished()) {
            advanceConstruction(task2);
        }
        postprocess(task2);

        // compare both results for equivalence
        auto equivalence = equals(task1.functionality, task2.functionality);

        maxActiveNodes = dd->mUniqueTable.getMaxActiveNodes();

        const auto end = std::chrono::steady_clock::now();
        runtime        = std::chrono::duration<double>(end - start).count();
        return equivalence;
    }

    void DDConstructionChecker::setupConstructionTask(const qc::QuantumComputation& qc, DDConstructionChecker::ConstructionTask& task) {
        task               = ConstructionTask(qc);
        task.functionality = dd->makeIdent(qc.getNqubits());
        dd->incRef(task.functionality);
        task.functionality = dd->reduceAncillae(task.functionality, qc.ancillary);
    }

    void DDConstructionChecker::advanceConstruction(DDConstructionChecker::ConstructionTask& task) {
        applyGate(*task.iterator, task.functionality, task.permutation);
        ++task.iterator;

        applyPotentialSwaps(task);
    }

    void DDConstructionChecker::applyPotentialSwaps(DDConstructionChecker::ConstructionTask& task) {
        // swiftly apply any SWAP operation as these merely modify the underlying permutation
        while (!task.finished() && (*task.iterator)->getType() == qc::SWAP) {
            applyGate(*task.iterator, task.functionality, task.permutation);
            ++task.iterator;
        }
    }

    void DDConstructionChecker::postprocess(DDConstructionChecker::ConstructionTask& task) {
        // ensure that the permutation that was tracked throughout the circuit matches the expected output permutation
        qc::QuantumComputation::changePermutation(task.functionality, task.permutation, task.qc->outputPermutation, dd);

        // eliminate the superfluous contributions of ancillary qubits
        task.functionality = dd->reduceAncillae(task.functionality, task.qc->ancillary);

        // sum up the contributions of garbage qubits
        task.functionality = dd->reduceGarbage(task.functionality, task.qc->garbage);
    }
} // namespace ec
