//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"
#include "QuantumComputation.hpp"

#include "gtest/gtest.h"

using namespace dd::literals;
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

  ec::Configuration config{};
  auto ecm = ec::EquivalenceCheckingManager(symQc1, symQc2, config);
  ecm.run();

  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_F(SymbolicTest, SymbolicNonEqu) {
  symQc1.rx(0, xMonom);

  symQc2.h(0);
  symQc2.rz(0, xMonomNeg);
  symQc2.h(0);

  ec::Configuration config{};
  auto ecm = ec::EquivalenceCheckingManager(symQc1, symQc2, config);
  ecm.run();

  EXPECT_FALSE(ecm.getResults().consideredEquivalent());
}
