//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"
#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"

#include "gtest/gtest.h"
#include <functional>
#include <sstream>

using namespace dd::literals;

class EqualityTest : public testing::Test {
  void SetUp() override {
    qc1                                       = qc::QuantumComputation(nqubits);
    qc2                                       = qc::QuantumComputation(nqubits);
    config.optimizations.fuseSingleQubitGates = false;
    config.optimizations.reorderOperations    = false;
    config.optimizations.reconstructSWAPs     = false;

    config.execution.runSimulationChecker   = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runConstructionChecker = false;
    config.execution.runZXChecker           = false;
  }

protected:
  dd::QubitCount         nqubits = 1U;
  qc::QuantumComputation qc1;
  qc::QuantumComputation qc2;
  ec::Configuration      config{};
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

TEST_F(EqualityTest, CloseButNotEqualAlternating) {
  qc1.x(0);

  qc2.x(0);
  qc2.phase(0, dd::PI / 1024.);

  config.functionality.traceThreshold    = 1e-2;
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm << std::endl;
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, CloseButNotEqualConstruction) {
  qc1.x(0);

  qc2.x(0);
  qc2.phase(0, dd::PI / 1024.);

  config.functionality.traceThreshold     = 1e-2;
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm << std::endl;
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, CloseButNotEqualAlternatingGlobalPhase) {
  qc1.x(0);

  qc2.x(0);
  qc2.phase(0, dd::PI / 1024.);
  // add a global phase of -1
  qc2.z(0);
  qc2.x(0);
  qc2.z(0);
  qc2.x(0);

  config.functionality.traceThreshold    = 1e-2;
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm << std::endl;
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(EqualityTest, CloseButNotEqualSimulation) {
  qc1.h(0);

  qc2.h(0);
  qc2.phase(0, dd::PI / 1024.);

  config.simulation.fidelityThreshold   = 1e-2;
  config.execution.runSimulationChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm << std::endl;
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::ProbablyEquivalent);
}

TEST_F(EqualityTest, SimulationMoreThan64Qubits) {
  qc1 = qc::QuantumComputation(65U);
  qc1.h(0);
  for (auto i = 0U; i < 64U; ++i) {
    qc1.x(static_cast<dd::Qubit>(i + 1), 0_pc);
  }
  qc2                                   = qc1.clone();
  config.execution.runSimulationChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  std::cout << ecm << std::endl;
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::ProbablyEquivalent);
}

TEST_F(EqualityTest, AutomaticSwitchToConstructionChecker) {
  // add ancillary qubits to both circuits
  qc1.addAncillaryQubit(1, -1);
  qc2.addAncillaryQubit(1, -1);

  // perform the same action on both circuits' primary qubit
  qc1.x(0);
  qc2.x(0);

  // perform a different action on the ancillary qubit
  qc1.x(1);
  qc2.z(1);

  // setup default configuration
  config = ec::Configuration{};
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);

  // this should notice that the alternating checker is not capable of running
  // the circuit and should switch to the construction checker
  const auto runConfig = ecm.getConfiguration();
  EXPECT_TRUE(runConfig.execution.runConstructionChecker);
  EXPECT_FALSE(runConfig.execution.runAlternatingChecker);

  // run the equivalence checker
  ecm.run();

  // both circuits should be equivalent since their action only differs on an
  // ancillary and garbage qubit
  const auto result = ecm.equivalence();
  EXPECT_EQ(result, ec::EquivalenceCriterion::Equivalent);

  // also check an exception is thrown
  // if the checker is configured after initialization
  ecm.reset();
  ecm.setAlternatingChecker(true);
  EXPECT_THROW(ecm.run(), std::invalid_argument);
}
