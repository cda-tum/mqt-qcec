/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <CompilationFlowEquivalenceChecker.hpp>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "EquivalenceChecker.hpp"

using ::testing::HasSubstr;

class GeneralTest : public ::testing::Test {
protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_alternative;

	void TearDown() override {

	}
};

TEST_F(GeneralTest, CSVOutput) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_alternative.real");

	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ec.expectEquivalent();
	ec.check(ec::Configuration{});
	EXPECT_NO_THROW(ec.exportResultAsDot("result.dot"));
	EXPECT_FALSE(ec.error());

	std::stringstream ss{};
	ec.printCSVEntry(ss);
	std::string csvEntry = ss.str();
	std::string expectedEntry = "test_original;3;4;test_alternative;3;16;EQ  ;EQ  ;Reference;";
	EXPECT_THAT(csvEntry, HasSubstr(expectedEntry));

	ss.str("");
	ss.clear();
	ec.printCSVHeader(ss);
	std::string csvHeader = ss.str();
	std::string expectedHeader = "filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive;nsims";
	EXPECT_THAT(csvHeader, HasSubstr(expectedHeader));
}

TEST_F(GeneralTest, InvalidInstance) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_error.qasm");

	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ec.check(ec::Configuration{});
	EXPECT_EQ(ec.results.equivalence, ec::NoInformation);
	EXPECT_TRUE(ec.error());

	ec::CompilationFlowEquivalenceChecker cfec(qc_original, qc_alternative);
	cfec.check(ec::Configuration{});
	EXPECT_EQ(cfec.results.equivalence, ec::NoInformation);
	EXPECT_TRUE(cfec.error());
}

TEST_F(GeneralTest, NonUnitary) {
	std::string bell_circuit_measure = "OPENQASM 2.0;\nqreg q[2];\ncreg c[2];\nU(pi/2,0,pi) q[0];\nCX q[0],q[1];\nmeasure q[0] -> c[0];\n";
	std::stringstream ss1{bell_circuit_measure};
	ASSERT_NO_THROW(qc_original.import(ss1, qc::OpenQASM));
	std::string bell_circuit = "OPENQASM 2.0;\nqreg q[2];\nU(pi/2,0,pi) q[0];\nCX q[0],q[1];\n";
	std::stringstream ss2{bell_circuit};
	ASSERT_NO_THROW(qc_alternative.import(ss2, qc::OpenQASM));
	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	EXPECT_EXIT(ec.check(ec::Configuration{}), ::testing::ExitedWithCode(1),
	            "Functionality not unitary.");

	ec::EquivalenceChecker ec2(qc_alternative, qc_original);
	EXPECT_EXIT(ec2.check(ec::Configuration{}), ::testing::ExitedWithCode(1),
	            "Functionality not unitary.");
}

TEST_F(GeneralTest, SwitchDifferentlySizedCircuits) {
	qc_original.import("./circuits/original/dk27_225.real");
	qc_alternative.import("./circuits/transpiled/dk27_225_transpiled.qasm");

	ec::CompilationFlowEquivalenceChecker ec(qc_alternative, qc_original);
	ec.check(ec::Configuration{true, true});
	EXPECT_TRUE(ec.results.equivalence == ec::Equivalent || ec.results.equivalence == ec::EquivalentUpToGlobalPhase);
	EXPECT_FALSE(ec.error());
}
