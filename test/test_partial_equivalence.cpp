//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"

class PartialEqualityTest : public testing::Test {
  void SetUp() override {
    qc1                                       = qc::QuantumComputation(nqubits);
    qc2                                       = qc::QuantumComputation(nqubits);
    config.optimizations.fuseSingleQubitGates = false;
    config.optimizations.reorderOperations    = false;
    config.optimizations.reconstructSWAPs     = false;
    config.optimizations.fixOutputPermutationMismatch = true;

    config.execution.runSimulationChecker   = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runConstructionChecker = false;
    config.execution.runZXChecker           = false;
  }

protected:
  std::size_t            nqubits = 3U;
  qc::QuantumComputation qc1;
  qc::QuantumComputation qc2;
  ec::Configuration      config{};
};

TEST_F(PartialEqualityTest, AlternatingCheckerGarbage) {
  // these circuits have the same gates acting on the measured qubit
  // and random gates acting on the two garbage qubits
  qc1.cswap(1, 0, 2);
  qc1.cx(2, 0);
  qc1.h(0);
  qc1.tdg(1);
  qc1.s(1);
  qc1.z(2);

  qc2.cswap(1, 0, 2);
  qc2.cx(2, 0);
  qc2.h(0);
  qc2.h(2);
  qc2.rz(dd::PI_4, 1);
  qc2.ry(0.1, 1);
  qc2.cx(1, 2);

  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(2);
  qc2.setLogicalQubitGarbage(1);

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEqualityTest, AlternatingCheckerGarbage2) {
  // measured qubit: 1
  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(0);

  qc1.h(1);
  qc1.tdg(0);
  qc1.s(0);
  qc1.z(2);

  // measured qubit: 1
  qc2.setLogicalQubitGarbage(0);
  qc2.setLogicalQubitGarbage(2);

  qc2.h(1);
  qc2.h(0);
  qc2.rz(dd::PI_4, 2);
  qc2.ry(0.1, 2);
  qc2.cx(2, 0);

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEqualityTest, AlternatingCheckerGarbageAndAncillary) {
  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc1.setLogicalQubitAncillary(2);

  qc1.h(0);
  qc1.tdg(1);
  qc1.s(1);
  // ancillary qubits are initialized to zero, therefore this gate doesn't
  // change the functionality of the circuit
  qc1.cx(2, 0);

  qc::QuantumComputation qc3(nqubits - 1);
  qc3.setLogicalQubitGarbage(2);
  qc3.setLogicalQubitGarbage(1);

  qc3.h(0);
  qc3.rz(dd::PI_4, 1);
  qc3.ry(0.1, 1);

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc3, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEqualityTest, AlternatingCheckerGarbageNotEquivalent) {
  // example from the paper https://arxiv.org/abs/2208.07564
  // these two circuits are only partially equivalent,
  // therefore the equivalence checker returns NotEquivalent
  qc1.cswap(1, 0, 2);
  qc1.h(0);
  qc1.z(2);
  qc1.cswap(1, 0, 2);

  qc2.x(1);
  qc2.ch(1, 0);

  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(2);
  qc2.setLogicalQubitGarbage(1);

  std::cout << "qc1:\n";
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::NotEquivalent);
}
