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
#include "PowerOfSimulationEquivalenceChecker.hpp"

using ::testing::HasSubstr;

class GeneralTest : public ::testing::Test {
protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_alternative;
};

TEST_F(GeneralTest, CSVOutput) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_alternative.real");

	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ec.expectEquivalent();
	ec.check();
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

TEST_F(GeneralTest, Output) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_alternative.real");

	ec::ImprovedDDEquivalenceChecker ec(qc_original, qc_alternative);
	ec.expectEquivalent();
	EXPECT_NO_THROW(ec.check(););

	ec.printCSVHeader();
	ec.printCSVEntry();
	ec.printJSONResult(true);
	ec.printResult();
}

TEST_F(GeneralTest, InvalidInstance) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_error.qasm");

	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ec.check();
	EXPECT_EQ(ec.results.equivalence, ec::NoInformation);
	EXPECT_TRUE(ec.error());

	ec::CompilationFlowEquivalenceChecker cfec(qc_original, qc_alternative);
	cfec.check();
	EXPECT_EQ(cfec.results.equivalence, ec::NoInformation);
	EXPECT_TRUE(cfec.error());
}

TEST_F(GeneralTest, NonUnitary) {
	std::string bell_circuit_measure = "OPENQASM 2.0;\nqreg q[2];\ncreg c[2];\nU(pi/2,0,pi) q[0];\nCX q[0],q[1];\nreset q[0];\n";
	std::stringstream ss1{bell_circuit_measure};
	ASSERT_NO_THROW(qc_original.import(ss1, qc::OpenQASM));
	std::string bell_circuit = "OPENQASM 2.0;\nqreg q[2];\nU(pi/2,0,pi) q[0];\nCX q[0],q[1];\n";
	std::stringstream ss2{bell_circuit};
	ASSERT_NO_THROW(qc_alternative.import(ss2, qc::OpenQASM));
	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	EXPECT_THROW(ec.check(), qc::QFRException);

	ec::EquivalenceChecker ec2(qc_alternative, qc_original);
	EXPECT_THROW(ec.check(), qc::QFRException);
}

TEST_F(GeneralTest, SwitchDifferentlySizedCircuits) {
	qc_original.import("./circuits/original/dk27_225.real");
	qc_alternative.import("./circuits/transpiled/dk27_225_transpiled.qasm");

	ec::CompilationFlowEquivalenceChecker ec(qc_alternative, qc_original);
	ec.check();
	EXPECT_TRUE(ec.results.equivalence == ec::Equivalent || ec.results.equivalence == ec::EquivalentUpToGlobalPhase);
	EXPECT_FALSE(ec.error());
}

TEST_F(GeneralTest, MeasurementPermutation) {
	std::string bell_circuit_measure =
			"OPENQASM 2.0;\n"
            "qreg q[2];\n"
			"creg c[2];\n"
            "U(pi/2,0,pi) q[0];\n"
			"CX q[0],q[1];\n"
			"measure q[0] -> c[1];\n"
			"measure q[1] -> c[0];\n";
	std::stringstream ss1{bell_circuit_measure};
	ASSERT_NO_THROW(qc_original.import(ss1, qc::OpenQASM));
	std::string bell_circuit =
			"// i 0 1\n"
            "// o 1 0\n"
			"OPENQASM 2.0;\n"
            "qreg q[2];\n"
            "U(pi/2,0,pi) q[0];\n"
            "CX q[0],q[1];\n";
	std::stringstream ss2{bell_circuit};
	ASSERT_NO_THROW(qc_alternative.import(ss2, qc::OpenQASM));
	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ASSERT_NO_THROW(ec.check());
	EXPECT_TRUE(ec.results.consideredEquivalent());
}

TEST_F(GeneralTest, NonEquivalence) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_erroneous.real");

	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ec.expectNonEquivalent();
	EXPECT_NO_THROW(ec.check(););
	EXPECT_EQ(ec.results.equivalence, ec::NonEquivalent);
}

TEST_F(GeneralTest, PowerOfSimulationConsistency) {
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

TEST_F(GeneralTest, PowerOfSimulationProveEquivalence) {
	qc_original.import("./circuits/test/test_original.real");
	qc_alternative.import("./circuits/test/test_alternative.real");

	ec::PowerOfSimulationEquivalenceChecker ec(qc_original, qc_alternative, 12345);
	ec.expectEquivalent();
	EXPECT_NO_THROW(ec.check(););
	ec.printResult();

	EXPECT_EQ(ec.results.equivalence,ec.results.consideredEquivalent());
}

TEST_F(GeneralTest, IntermediateMeasurementNotSupported) {
	std::string bell_circuit_measure =
			"OPENQASM 2.0;\n"
			"qreg q[2];\n"
			"creg c[2];\n"
			"U(pi/2,0,pi) q[0];\n"
			"measure q[0] -> c[0];\n"
			"CX q[0],q[1];\n"
			"measure q[1] -> c[0];\n";
	std::stringstream ss1{bell_circuit_measure};
	ASSERT_NO_THROW(qc_original.import(ss1, qc::OpenQASM));
	std::cout << qc_original << std::endl;
	std::string bell_circuit =
			"// i 0 1\n"
			"// o 1 0\n"
			"OPENQASM 2.0;\n"
			"qreg q[2];\n"
			"U(pi/2,0,pi) q[0];\n"
			"CX q[0],q[1];\n";
	std::stringstream ss2{bell_circuit};
	ASSERT_NO_THROW(qc_alternative.import(ss2, qc::OpenQASM));
	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	EXPECT_THROW(ec.check(), ec::QCECException);
	ec.printResult();
}

TEST_F(GeneralTest, RemoveDiagonalGatesBeforeMeasure) {
	using namespace qc;
	unsigned short nqubits = 1;
	qc::QuantumComputation qc1(nqubits);
	qc1.emplace_back<StandardOperation>(nqubits, 0, X);
	qc1.emplace_back<NonUnitaryOperation>(nqubits, std::vector<unsigned short>{0}, std::vector<unsigned short>{0});
	std::cout << qc1 << std::endl;
	std::cout << "-----------------------------" << std::endl;

	qc::QuantumComputation qc2(nqubits);
	qc2.emplace_back<StandardOperation>(nqubits, 0, X);
	qc2.emplace_back<StandardOperation>(nqubits, 0, Z);
	qc2.emplace_back<NonUnitaryOperation>(nqubits, std::vector<unsigned short>{0}, std::vector<unsigned short>{0});
	std::cout << qc2 << std::endl;
	std::cout << "-----------------------------" << std::endl;
	ec::EquivalenceChecker ec(qc1, qc2);
	ec.check();
	EXPECT_FALSE(ec.results.consideredEquivalent());

	ec::PowerOfSimulationEquivalenceChecker ecpos(qc1, qc2);
	ecpos.check();
	EXPECT_TRUE(ecpos.results.consideredEquivalent());

	ec::Configuration config{};
	config.removeDiagonalGatesBeforeMeasure = true;
	ec::EquivalenceChecker ec1(qc1, qc2);
	ec1.check(config);
	EXPECT_TRUE(ec1.results.consideredEquivalent());

	ec::ImprovedDDEquivalenceChecker ec2(qc1, qc2);
	ec2.check(config);
	EXPECT_TRUE(ec2.results.consideredEquivalent());

	ec::CompilationFlowEquivalenceChecker ec3(qc1, qc2);
	ec3.check(config);
	EXPECT_TRUE(ec3.results.consideredEquivalent());

}
