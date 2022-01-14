/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"

class SimulationTest: public ::testing::Test {
protected:
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_alternative;
    ec::Configuration      config{};

    void SetUp() override {
        config.execution.runAlternatingScheme  = false;
        config.execution.runConstructionScheme = false;
        config.execution.runSimulationScheme   = true;
        config.execution.parallel              = false;

        config.simulation.maxSims        = 8;
        config.simulation.storeCEXinput  = true;
        config.simulation.storeCEXoutput = true;
        config.simulation.seed           = 12345;
    }
};

TEST_F(SimulationTest, Consistency) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_erroneous.real");

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();

    ec::EquivalenceCheckingManager ecm2(qc_original, qc_alternative, config);
    ecm2.run();

    EXPECT_EQ(ecm.getResults().performedSimulations, ecm2.getResults().performedSimulations);
}

TEST_F(SimulationTest, ClassicalStimuli) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");

    config.simulation.stateType = ec::StateType::ComputationalBasis;
    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());

    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::EquivalenceCheckingManager ecm2(qc_original, qc_alternative, config);
    ecm2.run();
    std::cout << ecm2.toString() << std::endl;
    EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}

TEST_F(SimulationTest, LocalStimuli) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");

    config.simulation.stateType = ec::StateType::Random1QBasis;
    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());

    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::EquivalenceCheckingManager ecm2(qc_original, qc_alternative, config);
    ecm2.run();
    std::cout << ecm2.toString() << std::endl;
    EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}

TEST_F(SimulationTest, GlobalStimuli) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");

    config.simulation.stateType = ec::StateType::Stabilizer;
    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());

    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::EquivalenceCheckingManager ecm2(qc_original, qc_alternative, config);
    ecm2.run();
    std::cout << ecm2.toString() << std::endl;
    EXPECT_FALSE(ecm2.getResults().consideredEquivalent());
}
