//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"
#include "EquivalenceCriterion.hpp"
#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "dd/DDDefinitions.hpp"
#include "ir/operations/Control.hpp"

#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <stdexcept>

class EqualityTest : public testing::Test {
  void SetUp() override {
    qc1 = qc::QuantumComputation(nqubits);
    qc2 = qc::QuantumComputation(nqubits);

    config.execution.runSimulationChecker = false;
    config.execution.runAlternatingChecker = false;
    config.execution.runConstructionChecker = false;
    config.execution.runZXChecker = false;
  }

protected:
  std::size_t nqubits = 1U;
  qc::QuantumComputation qc1;
  qc::QuantumComputation qc2;
  ec::Configuration config{};
};

TEST_F(EqualityTest, GlobalPhase) {
  qc1.x(0);
  qc2.x(0);

  // add a global phase of -1
  qc2.z(0);
  qc2.x(0);
  qc2.z(0);
  qc2.x(0);

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

/**
 * @brief The following is a regression test for
 * https://github.com/cda-tum/mqt-qcec/issues/347
 */
TEST_F(EqualityTest, GlobalPhaseSimulation) {
  qc1.x(0);
  qc2.x(0);

  // add a global phase of -1
  qc2.z(0);
  qc2.x(0);
  qc2.z(0);
  qc2.x(0);

  config.execution.runAlternatingChecker = false;
  config.execution.runConstructionChecker = false;
  config.execution.runZXChecker = false;
  config.execution.runSimulationChecker = true;
  config.execution.parallel = false;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  const auto json = ecm.getResults().json();
  const auto simChecker = json["checkers"].front();
  EXPECT_EQ(simChecker["equivalence"], "equivalent_up_to_phase");
}

TEST_F(EqualityTest, CloseButNotEqualAlternating) {
  qc1.x(0);

  qc2.x(0);
  qc2.p(dd::PI / 1024., 0);

  config.functionality.traceThreshold = 1e-2;
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, CloseButNotEqualConstruction) {
  qc1.x(0);

  qc2.x(0);
  qc2.p(dd::PI / 1024., 0);

  config.functionality.traceThreshold = 1e-2;
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, CloseButNotEqualAlternatingGlobalPhase) {
  qc1.x(0);

  qc2.x(0);
  qc2.p(dd::PI / 1024., 0);
  // add a global phase of -1
  qc2.z(0);
  qc2.x(0);
  qc2.z(0);
  qc2.x(0);

  config.functionality.traceThreshold = 1e-2;
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(EqualityTest, CloseButNotEqualSimulation) {
  qc1.h(0);

  qc2.h(0);
  qc2.p(dd::PI / 1024., 0);

  config.simulation.fidelityThreshold = 1e-2;
  config.execution.runSimulationChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::ProbablyEquivalent);
}

TEST_F(EqualityTest, SimulationMoreThan64Qubits) {
  using namespace qc::literals;

  qc1 = qc::QuantumComputation(65U);
  qc1.h(0);
  for (auto i = 0U; i < 64U; ++i) {
    qc1.cx(0_pc, i + 1);
  }
  qc2 = qc1;
  config.execution.runSimulationChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::ProbablyEquivalent);
}

TEST_F(EqualityTest, AutomaticSwitchToConstructionChecker) {
  // add ancillary qubits to both circuits
  qc1.addAncillaryQubit(1, std::nullopt);
  qc2.addAncillaryQubit(1, std::nullopt);

  // perform the same action on both circuits' primary qubit
  qc1.x(0);
  qc2.x(0);

  // perform a different action on the ancillary qubit
  qc1.x(1);
  qc2.z(1);

  // setup default configuration
  config = ec::Configuration{};
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setCheckPartialEquivalence(true);

  // this should notice that the alternating checker is not capable of running
  // the circuit and should switch to the construction checker
  const auto runConfig = ecm.getConfiguration();
  EXPECT_TRUE(runConfig.execution.runConstructionChecker);
  EXPECT_FALSE(runConfig.execution.runAlternatingChecker);

  // run the equivalence checker
  ecm.run();

  // both circuits should be partially equivalent since their action only
  // differs on an ancillary and garbage qubit
  const auto result = ecm.equivalence();
  EXPECT_EQ(result, ec::EquivalenceCriterion::Equivalent);

  // Check an exception is raised for a checker configured after initialization.
  // Note: this exception can only be caught in sequential mode since it is
  // raised in a different thread otherwise.
  ecm.reset();
  ecm.setAlternatingChecker(true);
  ecm.setParallel(false);
  EXPECT_THROW(ecm.run(), std::invalid_argument);
}

TEST_F(EqualityTest, ExceptionInParallelThread) {
  qc1.x(0);

  config = ec::Configuration{};
  config.execution.runAlternatingChecker = false;
  config.execution.runConstructionChecker = false;
  config.execution.runSimulationChecker = true;
  config.execution.runZXChecker = false;
  config.application.simulationScheme = ec::ApplicationSchemeType::Lookahead;

  ec::EquivalenceCheckingManager ecm(qc1, qc1, config);
  EXPECT_THROW(ecm.run(), std::invalid_argument);
}

TEST_F(EqualityTest, BothCircuitsEmptyAlternatingChecker) {
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, BothCircuitsEmptyConstructionChecker) {
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, BothCircuitsEmptySimulationChecker) {
  config.execution.runSimulationChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, BothCircuitsEmptyZXChecker) {
  config.execution.runZXChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, OneCircuitEmptyAlternatingChecker) {
  qc2.h(0);
  qc2.x(0);
  qc2.h(0);
  qc2.z(0);
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, OneCircuitEmptyConstructionChecker) {
  qc2.h(0);
  qc2.x(0);
  qc2.h(0);
  qc2.z(0);
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, OneCircuitEmptySimulationChecker) {
  qc2.h(0);
  qc2.x(0);
  qc2.h(0);
  qc2.z(0);
  config.execution.runSimulationChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::ProbablyEquivalent);
}

TEST_F(EqualityTest, OneCircuitEmptyZXChecker) {
  qc2.h(0);
  qc2.x(0);
  qc2.h(0);
  qc2.z(0);
  config.execution.runZXChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, onlySingleTask) {
  qc1.h(0);
  qc2.h(0);
  config.execution.runSimulationChecker = true;
  config.simulation.maxSims = 1U;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_TRUE(ecm.getConfiguration().onlySingleTask());

  ecm.reset();
  ecm.disableAllCheckers();
  ecm.setConstructionChecker(true);
  ecm.run();
  EXPECT_TRUE(ecm.getConfiguration().onlySingleTask());

  ecm.reset();
  ecm.disableAllCheckers();
  ecm.setZXChecker(true);
  ecm.run();
  EXPECT_TRUE(ecm.getConfiguration().onlySingleTask());

  ecm.reset();
  ecm.disableAllCheckers();
  ecm.setAlternatingChecker(true);
  ecm.run();
  EXPECT_TRUE(ecm.getConfiguration().onlySingleTask());
}

TEST_F(EqualityTest, StripIdleQubitPresentInBothCircuits) {
  //  Remove an idle qubit that is present in both circuits
  qc1 = qc::QuantumComputation(2, 1);
  qc1.h(0);
  qc1.measure(0, 0);
  qc1.setLogicalQubitGarbage(1);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(2, 1);
  qc2.x(0);
  qc2.measure(0, 0);
  qc2.setLogicalQubitGarbage(1);
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::NotEquivalent);
  EXPECT_EQ(ecm.getResults().numQubits1, ecm.getResults().numQubits2);
  EXPECT_EQ(ecm.getResults().numQubits2, qc2.getNqubits() - 1);
  EXPECT_EQ(ecm.getResults().numMeasuredQubits1, 1);
  EXPECT_EQ(ecm.getResults().numMeasuredQubits2, 1);
}

TEST_F(EqualityTest, NotEqualDueToNoSeparateIdleQubitStripping) {
  // Test that circuits are not equivalent because different idle qubits are not
  // removed individually
  qc1 = qc::QuantumComputation(3, 3);
  qc1.h(0);
  qc1.x(2);
  qc1.measure(0, 0);
  qc1.measure(2, 2);
  qc1.setLogicalQubitGarbage(1);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(3, 3);
  qc2.h(0);
  qc2.x(1);
  qc2.measure(0, 0);
  qc2.measure(1, 1);
  qc2.setLogicalQubitGarbage(2);
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::NotEquivalent);
  // Check that idle qubits have not been removed and re-added as ancillary
  // qubits
  EXPECT_EQ(ecm.getResults().numQubits1, qc1.getNqubits());
  EXPECT_EQ(ecm.getResults().numAncillae1, 0);
  EXPECT_EQ(ecm.getResults().numQubits2, qc2.getNqubits());
  EXPECT_EQ(ecm.getResults().numAncillae2, 0);
  EXPECT_EQ(ecm.getResults().numMeasuredQubits1, 2);
  EXPECT_EQ(ecm.getResults().numMeasuredQubits2, 2);
}

TEST_F(EqualityTest, EqualDueToNoSeparateIdleQubitStripping) {
  // Test that circuits are equivalent because different idle qubits are not
  // removed individually
  qc1 = qc::QuantumComputation(3, 1);
  qc1.h(0);
  qc1.swap(1, 2);
  qc1.swap(1, 2);
  qc1.measure(0, 0);
  qc1.setLogicalQubitGarbage(1);
  qc1.setLogicalQubitGarbage(2);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(3, 1);
  qc2.h(0);
  qc2.measure(0, 0);
  qc2.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(2);
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  config.optimizations.elidePermutations = false;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  EXPECT_EQ(ecm.getResults().numQubits1, ecm.getResults().numQubits2);
  EXPECT_EQ(ecm.getResults().numQubits2, qc2.getNqubits());
  EXPECT_EQ(ecm.getResults().numAncillae1, 0);
  EXPECT_EQ(ecm.getResults().numAncillae2, 0);
}

TEST_F(EqualityTest,
       StripIdleQubitPresentInBothCircuitsWithDifferentInitialLayout) {
  // Test that idle qubits are removed correctly even if the initial layout is
  // different
  qc1 = qc::QuantumComputation(3, 2);
  qc1.initialLayout[0] = 0;
  qc1.initialLayout[1] = 2;
  qc1.initialLayout[2] = 1;
  qc1.h(0);
  qc1.x(2);
  qc1.measure(0, 0);
  qc1.measure(2, 1);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(3, 2);
  qc2.h(0);
  qc2.x(1);
  qc2.measure(0, 0);
  qc2.measure(1, 1);
  qc2.setLogicalQubitGarbage(2);
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  // Circuits should be equivalent even if elidePermutations is false
  config.optimizations.elidePermutations = false;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  EXPECT_EQ(ecm.getResults().numQubits1, ecm.getResults().numQubits2);
  EXPECT_EQ(ecm.getResults().numQubits2, qc2.getNqubits() - 1);
}

TEST_F(EqualityTest, StripIdleQubitPresentOnlyInOneCircuit) {
  //  Remove an idle logical qubit that is present only in one circuit
  qc1 = qc::QuantumComputation(2, 2);
  qc1.h(0);
  qc1.x(1);
  qc1.measure(0, 0);
  qc1.measure(1, 1);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(3, 2);
  qc2.h(0);
  qc2.x(1);
  qc2.measure(0, 0);
  qc2.measure(1, 1);
  qc2.setLogicalQubitGarbage(2);
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  EXPECT_EQ(ecm.getResults().numQubits2, qc2.getNqubits() - 1);
}

TEST_F(EqualityTest, StripIdleQubitLogicalOnlyInOnePhysicalInBothCircuits) {
  //  Remove an idle logical qubit that is present only in one circuit, but
  //  which is mapped to a physical qubit that is present in both circuits
  qc1 = qc::QuantumComputation(2, 2);
  qc1.h(0);
  qc1.x(1);
  qc1.measure(0, 0);
  qc1.measure(1, 1);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(3, 2);
  qc2.initialLayout[0] = 0;
  qc2.initialLayout[1] = 2;
  qc2.initialLayout[2] = 1;
  qc2.h(0);
  qc2.x(2);
  qc2.measure(0, 0);
  qc2.measure(2, 1);
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  // Circuits should be equivalent even if elidePermutations is false
  config.optimizations.elidePermutations = false;

  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  EXPECT_EQ(ecm.getResults().numQubits2, qc2.getNqubits() - 1);
}

TEST_F(EqualityTest, StripIdleQubitOutputPermutationDifferent) {
  // Test that qubits are not removed if they occur in the output permutation
  // and input and output permutation are not equivalent
  qc1 = qc::QuantumComputation(2);
  // Simulating Qiskit Import, where no measurements are included but
  // outputPermutation is set
  qc1.swap(0, 1);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(2);
  qc2.outputPermutation[0] = 1;
  qc2.outputPermutation[1] = 0;
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  config.optimizations.elidePermutations = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  // Check that no qubits were removed as the initial and output permutation are
  // not equivalent for the idle qubits
  EXPECT_EQ(ecm.getResults().numQubits1, ecm.getResults().numQubits2);
  EXPECT_EQ(ecm.getResults().numQubits2, qc2.getNqubits());
  EXPECT_EQ(ecm.getResults().numAncillae1, 0);
  EXPECT_EQ(ecm.getResults().numAncillae2, 0);
}

TEST_F(EqualityTest, StripIdleQubitOutputPermutationEquivalent) {
  // Test that idle qubits are removed if they occur in the output permutation
  // and input and output permutation are equivalent
  qc1 = qc::QuantumComputation(2);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(2);
  qc2.initialLayout[0] = 1;
  qc2.initialLayout[1] = 0;
  qc2.outputPermutation[0] = 1;
  qc2.outputPermutation[1] = 0;
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  // Check that qubits were removed as the initial and output permutation are
  // equivalent
  EXPECT_EQ(ecm.getResults().numQubits1, ecm.getResults().numQubits2);
  EXPECT_EQ(ecm.getResults().numQubits2, 0);
}

TEST_F(EqualityTest, StripQubitIdleInOneCircuitOnlyOutputPermutationDifferent) {
  // Test that qubits, idle in one circuit only, are not removed if they occur
  // in the output permutation and input and output permutation are not
  // equivalent
  qc1 = qc::QuantumComputation(1);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(2);
  qc2.outputPermutation[0] = 1;
  qc2.outputPermutation[1] = 0;
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  // Check that no qubits were removed as the initial and output permutation are
  // not equivalent for the idle qubits
  EXPECT_EQ(ecm.getResults().numQubits1, 2);
  EXPECT_EQ(ecm.getResults().numQubits2, 2);
  EXPECT_EQ(ecm.getResults().numAncillae1, 1);
  EXPECT_EQ(ecm.getResults().numAncillae2, 1);
}

TEST_F(EqualityTest,
       StripQubitIdleInOneCircuitOnlyOutputPermutationEquivalent) {
  // Test that qubits, idle in one circuit only, are removed if they occur in
  // the output permutation and input and output permutation are equivalent
  qc1 = qc::QuantumComputation(0);
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(1);
  qc1.initializeIOMapping();
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
  // Check that qubits were removed as the initial and output permutation are
  // equivalent
  EXPECT_EQ(ecm.getResults().numQubits1, ecm.getResults().numQubits2);
  EXPECT_EQ(ecm.getResults().numQubits2, 0);
}

TEST_F(EqualityTest, StripIdleQubitInOutputPermutationWithAncilla) {
  // Test that idle qubits, which are present in the output permutation of one
  // circuit and match the initial layout, are eliminated even if they do not
  // appear in the output permutation of the other circuit
  qc1 = qc::QuantumComputation(0);
  qc1.addAncillaryRegister(2);
  // Here, qubits that are ancillary and idle are removed from output
  // permutation
  qc1.initializeIOMapping();

  qc2 = qc::QuantumComputation(2);
  qc2.initializeIOMapping();

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.getResults().numQubits1, ecm.getResults().numQubits2);
  EXPECT_EQ(ecm.getResults().numQubits2, 0);
  EXPECT_EQ(ecm.getResults().numAncillae1, 0);
  EXPECT_EQ(ecm.getResults().numAncillae2, 0);
}
