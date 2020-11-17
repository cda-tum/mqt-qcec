/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "gtest/gtest.h"
#include "PowerOfSimulationEquivalenceChecker.hpp"

class SimulationTest : public ::testing::Test {
protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_alternative;
	ec::Configuration config{};

	void SetUp() override {
		config.storeCEXinput = true;
		config.storeCEXoutput = true;
	}
};



TEST_F(SimulationTest, Consistency) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_erroneous.real");

	ec::PowerOfSimulationEquivalenceChecker ec(qc_original, qc_alternative, 12345);
	ec.expectEquivalent();
	EXPECT_NO_THROW(ec.check(););

	ec::PowerOfSimulationEquivalenceChecker ec2(qc_original, qc_alternative, 12345);
	ec2.expectEquivalent();
	EXPECT_NO_THROW(ec2.check(););

	EXPECT_EQ(ec.results.nsims, ec2.results.nsims);
}

TEST_F(SimulationTest, ClassicalStimuli) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_alternative.real");

	ec::PowerOfSimulationEquivalenceChecker ec(qc_original, qc_alternative, 12345);
	ec.expectEquivalent();
	EXPECT_NO_THROW(ec.check(config););
	ec.printResult();
	ec.printJSONResult(true);

	EXPECT_EQ(ec.results.equivalence, ec::Equivalence::Equivalent);
	qc_alternative.import("./circuits/test/test_erroneous.real");
	ec::PowerOfSimulationEquivalenceChecker ec2(qc_original, qc_alternative);
	ec2.expectNonEquivalent();
	EXPECT_NO_THROW(ec2.check(config););
	ec2.printResult();
	ec2.printJSONResult(true);
	EXPECT_FALSE(ec2.results.consideredEquivalent());
}

TEST_F(SimulationTest, LocalStimuli) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_alternative.real");
	config.stimuliType = ec::LocalQuantum;
	ec::PowerOfSimulationEquivalenceChecker ec(qc_original, qc_alternative);
	ec.expectEquivalent();
	EXPECT_NO_THROW(ec.check(config););
	ec.printResult();
	ec.printJSONResult(true);
	EXPECT_TRUE(ec.results.consideredEquivalent());
	config.max_sims = 4;
	EXPECT_NO_THROW(ec.check(config););
	ec.printResult();
	EXPECT_TRUE(ec.results.consideredEquivalent());

	qc_alternative.import("./circuits/test/test_erroneous.real");
	ec::PowerOfSimulationEquivalenceChecker ec2(qc_original, qc_alternative);
	ec2.expectNonEquivalent();
	EXPECT_NO_THROW(ec2.check(config););
	ec2.printResult();
	ec2.printJSONResult(true);
	EXPECT_FALSE(ec2.results.consideredEquivalent());
}

TEST_F(SimulationTest, GlobalStimuli) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_alternative.real");
	config.stimuliType = ec::GlobalQuantum;
	ec::PowerOfSimulationEquivalenceChecker ec(qc_original, qc_alternative);
	ec.expectEquivalent();
	EXPECT_NO_THROW(ec.check(config););
	ec.printResult();
	ec.printJSONResult(true);
	EXPECT_TRUE(ec.results.consideredEquivalent());
	config.max_sims = 4;
	EXPECT_NO_THROW(ec.check(config););
	ec.printResult();
	EXPECT_TRUE(ec.results.consideredEquivalent());

	qc_alternative.import("./circuits/test/test_erroneous.real");
	ec::PowerOfSimulationEquivalenceChecker ec2(qc_original, qc_alternative);
	ec2.expectNonEquivalent();
	EXPECT_NO_THROW(ec2.check(config););
	ec2.printResult();
	ec2.printJSONResult(true);
	EXPECT_FALSE(ec2.results.consideredEquivalent());
}
