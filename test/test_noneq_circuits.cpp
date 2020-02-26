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

class NonEQCircuitsTest : public testing::TestWithParam<std::string> {

protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_erroneous;
	ec::Configuration config{};

	std::string test_original_dir = "./circuits/original/";
	std::string test_erroneous_dir = "./circuits/erroneous/";

	void SetUp() override {
		qc_original.import(test_original_dir + GetParam());
		qc_erroneous.import(test_erroneous_dir + GetParam());
	}

	void TearDown() override {

	}

};

INSTANTIATE_TEST_SUITE_P(SomeCircuits, NonEQCircuitsTest,
                         testing::Values(
				"3_17_13.real",
				"hwb7_59.real",
				"hwb8_114.real",
				"grover_5.real",
				"grover.qasm"
				),
                         [](const testing::TestParamInfo<NonEQCircuitsTest::ParamType>& info) {
	                         auto s = info.param;
	                         std::replace( s.begin(), s.end(), '.', '_');
	                         return s;});

TEST_P(NonEQCircuitsTest, NonEquivalenceReference) {
	ec::EquivalenceChecker noneq(qc_original, qc_erroneous);
	noneq.expectNonEquivalent();
	noneq.check(config);
	noneq.printResult(std::cout);
	EXPECT_EQ(noneq.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceReferenceFromImproved) {
	ec::ImprovedDDEquivalenceChecker noneq_ref(qc_original, qc_erroneous, ec::Reference);
	noneq_ref.expectNonEquivalent();
	noneq_ref.check(config);
	noneq_ref.printResult(std::cout);
	EXPECT_EQ(noneq_ref.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceProportional) {
	ec::ImprovedDDEquivalenceChecker noneq_proportional(qc_original, qc_erroneous, ec::Proportional);
	noneq_proportional.expectNonEquivalent();
	noneq_proportional.check(config);
	noneq_proportional.printResult(std::cout);
	EXPECT_EQ(noneq_proportional.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceLookahed) {
	ec::ImprovedDDEquivalenceChecker noneq_lookahead(qc_original, qc_erroneous, ec::Lookahead);
	noneq_lookahead.expectNonEquivalent();
	noneq_lookahead.check(config);
	noneq_lookahead.printResult(std::cout);
	EXPECT_EQ(noneq_lookahead.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceNaive) {
	ec::ImprovedDDEquivalenceChecker noneq_naive(qc_original, qc_erroneous, ec::Naive);
	noneq_naive.expectNonEquivalent();
	noneq_naive.check(config);
	noneq_naive.printResult(std::cout);
	EXPECT_EQ(noneq_naive.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceCompilationFlow) {
	ec::ImprovedDDEquivalenceChecker noneq_flow(qc_original, qc_erroneous);
	noneq_flow.expectNonEquivalent();
	noneq_flow.check(config);
	noneq_flow.printResult(std::cout);
	EXPECT_EQ(noneq_flow.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalencePowerOfSimulation) {
	ec::PowerOfSimulationEquivalenceChecker noneq_sim(qc_original, qc_erroneous);
	noneq_sim.expectNonEquivalent();
	noneq_sim.check(config);
	noneq_sim.printResult(std::cout);
	EXPECT_EQ(noneq_sim.results.equivalence, ec::NonEquivalent);
}
