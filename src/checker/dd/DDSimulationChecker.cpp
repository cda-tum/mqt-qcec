//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/DDSimulationChecker.hpp"

namespace ec {
DDSimulationChecker::DDSimulationChecker(const qc::QuantumComputation& circ1,
                                         const qc::QuantumComputation& circ2,
                                         Configuration                 config)
    : DDEquivalenceChecker(circ1, circ2, std::move(config)) {
  initialState = dd->makeZeroState(nqubits);
  initializeApplicationScheme(configuration.application.simulationScheme);
}

void DDSimulationChecker::initializeTask(
    TaskManager<qc::VectorDD, SimulationDDPackageConfig>& taskManager) {
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
  const auto nancillary = nqubits - qc1.getNqubitsWithoutAncillae();
  const auto stateType  = configuration.simulation.stateType;

  initialState =
      generator.generateRandomState(dd, nqubits, nancillary, stateType);
}

} // namespace ec
