/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "simulation/DDSimulationChecker.hpp"

namespace ec {
    EquivalenceCriterion DDSimulationChecker::run() {
        const auto start = std::chrono::steady_clock::now();

        // prepare initial vector for first circuit
        setupSimulationTask(qc1, task1);

        // prepare initial vector for second circuit
        setupSimulationTask(qc2, task2);

        while (!task1.finished() && !task2.finished()) {
            // in order to enable better operation caching in the decision diagram package the decision diagrams for both
            // states are not built in sequence but rather in parallel. To this end, a cost function is used to specify
            // how many gates to apply from the second circuit given every gate from the first circuit.
            const auto cost = costFunction(qc1, qc2, *task1.iterator);

            advanceSimulation(task1);

            // TODO: it might make sense to explore whether gate fusion improves performance
            for (std::size_t i = 0; i < cost && !task2.finished(); ++i) {
                advanceSimulation(task2);
            }
        }

        // finish first circuit
        while (!task1.finished()) {
            advanceSimulation(task1);
        }
        postprocess(task1);

        // finish second circuit
        while (!task2.finished()) {
            advanceSimulation(task2);
        }
        postprocess(task2);

        // compare both results for equivalence
        auto equivalence = equals(task1.state, task2.state);

        maxActiveNodes = dd->vUniqueTable.getMaxActiveNodes();

        const auto end = std::chrono::steady_clock::now();
        runtime        = std::chrono::duration<double>(end - start).count();
        return equivalence;
    }

    void DDSimulationChecker::setupSimulationTask(const qc::QuantumComputation& qc, DDSimulationChecker::SimulationTask& task) {
        task = SimulationTask(qc, initialState);
        dd->incRef(task.state);
    }

    void DDSimulationChecker::advanceSimulation(DDSimulationChecker::SimulationTask& task) {
        applyGate(*task.iterator, task.state, task.permutation);
        ++task.iterator;
    }

    void DDSimulationChecker::postprocess(DDSimulationChecker::SimulationTask& task) {
        // ensure that the permutation that was tracked throughout the circuit matches the expected output permutation
        qc::QuantumComputation::changePermutation(task.state, task.permutation, task.qc->outputPermutation, dd);

        // sum up the contributions of garbage qubits
        task.state = dd->reduceGarbage(task.state, task.qc->garbage);
    }
} // namespace ec
