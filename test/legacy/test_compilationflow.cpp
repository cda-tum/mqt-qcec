//
// This file is part of MQT QCEC library which is released under the MIT license.
// See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"
#include <functional>
#include <string>

class CompilationFlowTest: public testing::TestWithParam<std::string> {
protected:
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_transpiled;

    std::string test_original_dir   = "./circuits/original/";
    std::string test_transpiled_dir = "./circuits/transpiled/";

    ec::Configuration configuration{};

    void SetUp() override {
        qc_original.import(test_original_dir + GetParam() + ".real");
        qc_transpiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");

        configuration.execution.runAlternatingChecker  = true;
        configuration.execution.runConstructionChecker = false;
        configuration.execution.runSimulationChecker   = false;

        configuration.application.alternatingScheme = ec::ApplicationSchemeType::GateCost;
        configuration.application.costFunction      = ec::LegacyIBMCostFunction;
    }
};

INSTANTIATE_TEST_SUITE_P(CompilationFlowTest, CompilationFlowTest,
                         testing::Values(
                             "dk27_225",
                             "pcler8_248",
                             "5xp1_194",
                             "alu1_198",
                             "mlp4_245",
                             "dk17_224",
                             "add6_196",
                             "C7552_205",
                             "cu_219",
                             "example2_231",
                             "c2_181",
                             "rd73_312",
                             "cm150a_210",
                             "cm163a_213",
                             "c2_182",
                             "sym9_317",
                             "mod5adder_306",
                             "rd84_313",
                             "cm151a_211",
                             "apla_203"),
                         [](const testing::TestParamInfo<CompilationFlowTest::ParamType>& info) {
							 auto s = info.param;
							 std::replace( s.begin(), s.end(), '-', '_');
	                         return s; });

TEST_P(CompilationFlowTest, EquivalenceCompilationFlow) {
    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, configuration);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceCompilationFlowParallel) {
    configuration.execution.runSimulationChecker = true;
    configuration.execution.parallel             = true;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, configuration);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}
