/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#include "checker/dd/DDSimulationChecker.hpp"

namespace ec {
    DDSimulationChecker::DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration) noexcept:
        DDEquivalenceChecker(qc1, qc2, configuration) {
        initialState = dd->makeZeroState(nqubits);
        initializeApplicationScheme(this->configuration.application.simulationScheme);
    }

    void DDSimulationChecker::initializeTask(TaskManager<qc::VectorDD, SimulationDDPackage>& task) {
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
