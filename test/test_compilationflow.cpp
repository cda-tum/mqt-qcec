/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <functional>

#include "gtest/gtest.h"
#include "PowerOfSimulationEquivalenceChecker.hpp"
#include "CompilationFlowEquivalenceChecker.hpp"

class CompilationFlowTest : public testing::TestWithParam<std::string> {

protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_transpiled;
	ec::Configuration config;

	std::string test_original_dir = "./circuits/original/";
	std::string test_transpiled_dir = "./circuits/transpiled/";

	void SetUp() override {
		qc_original.import(test_original_dir + GetParam() + ".real");
		qc_transpiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");
		config.augmentQubitRegisters = true;
	}

	void TearDown() override {

	}

};

INSTANTIATE_TEST_SUITE_P(CompilationFlowTest, CompilationFlowTest,
                         testing::Values(
		                         "4gt11_84", "4gt11-v1_85", "deutsch-josza_16_const", "3_17_13", "3_17_15", "4_49_16", "4gt4-v0_72", "4mod5-v0_18", "4mod5-v0_19", "ham3_103", "0410184_169", "decod24-v1_42", "decod24-v3_46", "hwb4_51", "rd32-v0_67", "dk27_225"),
                         [](const testing::TestParamInfo<CompilationFlowTest::ParamType>& info) {
							 auto s = info.param;
							 std::replace( s.begin(), s.end(), '-', '_');
	                         return s;});

TEST_P(CompilationFlowTest, EquivalenceCompilationFlow) {
	ec::CompilationFlowEquivalenceChecker ec_flow(qc_original, qc_transpiled);
	ec_flow.expectEquivalent();
	ec_flow.check(config);
	ec_flow.printResult(std::cout);
	EXPECT_TRUE(ec_flow.results.consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceReference) {
	ec::EquivalenceChecker eq(qc_original, qc_transpiled);
	eq.expectEquivalent();
	eq.check(config);
	eq.printResult(std::cout);
	EXPECT_TRUE(eq.results.consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceReferenceFromImproved) {
	ec::ImprovedDDEquivalenceChecker eq_ref(qc_original, qc_transpiled, ec::Reference);
	eq_ref.expectEquivalent();
	eq_ref.check(config);
	eq_ref.printResult(std::cout);
	EXPECT_TRUE(eq_ref.results.consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceNaive) {
	ec::ImprovedDDEquivalenceChecker eq_naive(qc_original, qc_transpiled, ec::Naive);
	eq_naive.expectEquivalent();
	eq_naive.check(config);
	eq_naive.printResult(std::cout);
	EXPECT_TRUE(eq_naive.results.consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceProportional) {
	ec::ImprovedDDEquivalenceChecker eq_proportional(qc_original, qc_transpiled, ec::Proportional);
	eq_proportional.expectEquivalent();
	eq_proportional.check(config);
	eq_proportional.printResult(std::cout);
	EXPECT_TRUE(eq_proportional.results.consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceLookahead) {
	ec::ImprovedDDEquivalenceChecker eq_lookahead(qc_original, qc_transpiled, ec::Lookahead);
	eq_lookahead.expectEquivalent();
	eq_lookahead.check(config);
	eq_lookahead.printResult(std::cout);
	EXPECT_TRUE(eq_lookahead.results.consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalencePowerOfSimulation) {
	ec::PowerOfSimulationEquivalenceChecker eq_sim(qc_original, qc_transpiled);
	eq_sim.expectEquivalent();
	eq_sim.check(config);
	eq_sim.printResult(std::cout);
	EXPECT_TRUE(eq_sim.results.consideredEquivalent());
}
