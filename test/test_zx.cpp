//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "Definitions.hpp"
#include "EquivalenceCheckingManager.hpp"
#include "QuantumComputation.hpp"
#include "dd/Control.hpp"

#include "gtest/gtest.h"
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using namespace dd::literals;
class ZXTest : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcAlternative;
  ec::Configuration      config{};

  std::unique_ptr<ec::EquivalenceCheckingManager> ecm{};

  std::string testOriginal       = "./circuits/test/test.real";
  std::string testAlternativeDir = "./circuits/test/";

  void SetUp() override {
    config.execution.parallel               = true;
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runSimulationChecker   = false;
    config.execution.runZXChecker           = true;
  }

  void TearDown() override { std::cout << ecm->toString() << std::endl; }
};

INSTANTIATE_TEST_SUITE_P(
    TestCircuits, ZXTest,
    testing::Values("inputperm", "ancilla", "ancilla_inputperm", "swap",
                    "outputperm", "ancilla_inputperm_outputperm",
                    "optimizedswap",
                    "ancilla_inputperm_outputperm_optimizedswap",
                    "ancilla_inputperm_outputperm_optimizedswap2"),
    [](const testing::TestParamInfo<ZXTest::ParamType>& info) {
      std::stringstream ss{};
      ss << info.param;
      return ss.str();
    });

TEST_P(ZXTest, TestCircuits) {
  qcOriginal.import(testOriginal);
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, NonEquivalent) {
  qcOriginal.import(
      std::stringstream("OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg "
                        "q[2];\ncx q[0], q[1];\n"),
      qc::OpenQASM);
  qcAlternative.import(
      std::stringstream("OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\nh "
                        "q[0]; cx q[1], q[0]; h q[0]; h q[1];\n"),
      qc::OpenQASM);
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_FALSE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, Timeout) {
  // construct large circuit
  constexpr auto numLayers = 10000;
  qcOriginal               = qc::QuantumComputation(2);
  qcAlternative            = qc::QuantumComputation(2);
  for (auto i = 0; i < numLayers; ++i) {
    qcOriginal.x(0, 1_pc);
    qcOriginal.h(0);

    qcAlternative.x(0, 1_pc);
    qcAlternative.h(0);
  }

  config.execution.timeout = 1s;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, CloseButNotEqual) {
  qcOriginal = qc::QuantumComputation(1);
  qcOriginal.x(0);

  qcAlternative = qc::QuantumComputation(1);
  qcAlternative.x(0);
  qcAlternative.phase(0, dd::PI / 1024.);

  config.functionality.traceThreshold = 1e-2;

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_EQ(ecm->equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(ZXTest, NotEqual) {
  qcOriginal = qc::QuantumComputation(1);
  qcOriginal.x(0);

  qcAlternative = qc::QuantumComputation(1);
  qcAlternative.x(0);
  qcAlternative.phase(0, dd::PI / 1024.);

  config.functionality.traceThreshold = 1e-9;

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);
  ecm->run();
  EXPECT_EQ(ecm->equivalence(),
            ec::EquivalenceCriterion::ProbablyNotEquivalent);
}

TEST_F(ZXTest, PermutationMismatch) {
  qcOriginal = qc::QuantumComputation(2);
  qcOriginal.x(0, 1_pc);

  qcAlternative = qc::QuantumComputation(2);
  qcAlternative.x(0, 1_pc);

  qcAlternative.outputPermutation[0] = 1;
  qcAlternative.outputPermutation[1] = 0;

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);
  ecm->run();

  EXPECT_FALSE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, Permutation) {
  auto qc1 = qc::QuantumComputation(2);
  auto qc2 = qc::QuantumComputation(2);
  qc1.x(0);
  qc1.x(1);
  qc2.x(0);
  qc2.x(1);
  qc::Permutation p;
  p[0]                  = 1;
  p[1]                  = 0;
  qc1.initialLayout     = p;
  qc1.outputPermutation = p;

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);
  ecm->run();

  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, Ancilla) {
  auto qc1 = qc::QuantumComputation(1);
  auto qc2 = qc::QuantumComputation(2);

  qc1.i(0);
  qc2.x(0, 1_pc);
  qc2.setLogicalQubitAncillary(1);

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);
  ecm->run();

  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, ZXWrongAncilla) {
  auto            qc1 = qc::QuantumComputation(1);
  auto            qc2 = qc::QuantumComputation(2);
  qc::Permutation p1{};
  p1[0] = 0;
  qc1.x(0);
  qc2.x(0, 1_nc);
  qc2.setLogicalQubitAncillary(1);

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, ZXConfiguredForInvalidCircuitParallel) {
  auto qc = qc::QuantumComputation(4);
  qc.x(0, {1_pc, 2_pc, 3_pc});

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qc, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, ZXConfiguredForInvalidCircuitSequential) {
  auto qc = qc::QuantumComputation(4);
  qc.x(0, {1_pc, 2_pc, 3_pc});

  config.execution.parallel = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qc, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

class ZXTestCompFlow : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcTranspiled;
  ec::Configuration      config{};

  std::unique_ptr<ec::EquivalenceCheckingManager> ecm{};

  std::string test_original_dir   = "./circuits/original/";
  std::string test_transpiled_dir = "./circuits/transpiled/";

  void SetUp() override {
    config.execution.parallel               = false;
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runSimulationChecker   = false;
    config.execution.runZXChecker           = true;

    qcOriginal.import(test_original_dir + GetParam() + ".real");
    qcTranspiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");
  }
};

INSTANTIATE_TEST_SUITE_P(
    ZXTestCompFlow, ZXTestCompFlow,
    testing::Values("c2_181", "rd73_312", "sym9_317", "mod5adder_306",
                    "rd84_313"),
    [](const testing::TestParamInfo<ZXTestCompFlow::ParamType>& info) {
      auto s = info.param;
      std::replace(s.begin(), s.end(), '-', '_');
      return s;
    });

TEST_P(ZXTestCompFlow, EquivalenceCompilationFlow) {
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcTranspiled, config);
  ecm->run();
  std::cout << ecm->toString() << std::endl;
  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}
