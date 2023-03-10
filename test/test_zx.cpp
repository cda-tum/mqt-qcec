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
  qcOriginal.import(
      std::stringstream("OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg "
                        "q[2];\ncx q[0], q[1];\n"),
      qc::Format::OpenQASM);
  qcAlternative.import(
      std::stringstream("OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\nh "
                        "q[0]; cx q[1], q[0]; h q[0]; h q[1];\n"),
      qc::Format::OpenQASM);
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal,
                                                         qcAlternative, config);

  ecm->run();
  EXPECT_FALSE(ecm->getResults().consideredEquivalent());
}

TEST_F(ZXTest, Timeout) {
  using namespace qc::literals;

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

  config.execution.timeout = 1;
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
  EXPECT_EQ(ecm->equivalence(), ec::EquivalenceCriterion::Equivalent);
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
  using namespace qc::literals;

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
  using namespace qc::literals;

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
  using namespace qc::literals;

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
  using namespace qc::literals;

  auto qc = qc::QuantumComputation(4);
  qc.x(0, {1_pc, 2_pc, 3_pc});

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qc, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, ZXConfiguredForInvalidCircuitSequential) {
  using namespace qc::literals;

  auto qc = qc::QuantumComputation(4);
  qc.x(0, {1_pc, 2_pc, 3_pc});

  config.execution.parallel = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qc, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}

TEST_F(ZXTest, GlobalPhase) {
  auto qc = qc::QuantumComputation(1);
  qc.rz(0, zx::PI / 8);

  auto qcPrime = qc::QuantumComputation(1);
  qcPrime.phase(0, zx::PI / 8);

  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc, qcPrime, config);
  ecm->run();

  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

class ZXTestCompFlow : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcTranspiled;
  ec::Configuration      config{};

  std::unique_ptr<ec::EquivalenceCheckingManager> ecm{};

  std::string testOriginalDir   = "./circuits/original/";
  std::string testTranspiledDir = "./circuits/transpiled/";

  void SetUp() override {
    config.execution.parallel               = false;
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runSimulationChecker   = false;
    config.execution.runZXChecker           = true;

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
  std::cout << ecm->toString() << std::endl;
  EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST(ZXTestsMisc, IdentityNotHadamard) {
  const auto qc1 = qc::QuantumComputation(1);
  auto       qc2 = qc::QuantumComputation(1);
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
  auto qc1 = qc::QuantumComputation(3U);
  qc1.h(0);
  qc1.x(1, 0_pc);
  qc1.x(2, 1_pc);
  qc1.measure(0, 0);
  qc1.measure(1, 1);
  qc1.measure(2, 2);
  qc1.initializeIOMapping();

  auto qc2 = qc::QuantumComputation(5U);
  qc2.h(1);
  qc2.x(0, 1_pc);
  qc2.swap(0, 2);
  qc2.x(4, 2_pc);
  qc2.measure(1, 0);
  qc2.measure(2, 1);
  qc2.measure(4, 2);
  qc2.initializeIOMapping();

  config.execution.runZXChecker           = true;
  config.execution.parallel               = false;
  config.execution.runSimulationChecker   = false;
  config.execution.runAlternatingChecker  = false;
  config.execution.runConstructionChecker = false;
  ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc1, qc2, config);

  ecm->run();
  EXPECT_EQ(ecm->getResults().equivalence,
            ec::EquivalenceCriterion::NoInformation);
}
