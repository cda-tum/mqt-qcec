//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"
#include <iostream>
#include <string>

class FunctionalityTest : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcAlternative;
  ec::Configuration      config{};

  std::string testOriginal       = "./circuits/test/test.real";
  std::string testAlternativeDir = "./circuits/test/";

  void SetUp() override {
    qcOriginal.import(testOriginal);

    config.execution.parallel               = false;
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runSimulationChecker   = false;
    config.execution.runZXChecker           = false;
    config.simulation.maxSims               = 16;
    config.application.constructionScheme =
        ec::ApplicationSchemeType::Sequential;
    config.application.simulationScheme = ec::ApplicationSchemeType::Sequential;
    config.application.alternatingScheme =
        ec::ApplicationSchemeType::Sequential;
  }
};

INSTANTIATE_TEST_SUITE_P(
    TestCircuits, FunctionalityTest,
    testing::Values("inputperm", "ancilla", "ancilla_inputperm", "swap",
                    "outputperm", "ancilla_inputperm_outputperm",
                    "optimizedswap",
                    "ancilla_inputperm_outputperm_optimizedswap",
                    "ancilla_inputperm_outputperm_optimizedswap2"),
    [](const testing::TestParamInfo<FunctionalityTest::ParamType>& inf) {
      std::stringstream ss{};
      ss << inf.param;
      return ss.str();
    });

TEST_P(FunctionalityTest, Reference) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runConstructionChecker = true;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Proportional) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme =
      ec::ApplicationSchemeType::Proportional;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Lookahead) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme   = ec::ApplicationSchemeType::Lookahead;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Naive) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme   = ec::ApplicationSchemeType::OneToOne;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, CompilationFlow) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme   = ec::ApplicationSchemeType::GateCost;
  config.application.costFunction        = ec::legacyCostFunction;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Simulation) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runSimulationChecker = true;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, SimulationRandom1QBasis) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runSimulationChecker = true;
  config.simulation.stateType           = ec::StateType::Random1QBasis;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, SimulationStabilizer) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runSimulationChecker = true;
  config.simulation.stateType           = ec::StateType::Stabilizer;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, SimulationParallel) {
  qcAlternative.import(testAlternativeDir + "test_" + GetParam() + ".qasm");

  config.execution.runSimulationChecker = true;
  config.execution.parallel             = true;
  config.execution.nthreads = std::thread::hardware_concurrency() + 1U;
  config.simulation.maxSims = std::thread::hardware_concurrency() + 1U;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_F(FunctionalityTest, test2) {
  testOriginal = "./circuits/test/test2.real";
  qcOriginal.import(testOriginal);
  qcAlternative.import(testAlternativeDir + "test2_optimized.qasm");

  config.execution.runConstructionChecker = true;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());

  config.execution.runConstructionChecker = false;
  config.execution.runAlternatingChecker  = true;
  config.application.alternatingScheme =
      ec::ApplicationSchemeType::Proportional;

  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();
  std::cout << ecm2.getResults() << "\n";
  EXPECT_TRUE(ecm2.getResults().consideredEquivalent());

  config.application.alternatingScheme = ec::ApplicationSchemeType::Lookahead;

  ec::EquivalenceCheckingManager ecm3(qcOriginal, qcAlternative, config);
  ecm3.run();
  std::cout << ecm3.getResults() << "\n";
  EXPECT_TRUE(ecm3.getResults().consideredEquivalent());

  config.application.alternatingScheme = ec::ApplicationSchemeType::OneToOne;

  ec::EquivalenceCheckingManager ecm4(qcOriginal, qcAlternative, config);
  ecm4.run();
  std::cout << ecm4.getResults() << "\n";
  EXPECT_TRUE(ecm4.getResults().consideredEquivalent());

  config.application.alternatingScheme = ec::ApplicationSchemeType::GateCost;
  config.application.costFunction      = ec::legacyCostFunction;

  ec::EquivalenceCheckingManager ecm5(qcOriginal, qcAlternative, config);
  ecm5.run();
  std::cout << ecm5.getResults() << "\n";
  EXPECT_TRUE(ecm5.getResults().consideredEquivalent());
}
