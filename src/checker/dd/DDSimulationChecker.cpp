//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/DDSimulationChecker.hpp"

namespace ec {
DDSimulationChecker::DDSimulationChecker(const qc::QuantumComputation& qc1,
                                         const qc::QuantumComputation& qc2,
                                         Configuration config) noexcept
    : DDEquivalenceChecker(qc1, qc2, std::move(config)) {
  initialState = dd->makeZeroState(nqubits);
  initializeApplicationScheme(configuration.application.simulationScheme);
}

void DDSimulationChecker::initializeTask(
    TaskManager<qc::VectorDD, SimulationDDPackage>& taskManager) {
  taskManager.setInternalState(initialState);
  taskManager.incRef();
}

EquivalenceCriterion DDSimulationChecker::checkEquivalence() {
  equivalence = DDEquivalenceChecker::checkEquivalence();

  // adjust reference counts to facilitate reuse of the simulation checker
  taskManager1.decRef();
  taskManager2.decRef();

  return equivalence;
}

void DDSimulationChecker::setRandomInitialState(StateGenerator& generator) {
  const dd::QubitCount nancillary = nqubits - qc1.getNqubitsWithoutAncillae();
  const auto           stateType  = configuration.simulation.stateType;

  initialState =
      generator.generateRandomState(dd, nqubits, nancillary, stateType);
}

} // namespace ec
