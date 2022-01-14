/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "checker/DDSimulationChecker.hpp"

namespace ec {
    DDSimulationChecker::DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration, bool& done):
        DDEquivalenceChecker(qc1, qc2, configuration, done) {
        initialState = dd->makeZeroState(nqubits);
    }

    DDSimulationChecker::DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration, bool& done, const qc::VectorDD& initialState):
        DDEquivalenceChecker(qc1, qc2, configuration, done), initialState(initialState) {}

    void DDSimulationChecker::initializeTask(TaskManager<qc::VectorDD>& task) {
        task.setInternalState(initialState);
        task.incRef();
    }

    EquivalenceCriterion DDSimulationChecker::checkEquivalence() {
        equivalence = DDEquivalenceChecker::checkEquivalence();

        // adjust reference counts to facilitate reuse of the simulation checker
        taskManager1.decRef();
        taskManager2.decRef();

        return equivalence;
    }

    void DDSimulationChecker::setRandomInitialState(StateGenerator& generator) {
        const auto nancillary = nqubits - qc1.getNqubitsWithoutAncillae();
        initialState          = generator.generateRandomState(dd, nqubits, nancillary, configuration.simulation.stateType);
    }

} // namespace ec
