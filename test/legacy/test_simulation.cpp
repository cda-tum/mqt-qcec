//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"

class SimulationTest : public ::testing::Test {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcAlternative;
  ec::Configuration      config{};

  void SetUp() override {
    config.execution.runAlternatingChecker  = false;
    config.execution.runConstructionChecker = false;
    config.execution.runSimulationChecker   = true;
    config.execution.parallel               = false;

    config.simulation.maxSims        = 8U;
    config.simulation.storeCEXinput  = true;
    config.simulation.storeCEXoutput = true;
    config.simulation.seed           = 12345U;
  }
};

TEST_F(SimulationTest, Consistency) {
  qcOriginal.import("./circuits/test/test_original.real");
  qcAlternative.import("./circuits/test/test_erroneous.real");

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();

  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();

  EXPECT_EQ(ecm.getResults().performedSimulations,
            ecm2.getResults().performedSimulations);
}

TEST_F(SimulationTest, ClassicalStimuli) {
  qcOriginal.import("./circuits/test/test_original.real");
  qcAlternative.import("./circuits/test/test_alternative.real");

  config.simulation.stateType = ec::StateType::ComputationalBasis;
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());

  qcAlternative.import("./circuits/test/test_erroneous.real");
  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();
  std::cout << ecm2.getResults() << "\n";
  EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}

TEST_F(SimulationTest, LocalStimuli) {
  qcOriginal.import("./circuits/test/test_original.real");
  qcAlternative.import("./circuits/test/test_alternative.real");

  config.simulation.stateType = ec::StateType::Random1QBasis;
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());

  qcAlternative.import("./circuits/test/test_erroneous.real");
  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();
  std::cout << ecm2.getResults() << "\n";
  EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}

TEST_F(SimulationTest, GlobalStimuli) {
  qcOriginal.import("./circuits/test/test_original.real");
  qcAlternative.import("./circuits/test/test_alternative.real");

  config.simulation.stateType = ec::StateType::Stabilizer;
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());

  qcAlternative.import("./circuits/test/test_erroneous.real");
  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();
  std::cout << ecm2.getResults() << "\n";
  EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}

TEST_F(SimulationTest, ClassicalStimuliParallel) {
  config.execution.parallel = true;
  qcOriginal.import("./circuits/test/test_original.real");
  qcAlternative.import("./circuits/test/test_alternative.real");

  config.simulation.stateType = ec::StateType::ComputationalBasis;
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());

  qcAlternative.import("./circuits/test/test_erroneous.real");
  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();
  std::cout << ecm2.getResults() << "\n";
  EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}

TEST_F(SimulationTest, LocalStimuliParallel) {
  config.execution.parallel = true;
  qcOriginal.import("./circuits/test/test_original.real");
  qcAlternative.import("./circuits/test/test_alternative.real");

  config.simulation.stateType = ec::StateType::Random1QBasis;
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());

  qcAlternative.import("./circuits/test/test_erroneous.real");
  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();
  std::cout << ecm2.getResults() << "\n";
  EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}

TEST_F(SimulationTest, GlobalStimuliParallel) {
  config.execution.parallel = true;
  qcOriginal.import("./circuits/test/test_original.real");
  qcAlternative.import("./circuits/test/test_alternative.real");

  config.simulation.stateType = ec::StateType::Stabilizer;
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcAlternative, config);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());

  qcAlternative.import("./circuits/test/test_erroneous.real");
  ec::EquivalenceCheckingManager ecm2(qcOriginal, qcAlternative, config);
  ecm2.run();
  std::cout << ecm2.getResults() << "\n";
  EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}
