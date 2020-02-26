/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"
#include "CompilationFlowEquivalenceChecker.hpp"
#include "PowerOfSimulationEquivalenceChecker.hpp"

class EQCircuitsTest : public testing::TestWithParam<std::string> {

protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_alternative;
	ec::Configuration config{};

	std::string test_original_dir = "./circuits/original/";
	std::string test_alternative_dir = "./circuits/alternative/";

	void SetUp() override {
		qc_original.import(test_original_dir + GetParam());
		qc_alternative.import(test_alternative_dir + GetParam());
	}

	void TearDown() override {

	}

};

INSTANTIATE_TEST_SUITE_P(SomeCircuits, EQCircuitsTest,
                         testing::Values(
				"3_17_13.real",
				"hwb7_59.real",
				"hwb8_114.real",
				"mod5adder_129.real",
				"grover_5.real",
				"grover.qasm"),
                         [](const testing::TestParamInfo<EQCircuitsTest::ParamType>& info) {
	                         auto s = info.param;
	                         std::replace( s.begin(), s.end(), '.', '_');
	                         return s;});

TEST_P(EQCircuitsTest, EquivalenceReference) {
	ec::EquivalenceChecker eq(qc_original, qc_alternative);
	eq.expectEquivalent();
	eq.check(config);
	eq.printResult(std::cout);
	EXPECT_EQ(eq.results.equivalence, ec::Equivalent);
}

TEST_P(EQCircuitsTest, EquivalenceReferenceFromImproved) {
	ec::ImprovedDDEquivalenceChecker eq_ref(qc_original, qc_alternative, ec::Reference);
	eq_ref.expectEquivalent();
	eq_ref.check(config);
	eq_ref.printResult(std::cout);
	EXPECT_EQ(eq_ref.results.equivalence, ec::Equivalent);
}

TEST_P(EQCircuitsTest, EquivalenceProportional) {
	ec::ImprovedDDEquivalenceChecker eq_proportional(qc_original, qc_alternative, ec::Proportional);
	eq_proportional.expectEquivalent();
	eq_proportional.check(config);
	eq_proportional.printResult(std::cout);
	EXPECT_EQ(eq_proportional.results.equivalence, ec::Equivalent);
}

TEST_P(EQCircuitsTest, EquivalenceLookahead) {
	ec::ImprovedDDEquivalenceChecker eq_lookahead(qc_original, qc_alternative, ec::Lookahead);
	eq_lookahead.expectEquivalent();
	eq_lookahead.check(config);
	eq_lookahead.printResult(std::cout);
	EXPECT_EQ(eq_lookahead.results.equivalence, ec::Equivalent);
}

TEST_P(EQCircuitsTest, EquivalenceNaive) {
	ec::ImprovedDDEquivalenceChecker eq_naive(qc_original, qc_alternative, ec::Naive);
	eq_naive.expectEquivalent();
	eq_naive.check(config);
	eq_naive.printResult(std::cout);
	EXPECT_EQ(eq_naive.results.equivalence, ec::Equivalent);
}

TEST_P(EQCircuitsTest, EquivalenceCompilationFlow) {
	ec::CompilationFlowEquivalenceChecker eq_flow(qc_original, qc_alternative);
	eq_flow.expectEquivalent();
	eq_flow.check(config);
	eq_flow.printResult(std::cout);
	EXPECT_EQ(eq_flow.results.equivalence, ec::Equivalent);
}

TEST_P(EQCircuitsTest, EquivalencePowerOfSimulation) {
	ec::PowerOfSimulationEquivalenceChecker eq_sim(qc_original, qc_alternative);
	eq_sim.expectEquivalent();
	eq_sim.check(config);
	eq_sim.printResult(std::cout);
	EXPECT_TRUE(eq_sim.results.consideredEquivalent());
}

