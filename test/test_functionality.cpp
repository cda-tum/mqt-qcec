/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "CompilationFlowEquivalenceChecker.hpp"
#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"

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
    ec::EquivalenceChecker eq(qc_original, qc_alternative);
    auto                   results = eq.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);
}

TEST_P(FunctionalityTest, Proportional) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
    ec::ImprovedDDEquivalenceChecker eq_proportional(qc_original, qc_alternative);
    config.strategy = ec::Strategy::Proportional;
    auto results    = eq_proportional.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);
}

TEST_P(FunctionalityTest, Lookahead) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
    ec::ImprovedDDEquivalenceChecker eq_lookahead(qc_original, qc_alternative);
    config.strategy = ec::Strategy::Lookahead;
    auto results    = eq_lookahead.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);
}

TEST_P(FunctionalityTest, Naive) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
    ec::ImprovedDDEquivalenceChecker eq_naive(qc_original, qc_alternative);
    config.strategy = ec::Strategy::Naive;
    auto results    = eq_naive.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);
}

TEST_P(FunctionalityTest, CompilationFlow) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
    ec::CompilationFlowEquivalenceChecker eq_flow(qc_original, qc_alternative);
    auto                                  results = eq_flow.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);
}

TEST_P(FunctionalityTest, Optimizations) {
    qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
    ec::EquivalenceChecker eq(qc_original, qc_alternative);
    config.fuseSingleQubitGates = true;
    auto results                = eq.check(config);
    results.print();
    EXPECT_TRUE(results.consideredEquivalent());
    config.reconstructSWAPs = true;
    results                 = eq.check(config);
    results.print();
    EXPECT_TRUE(results.consideredEquivalent());
    config.removeDiagonalGatesBeforeMeasure = true;
    results                                 = eq.check(config);
    results.print();
    EXPECT_TRUE(results.consideredEquivalent());
}

TEST_F(FunctionalityTest, test2) {
    test_original = "./circuits/test/test2.real";
    qc_original.import(test_original);
    qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

    ec::EquivalenceChecker eq(qc_original, qc_alternative);
    auto                   results = eq.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);

    qc_original.import(test_original);
    qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

    ec::ImprovedDDEquivalenceChecker eq_proportional(qc_original, qc_alternative);
    config.strategy = ec::Strategy::Proportional;
    results         = eq_proportional.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);

    qc_original.import(test_original);
    qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

    ec::ImprovedDDEquivalenceChecker eq_lookahead(qc_original, qc_alternative);
    config.strategy = ec::Strategy::Lookahead;
    results         = eq_lookahead.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);

    qc_original.import(test_original);
    qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

    ec::ImprovedDDEquivalenceChecker eq_naive(qc_original, qc_alternative);
    config.strategy = ec::Strategy::Naive;
    results         = eq_naive.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);

    qc_original.import(test_original);
    qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

    ec::CompilationFlowEquivalenceChecker eq_flow(qc_original, qc_alternative);
    results = eq_flow.check(config);
    results.print();
    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);
}
