/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"
#include "CompilationFlowEquivalenceChecker.hpp"

class FunctionalityTest : public testing::TestWithParam<std::string> {

protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_alternative;
	ec::Configuration config{};

	std::string test_original = "./circuits/test/test.real";
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
			                 return ss.str();});

TEST_P(FunctionalityTest, Reference) {
	qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
	ec::EquivalenceChecker eq(qc_original, qc_alternative);
	eq.expectEquivalent();
	eq.check(config);
	eq.printResult(std::cout);
	EXPECT_EQ(eq.results.equivalence, ec::Equivalent);
}

TEST_P(FunctionalityTest, ReferenceFromImproved) {
	qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
	ec::ImprovedDDEquivalenceChecker eq_ref(qc_original, qc_alternative, ec::Reference);
	eq_ref.expectEquivalent();
	eq_ref.check(config);
	eq_ref.printResult(std::cout);
	EXPECT_EQ(eq_ref.results.equivalence, ec::Equivalent);
}

TEST_P(FunctionalityTest, Proportional) {
	qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
	ec::ImprovedDDEquivalenceChecker eq_proportional(qc_original, qc_alternative, ec::Proportional);
	eq_proportional.expectEquivalent();
	eq_proportional.check(config);
	eq_proportional.printResult(std::cout);
	EXPECT_EQ(eq_proportional.results.equivalence, ec::Equivalent);
}

TEST_P(FunctionalityTest, Lookahead) {
	qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
	ec::ImprovedDDEquivalenceChecker eq_lookahead(qc_original, qc_alternative, ec::Lookahead);
	eq_lookahead.expectEquivalent();
	eq_lookahead.check(config);
	eq_lookahead.printResult(std::cout);
	EXPECT_EQ(eq_lookahead.results.equivalence, ec::Equivalent);
}

TEST_P(FunctionalityTest, Naive) {
	qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
	ec::ImprovedDDEquivalenceChecker eq_naive(qc_original, qc_alternative, ec::Naive);
	eq_naive.expectEquivalent();
	eq_naive.check(config);
	eq_naive.printResult(std::cout);
	EXPECT_EQ(eq_naive.results.equivalence, ec::Equivalent);
}

TEST_P(FunctionalityTest, CompilationFlow) {
	qc_alternative.import(test_alternative_dir + "test_" + GetParam() + ".qasm");
	ec::CompilationFlowEquivalenceChecker eq_flow(qc_original, qc_alternative);
	eq_flow.expectEquivalent();
	eq_flow.check(config);
	eq_flow.printResult(std::cout);
	EXPECT_EQ(eq_flow.results.equivalence, ec::Equivalent);
}

TEST_F(FunctionalityTest, test2) {
	test_original = "./circuits/test/test2.real";
	qc_original.import(test_original);
	qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

	ec::EquivalenceChecker eq(qc_original, qc_alternative);
	eq.expectEquivalent();
	eq.check(config);
	eq.printResult(std::cout);
	EXPECT_EQ(eq.results.equivalence, ec::Equivalent);

	qc_original.import(test_original);
	qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

	ec::ImprovedDDEquivalenceChecker eq_proportional(qc_original, qc_alternative, ec::Proportional);
	eq_proportional.expectEquivalent();
	eq_proportional.check(config);
	eq_proportional.printResult(std::cout);
	EXPECT_EQ(eq_proportional.results.equivalence, ec::Equivalent);

	qc_original.import(test_original);
	qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

	ec::ImprovedDDEquivalenceChecker eq_lookahead(qc_original, qc_alternative, ec::Lookahead);
	eq_lookahead.expectEquivalent();
	eq_lookahead.check(config);
	eq_lookahead.printResult(std::cout);
	EXPECT_EQ(eq_lookahead.results.equivalence, ec::Equivalent);

	qc_original.import(test_original);
	qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

	ec::ImprovedDDEquivalenceChecker eq_naive(qc_original, qc_alternative, ec::Naive);
	eq_naive.expectEquivalent();
	eq_naive.check(config);
	eq_naive.printResult(std::cout);
	EXPECT_EQ(eq_naive.results.equivalence, ec::Equivalent);

	qc_original.import(test_original);
	qc_alternative.import(test_alternative_dir + "test2_optimized.qasm");

	ec::CompilationFlowEquivalenceChecker eq_flow(qc_original, qc_alternative);
	eq_flow.expectEquivalent();
	eq_flow.check(config);
	eq_flow.printResult(std::cout);
	EXPECT_EQ(eq_flow.results.equivalence, ec::Equivalent);
}

