/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "checker/DDSimulationChecker.hpp"

namespace ec {
    DDSimulationChecker::DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration):
        EquivalenceChecker(qc1, qc2, configuration) {
        initialState = dd->makeZeroState(nqubits);
    }

    DDSimulationChecker::DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration, const qc::VectorDD& initialState):
        EquivalenceChecker(qc1, qc2, configuration), initialState(initialState) {}

    void DDSimulationChecker::initializeTask(TaskManager<qc::VectorDD>& task) {
        dd->incRef(initialState);
        task.setInternalState(initialState);
    }
} // namespace ec
