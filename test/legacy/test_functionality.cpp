//
// This file is part of MQT QCEC library which is released under the MIT license.
// See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"
#include <iostream>
#include <string>

class FunctionalityTest: public testing::TestWithParam<std::string> {
protected:
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_alternative;
    ec::Configuration      config{};

    std::string test_original        = "./circuits/test/test.real";
    std::string test_alternative_dir = "./circuits/test/";

    void SetUp() override {
        qc_original.import(test_original);

        config.execution.parallel               = false;
        config.execution.runConstructionChecker = false;
        config.execution.runAlternatingChecker  = false;
        config.execution.runSimulationChecker   = false;
        config.simulation.maxSims               = 16;
        config.application.constructionScheme   = ec::ApplicationSchemeType::Sequential;
        config.application.simulationScheme     = ec::ApplicationSchemeType::Sequential;
        config.application.alternatingScheme    = ec::ApplicationSchemeType::Sequential;
    }
};

INSTANTIATE_TEST_SUITE_P(TestCircuits, FunctionalityTest,
                         testing::Values(
                                 "inputperm", "ancilla", "ancilla_inputperm", "swap",
                                 "outputperm", "ancilla_inputperm_outputperm",
                                 "optimizedswap", "ancilla_inputperm_outputperm_optimizedswap",
                                 "ancilla_inputperm_outputperm_optimizedswap2"),
                         [](const testing::TestParamInfo<FunctionalityTest::ParamType>& info) {
			                 std::stringstream ss{};
			                 ss << info.param;
			                 return ss.str(); });

TEST_P(FunctionalityTest, Reference) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runConstructionChecker = true;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Proportional) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::Proportional;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Lookahead) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::Lookahead;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Naive) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::OneToOne;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, CompilationFlow) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::GateCost;
    config.application.costFunction        = ec::LegacyIBMCostFunction;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, Simulation) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runSimulationChecker = true;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, SimulationRandom1QBasis) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runSimulationChecker = true;
    config.simulation.stateType           = ec::StateType::Random1QBasis;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, SimulationStabilizer) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runSimulationChecker = true;
    config.simulation.stateType           = ec::StateType::Stabilizer;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(FunctionalityTest, SimulationParallel) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");

    config.execution.runSimulationChecker = true;
    config.execution.parallel             = true;
    config.execution.nthreads             = std::thread::hardware_concurrency() + 1U;
    config.simulation.maxSims             = std::thread::hardware_concurrency() + 1U;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_F(FunctionalityTest, test2) {
    test_original = "./circuits/test/test2.real";
    qc_original.import(test_original);
    qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

    config.execution.runConstructionChecker = true;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_alternative, config);
    ecm.run();
    std::cout << ecm << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());

    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker  = true;
    config.application.alternatingScheme    = ec::ApplicationSchemeType::Proportional;

    ec::EquivalenceCheckingManager ecm2(qc_original, qc_alternative, config);
    ecm2.run();
    std::cout << ecm2.toString() << std::endl;
    EXPECT_TRUE(ecm2.getResults().consideredEquivalent());

    config.application.alternatingScheme = ec::ApplicationSchemeType::Lookahead;

    ec::EquivalenceCheckingManager ecm3(qc_original, qc_alternative, config);
    ecm3.run();
    std::cout << ecm3.toString() << std::endl;
    EXPECT_TRUE(ecm3.getResults().consideredEquivalent());

    config.application.alternatingScheme = ec::ApplicationSchemeType::OneToOne;

    ec::EquivalenceCheckingManager ecm4(qc_original, qc_alternative, config);
    ecm4.run();
    std::cout << ecm4.toString() << std::endl;
    EXPECT_TRUE(ecm4.getResults().consideredEquivalent());

    config.application.alternatingScheme = ec::ApplicationSchemeType::GateCost;
    config.application.costFunction      = ec::LegacyIBMCostFunction;

    ec::EquivalenceCheckingManager ecm5(qc_original, qc_alternative, config);
    ecm5.run();
    std::cout << ecm5.toString() << std::endl;
    EXPECT_TRUE(ecm5.getResults().consideredEquivalent());
}
