//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "Configuration.hpp"
#include "Definitions.hpp"
#include "EquivalenceCheckingManager.hpp"
#include "EquivalenceCriterion.hpp"
#include "Permutation.hpp"
#include "QuantumComputation.hpp"
#include "operations/Control.hpp"
#include "zx/ZXDefinitions.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class ZXTest : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcAlternative;
  ec::Configuration config{};

  std::unique_ptr<ec::EquivalenceCheckingManager> ecm;

  std::string testOriginal = "./circuits/test/test.real";
  std::string testAlternativeDir = "./circuits/test/";

  void SetUp() override {
    config.execution.parallel = true;
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker = false;
    config.execution.runSimulationChecker = false;
    config.execution.runZXChecker = true;
  }

  void TearDown() override { std::cout << ecm->getResults() << "\n"; }
};

INSTANTIATE_TEST_SUITE_P(
    TestCircuits, ZXTest,
    testing::Values("inputperm", "ancilla", "ancilla_inputperm", "swap",
                    "outputperm", "ancilla_inputperm_outputperm",
                    "optimizedswap",
                    "ancilla_inputperm_outputperm_optimizedswap",
                    "ancilla_inputperm_outputperm_optimizedswap2"),
    [](const testing::TestParamInfo<ZXTest::ParamType>& inf) {
      std::stringstream ss{};
      ss << inf.param;
      return ss.str();
    });

TEST_P(ZXTest, TestCircuits) {
  qcOriginal.import(testOriginal);
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_TRUE(ecm->getResults().consideredEquivalent() ||
              ecm->getResults().equivalence ==
                  ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, NonEquivalent) {
  auto ss = std::stringstream("OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg "
                              "q[2];\ncx q[0], q[1];\n");
  qcOriginal.import(ss, qc::Format::OpenQASM2);
  auto ss2 =
      std::stringstream("OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\nh "
                        "q[0]; cx q[1], q[0]; h q[0]; h q[1];\n");
  qcAlternative.import(ss2, qc::Format::OpenQASM2);
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_FALSE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, Timeout) {
  using namespace qc::literals;

  // construct large circuit
  constexpr auto numLayers = 10000;
  qcOriginal = qc::QuantumComputation(2);
  qcAlternative = qc::QuantumComputation(2);
  for (auto i = 0; i < numLayers; ++i) {
    qcOriginal.cx(1, 0);
    qcOriginal.h(0);

    qcAlternative.cx(1, 0);
    qcAlternative.h(0);
  }

  config.execution.timeout = 0.1;
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
  qcAlternative.p(qc::PI / 1024., 0);

  config.functionality.traceThreshold = 1e-2;

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_EQ(ecm->equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(ZXTest, NotEqual) {
  qcOriginal = qc::QuantumComputation(1);
  qcOriginal.x(0);

  qcAlternative = qc::QuantumComputation(1);
  qcAlternative.x(0);
  qcAlternative.p(qc::PI / 1024., 0);

  config.functionality.traceThreshold = 1e-9;

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);
  ecm->run();
  EXPECT_EQ(ecm->equivalence(),
            ec::EquivalenceCriterion::ProbablyNotEquivalent);
}

TEST_F(ZXTest, PermutationMismatch) {
  using namespace qc::literals;

  qcOriginal = qc::QuantumComputation(2);
  qcOriginal.cx(1, 0);

  qcAlternative = qc::QuantumComputation(2);
  qcAlternative.cx(1, 0);

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
  p[0] = 1;
  p[1] = 0;
  qc1.initialLayout = p;
  qc1.outputPermutation = p;

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);
  ecm->run();

  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, Ancilla) {
  using namespace qc::literals;

  auto qc1 = qc::QuantumComputation(1);
  auto qc2 = qc::QuantumComputation(2);

  qc1.i(0);
  qc2.cx(1, 0);
  qc2.setLogicalQubitAncillary(1);

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);
  ecm->run();

  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, ZXWrongAncilla) {
  using namespace qc::literals;

  auto qc1 = qc::QuantumComputation(1);
  auto qc2 = qc::QuantumComputation(2);
  qc::Permutation p1{};
  p1[0] = 0;
  qc1.x(0);
  qc2.cx(1_nc, 0);
  qc2.setLogicalQubitAncillary(1);

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, ZXConfiguredForInvalidCircuitParallel) {
  using namespace qc::literals;

  auto qc = qc::QuantumComputation(4);
  qc.mcx({1, 2, 3}, 0);

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qc, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, ZXConfiguredForInvalidCircuitSequential) {
  using namespace qc::literals;

  auto qc = qc::QuantumComputation(4);
  qc.mcx({1, 2, 3}, 0);

  config.execution.parallel = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qc, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, GlobalPhase) {
  auto qc = qc::QuantumComputation(1);
  qc.rz(zx::PI / 8, 0);

  auto qcPrime = qc::QuantumComputation(1);
  qcPrime.p(zx::PI / 8, 0);

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qcPrime, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

class ZXTestCompFlow : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcTranspiled;
  ec::Configuration config{};

  std::unique_ptr<ec::EquivalenceCheckingManager> ecm;

  std::string testOriginalDir = "./circuits/original/";
  std::string testTranspiledDir = "./circuits/transpiled/";

  void SetUp() override {
    config.execution.parallel = false;
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker = false;
    config.execution.runSimulationChecker = false;
    config.execution.runZXChecker = true;

    qcOriginal.import(testOriginalDir + GetParam() + ".real");
    qcTranspiled.import(testTranspiledDir + GetParam() + "_transpiled.qasm");
  }
};

INSTANTIATE_TEST_SUITE_P(
    ZXTestCompFlow, ZXTestCompFlow,
    testing::Values("c2_181", "rd73_312", "sym9_317", "mod5adder_306",
                    "rd84_313"),
    [](const testing::TestParamInfo<ZXTestCompFlow::ParamType>& inf) {
      auto s = inf.param;
      std::replace(s.begin(), s.end(), '-', '_');
      return s;
    });

TEST_P(ZXTestCompFlow, EquivalenceCompilationFlow) {
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcTranspiled, config);
  ecm->run();
  std::cout << ecm->getResults() << "\n";
  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST(ZXTestsMisc, IdentityNotHadamard) {
  const auto qc1 = qc::QuantumComputation(1);
  auto qc2 = qc::QuantumComputation(1);
  qc2.h(0);

  auto ecm = ec::EquivalenceCheckingManager(qc1, qc2);
  ecm.disableAllCheckers();
  ecm.setZXChecker(true);
  ecm.run();

  EXPECT_EQ(ecm.getResults().equivalence,
            ec::EquivalenceCriterion::ProbablyNotEquivalent);
}

TEST_F(ZXTest, NonEquivalentAncillaryCircuit) {
  // ensure that a non-equivalent circuit with ancillas yields no information
  qcOriginal = qc::QuantumComputation(1);
  qcOriginal.x(0);
  qcAlternative = qc::QuantumComputation(2);
  qcAlternative.x(0);
  qcAlternative.swap(0, 1);
  config.execution.parallel = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);

  ecm->setParallel(true);
  ecm->reset();
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);

  // ensure that enabling another checker allows to detect non-equivalence and
  // does not abort the computation.
  ecm->setAlternatingChecker(true);
  ecm->setParallel(false);
  ecm->reset();
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NotEquivalent);

  ecm->setParallel(true);
  ecm->reset();
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NotEquivalent);
}

TEST_F(ZXTest, NonEquivalentCircuit) {
  // ensure that a non-equivalent circuit with ancillas yields no information
  qcOriginal = qc::QuantumComputation(2);
  qcOriginal.x(0);
  qcOriginal.z(1);
  qcAlternative = qc::QuantumComputation(2);
  qcAlternative.x(0);
  qcAlternative.swap(0, 1);
  qcAlternative.z(0);
  config.execution.parallel = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::ProbablyNotEquivalent);

  ecm->setParallel(true);
  ecm->reset();
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::ProbablyNotEquivalent);

  // ensure that enabling another checker allows to detect non-equivalence and
  // does not abort the computation.
  ecm->setAlternatingChecker(true);
  ecm->setParallel(false);
  ecm->reset();
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NotEquivalent);

  ecm->setParallel(true);
  ecm->reset();
  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NotEquivalent);
}

TEST_F(ZXTest, IdleQubit) {
  using namespace qc::literals;
  auto qc1 = qc::QuantumComputation(3U, 3U);
  qc1.h(0);
  qc1.cx(0, 1);
  qc1.cx(1, 2);
  qc1.measure(0, 0);
  qc1.measure(1, 1);
  qc1.measure(2, 2);
  qc1.initializeIOMapping();

  auto qc2 = qc::QuantumComputation(5U, 5U);
  qc2.h(1);
  qc2.cx(1, 0);
  qc2.swap(0, 2);
  qc2.cx(2, 4);
  qc2.measure(1, 0);
  qc2.measure(2, 1);
  qc2.measure(4, 2);
  qc2.initializeIOMapping();

  config.execution.runZXChecker = true;
  config.execution.parallel = false;
  config.execution.runSimulationChecker = false;
  config.execution.runAlternatingChecker = false;
  config.execution.runConstructionChecker = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);

  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, TwoQubitRotations) {
  using namespace qc::literals;
  auto qc1 = qc::QuantumComputation(2U);
  qc1.rzz(0.1, 0, 1);
  qc1.rxx(0.3, 0, 1);
  qc1.ryy(0.2, 0, 1);
  auto qc2 = qc::QuantumComputation(2U);
  qc2.cx(0, 1);
  qc2.rz(0.1, 1);
  qc2.cx(0, 1);
  qc2.h(0);
  qc2.h(1);
  qc2.cx(0, 1);
  qc2.rz(0.3, 1);
  qc2.cx(0, 1);
  qc2.h(0);
  qc2.h(1);
  qc2.rx(qc::PI_2, 0);
  qc2.rx(qc::PI_2, 1);
  qc2.cx(0, 1);
  qc2.rz(0.2, 1);
  qc2.cx(0, 1);
  qc2.rx(-qc::PI_2, 0);
  qc2.rx(-qc::PI_2, 1);
  config.execution.runZXChecker = true;
  config.execution.parallel = false;
  config.execution.runSimulationChecker = false;
  config.execution.runAlternatingChecker = false;
  config.execution.runConstructionChecker = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);

  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::Equivalent);
}
