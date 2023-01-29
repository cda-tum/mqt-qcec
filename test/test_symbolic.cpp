//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"
#include "QuantumComputation.hpp"

#include "gtest/gtest.h"

using namespace qc;
using namespace sym;
class SymbolicTest : public ::testing::Test {
public:
  Variable x = Variable("x");

  Symbolic xMonom    = Symbolic{Term<dd::fp>{x}};
  Symbolic xMonomNeg = Symbolic{-Term<dd::fp>{x}};

  QuantumComputation symQc1 = QuantumComputation(1);
  QuantumComputation symQc2 = QuantumComputation(1);
};

TEST_F(SymbolicTest, SymbolicEqu) {
  symQc1.rx(0, xMonom);

  symQc2.h(0);
  symQc2.rz(0, xMonom);
  symQc2.h(0);

  auto ecm = ec::EquivalenceCheckingManager(symQc1, symQc2);
  ecm.run();

  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_F(SymbolicTest, SymbolicNonEqu) {
  symQc1.rx(0, xMonom);

  symQc2.h(0);
  symQc2.rz(0, xMonomNeg);
  symQc2.h(0);

  auto ecm = ec::EquivalenceCheckingManager(symQc1, symQc2);
  ecm.run();

  EXPECT_FALSE(ecm.getResults().consideredEquivalent());
}

TEST_F(SymbolicTest, Timeout) {
  // construct large circuit
  constexpr auto numLayers = 100000;
  symQc1                   = qc::QuantumComputation(2);
  symQc2                   = qc::QuantumComputation(2);
  for (auto i = 0; i < numLayers; ++i) {
    symQc1.x(0, 1_pc);
    symQc1.rx(0, xMonom);

    symQc2.x(0, 1_pc);
    symQc2.rx(0, xMonom);
  }
  ec::Configuration config{};
  config.execution.timeout = 1;
  auto ecm = ec::EquivalenceCheckingManager(symQc1, symQc2, config);

  ecm.run();
  EXPECT_EQ(ecm.getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(SymbolicTest, InvalidCircuit) {
  auto qc = qc::QuantumComputation(4);
  qc.x(0, {1_pc, 2_pc, 3_pc});
  qc.rx(0, xMonom);
  auto ecm = ec::EquivalenceCheckingManager(qc, qc);
  ecm.run();

  EXPECT_EQ(ecm.getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}
