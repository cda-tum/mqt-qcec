/*
 * This file is part of MQT QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "SimulationBasedEquivalenceChecker.hpp"

#include "gtest/gtest.h"

class SimulationTest: public ::testing::Test {
protected:
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_alternative;
    ec::Configuration      config{};

    void SetUp() override {
        config.storeCEXinput  = true;
        config.storeCEXoutput = true;
    }
};

TEST_F(SimulationTest, Consistency) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_erroneous.real");

    ec::SimulationBasedEquivalenceChecker ec(qc_original, qc_alternative, 12345);
    auto                                  results1 = ec.check();

    ec::SimulationBasedEquivalenceChecker ec2(qc_original, qc_alternative, 12345);
    auto                                  results2 = ec2.check();
    EXPECT_EQ(results1.nsims, results2.nsims);
}

TEST_F(SimulationTest, ClassicalStimuli) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");

    ec::SimulationBasedEquivalenceChecker ec(qc_original, qc_alternative, 12345);
    auto                                  results = ec.check(config);
    results.print();
    results.printJSON();

    EXPECT_EQ(results.equivalence, ec::Equivalence::Equivalent);
    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::SimulationBasedEquivalenceChecker ec2(qc_original, qc_alternative);
    auto                                  results2 = ec2.check(config);
    results2.print();
    results2.printJSON();
    EXPECT_FALSE(results2.consideredEquivalent());
}

TEST_F(SimulationTest, LocalStimuli) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");
    config.stimuliType = ec::StimuliType::LocalQuantum;
    ec::SimulationBasedEquivalenceChecker ec(qc_original, qc_alternative);
    auto                                  results = ec.check(config);
    results.print();
    results.printJSON();
    EXPECT_TRUE(results.consideredEquivalent());
    config.max_sims = 4;
    results         = ec.check(config);
    results.print();
    EXPECT_TRUE(results.consideredEquivalent());

    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::SimulationBasedEquivalenceChecker ec2(qc_original, qc_alternative);
    auto                                  results2 = ec2.check(config);
    results2.print();
    results2.printJSON();
    EXPECT_FALSE(results2.consideredEquivalent());
}

TEST_F(SimulationTest, GlobalStimuli) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");
    config.stimuliType = ec::StimuliType::GlobalQuantum;
    ec::SimulationBasedEquivalenceChecker ec(qc_original, qc_alternative);
    auto                                  results = ec.check(config);
    results.print();
    results.printJSON();
    EXPECT_TRUE(results.consideredEquivalent());
    config.max_sims = 4;
    results         = ec.check(config);
    results.print();
    EXPECT_TRUE(results.consideredEquivalent());

    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::SimulationBasedEquivalenceChecker ec2(qc_original, qc_alternative);
    auto                                  results2 = ec2.check(config);
    results2.print();
    results2.printJSON();
    EXPECT_FALSE(results2.consideredEquivalent());
}

TEST_F(SimulationTest, ZeroState) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");
    ec::SimulationBasedEquivalenceChecker ec(qc_original, qc_alternative);
    auto                                  results = ec.checkZeroState(config);
    results.print();
    results.printJSON();
    EXPECT_TRUE(results.consideredEquivalent());

    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::SimulationBasedEquivalenceChecker ec2(qc_original, qc_alternative);
    auto                                  results2 = ec2.checkZeroState(config);
    results2.print();
    results2.printJSON();
    EXPECT_FALSE(results2.consideredEquivalent());
}

TEST_F(SimulationTest, PlusState) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");
    ec::SimulationBasedEquivalenceChecker ec(qc_original, qc_alternative);
    auto                                  results = ec.checkPlusState(config);
    results.print();
    results.printJSON();
    EXPECT_TRUE(results.consideredEquivalent());

    qc_alternative.import("./circuits/test/test_erroneous.real");
    ec::SimulationBasedEquivalenceChecker ec2(qc_original, qc_alternative);
    auto                                  results2 = ec2.checkPlusState(config);
    results2.print();
    results2.printJSON();
    EXPECT_FALSE(results2.consideredEquivalent());
}
