/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"

class NonEQCircuitsTest : public testing::TestWithParam<std::string> {

protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_erroneous;

	std::string test_original_dir = "./circuits/original/";
	std::string test_erroneous_dir = "./circuits/erroneous/";

	void SetUp() override {
		qc::Format format = qc::Real;
		qc_original.import(test_original_dir + GetParam() + ".real", format);
		qc_erroneous.import(test_erroneous_dir + GetParam() + ".real", format);
	}

	void TearDown() override {
		qc_original.reset();
		qc_erroneous.reset();
	}

};

INSTANTIATE_TEST_SUITE_P(SomeCircuits, NonEQCircuitsTest,
                         testing::Values(
				"3_17_13",
				"hwb7_59",
				"hwb8_114",
				"grover_5"
				),
                         [](const testing::TestParamInfo<NonEQCircuitsTest::ParamType>& info) {
			                 std::stringstream ss{};
			                 ss << info.param;
			                 return ss.str();});

TEST_P(NonEQCircuitsTest, NonEquivalenceReference) {
	ec::EquivalenceChecker noneq(qc_original, qc_erroneous);
	noneq.expectNonEquivalent();
	noneq.check();
	noneq.printResult(std::cout);
	EXPECT_EQ(noneq.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceReferenceFromImproved) {
	ec::ImprovedDDEquivalenceChecker noneq_ref(qc_original, qc_erroneous, ec::Reference);
	noneq_ref.expectNonEquivalent();
	noneq_ref.check();
	noneq_ref.printResult(std::cout);
	EXPECT_EQ(noneq_ref.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceProportional) {
	ec::ImprovedDDEquivalenceChecker noneq_proportional(qc_original, qc_erroneous, ec::Proportional);
	noneq_proportional.expectNonEquivalent();
	noneq_proportional.check();
	noneq_proportional.printResult(std::cout);
	EXPECT_EQ(noneq_proportional.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceLookahed) {
	ec::ImprovedDDEquivalenceChecker noneq_lookahead(qc_original, qc_erroneous, ec::Lookahead);
	noneq_lookahead.expectNonEquivalent();
	noneq_lookahead.check();
	noneq_lookahead.printResult(std::cout);
	EXPECT_EQ(noneq_lookahead.results.equivalence, ec::NonEquivalent);
}

TEST_P(NonEQCircuitsTest, NonEquivalenceNaive) {
	ec::ImprovedDDEquivalenceChecker noneq_naive(qc_original, qc_erroneous, ec::Naive);
	noneq_naive.expectNonEquivalent();
	noneq_naive.check();
	noneq_naive.printResult(std::cout);
	EXPECT_EQ(noneq_naive.results.equivalence, ec::NonEquivalent);
}


