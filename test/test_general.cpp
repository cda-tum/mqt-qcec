/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "CompilationFlowEquivalenceChecker.hpp"
#include "SimulationBasedEquivalenceChecker.hpp"
#include "algorithms/BernsteinVazirani.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>
#include <sstream>
#include <string>

using ::testing::HasSubstr;

class GeneralTest: public ::testing::Test {
protected:
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_alternative;
};

TEST_F(GeneralTest, CSVOutput) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");

    ec::EquivalenceChecker ec(qc_original, qc_alternative);
    auto                   results = ec.check();

    std::stringstream ss{};
    results.printCSVEntry(ss);
    std::string csvEntry      = ss.str();
    std::string expectedEntry = "test_original;3;4;test_alternative;3;16;equivalent;";
    EXPECT_THAT(csvEntry, HasSubstr(expectedEntry));

    ss.str("");
    ss.clear();
    ec::EquivalenceCheckingResults::printCSVHeader(ss);
    std::string csvHeader      = ss.str();
    std::string expectedHeader = "filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;equivalent;t_pre;t_ver;maxActive;method;strategy;nsims;stimuliType";
    EXPECT_THAT(csvHeader, HasSubstr(expectedHeader));
}

TEST_F(GeneralTest, Output) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_alternative.real");

    ec::ImprovedDDEquivalenceChecker ec(qc_original, qc_alternative);
    auto                             results = ec.check();

    ec::EquivalenceCheckingResults::printCSVHeader();
    results.printCSVEntry();
    results.printJSON();
    results.print();
}

TEST_F(GeneralTest, InvalidInstance) {
    qc_original.import("./circuits/test/test_original.real");
    EXPECT_THROW(qc_alternative.import("./circuits/test/test_error.qasm"), qc::QFRException);
}

TEST_F(GeneralTest, NonUnitary) {
    std::string       bell_circuit_measure = "OPENQASM 2.0;\nqreg q[2];\ncreg c[2];\nU(pi/2,0,pi) q[0];\nCX q[0],q[1];\nreset q[0];\n";
    std::stringstream ss1{bell_circuit_measure};
    ASSERT_NO_THROW(qc_original.import(ss1, qc::OpenQASM));
    std::string       bell_circuit = "OPENQASM 2.0;\nqreg q[2];\nU(pi/2,0,pi) q[0];\nCX q[0],q[1];\n";
    std::stringstream ss2{bell_circuit};
    ASSERT_NO_THROW(qc_alternative.import(ss2, qc::OpenQASM));
    ec::EquivalenceChecker ec(qc_original, qc_alternative);
    EXPECT_THROW(ec.check(), std::runtime_error);
}

TEST_F(GeneralTest, SwitchDifferentlySizedCircuits) {
    qc_original.import("./circuits/original/dk27_225.real");
    qc_alternative.import("./circuits/transpiled/dk27_225_transpiled.qasm");

    ec::CompilationFlowEquivalenceChecker ec(qc_alternative, qc_original);
    auto                                  results = ec.check();
    EXPECT_TRUE(results.consideredEquivalent());
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
    auto                   results = ec.check();
    EXPECT_TRUE(results.consideredEquivalent());
}

TEST_F(GeneralTest, NonEquivalence) {
    qc_original.import("./circuits/test/test_original.real");
    qc_alternative.import("./circuits/test/test_erroneous.real");

    ec::EquivalenceChecker ec(qc_original, qc_alternative);
    auto                   results = ec.check();
    EXPECT_EQ(results.equivalence, ec::Equivalence::NotEquivalent);
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
    EXPECT_THROW(ec.check(), std::runtime_error);
}

TEST_F(GeneralTest, RemoveDiagonalGatesBeforeMeasure) {
    using namespace qc;
    unsigned short         nqubits = 1;
    qc::QuantumComputation qc1(nqubits);
    qc1.emplace_back<StandardOperation>(nqubits, 0, X);
    qc1.emplace_back<NonUnitaryOperation>(nqubits, std::vector<dd::Qubit>{0}, std::vector<std::size_t>{0});
    std::cout << qc1 << std::endl;
    std::cout << "-----------------------------" << std::endl;

    qc::QuantumComputation qc2(nqubits);
    qc2.emplace_back<StandardOperation>(nqubits, 0, X);
    qc2.emplace_back<StandardOperation>(nqubits, 0, Z);
    qc2.emplace_back<NonUnitaryOperation>(nqubits, std::vector<dd::Qubit>{0}, std::vector<std::size_t>{0});
    std::cout << qc2 << std::endl;
    std::cout << "-----------------------------" << std::endl;
    ec::EquivalenceChecker ec(qc1, qc2);
    auto                   results = ec.check();
    EXPECT_FALSE(results.consideredEquivalent());

    ec::SimulationBasedEquivalenceChecker ecpos(qc1, qc2);
    results = ecpos.check();
    EXPECT_TRUE(results.consideredEquivalent());

    ec::Configuration config{};
    config.removeDiagonalGatesBeforeMeasure = true;
    ec::EquivalenceChecker ec1(qc1, qc2);
    results = ec1.check(config);
    EXPECT_TRUE(results.consideredEquivalent());

    ec::ImprovedDDEquivalenceChecker ec2(qc1, qc2);
    results = ec2.check(config);
    EXPECT_TRUE(results.consideredEquivalent());

    ec::CompilationFlowEquivalenceChecker ec3(qc1, qc2);
    results = ec3.check(config);
    EXPECT_TRUE(results.consideredEquivalent());
}

TEST_F(GeneralTest, EquivalenceUpToGlobalPhase) {
    qc_original.addQubitRegister(1);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::X);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::Z);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::X);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::Z);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::X);
    std::cout << qc_original << std::endl;

    qc_alternative.addQubitRegister(1);
    qc_alternative.emplace_back<qc::StandardOperation>(1, 0, qc::X);
    std::cout << qc_alternative << std::endl;

    ec::EquivalenceChecker ec(qc_original, qc_alternative);
    auto                   results = ec.check();
    EXPECT_EQ(results.equivalence, ec::Equivalence::EquivalentUpToGlobalPhase);
    results.print();

    ec::ImprovedDDEquivalenceChecker ec2(qc_original, qc_alternative);
    auto                             results2 = ec2.check();
    EXPECT_EQ(results2.equivalence, ec::Equivalence::EquivalentUpToGlobalPhase);
    results2.print();
}

TEST_F(GeneralTest, InvalidStrategy) {
    qc_original.addQubitRegister(1);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::X);

    ec::ImprovedDDEquivalenceChecker ec2(qc_original, qc_original);
    ec::Configuration                config{};
    config.strategy = ec::Strategy::CompilationFlow;
    EXPECT_THROW(ec2.check(config), std::invalid_argument);
}

TEST_F(GeneralTest, FinishFirstCircuit) {
    qc_original.addQubitRegister(1);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::X);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::X);
    qc_original.emplace_back<qc::StandardOperation>(1, 0, qc::X);

    qc_alternative.addQubitRegister(1);
    qc_alternative.emplace_back<qc::StandardOperation>(1, 0, qc::X);

    ec::Configuration config{};
    config.strategy             = ec::Strategy::Naive;
    config.fuseSingleQubitGates = false;
    ec::ImprovedDDEquivalenceChecker ec(qc_original, qc_alternative);
    auto                             results = ec.check(config);
    EXPECT_TRUE(results.consideredEquivalent());
}

TEST_F(GeneralTest, CompilationFlowFinishSecondCircuit) {
    qc_original.addQubitRegister(2);
    qc_original.emplace_back<qc::StandardOperation>(2, dd::Control{0}, 1, qc::X);

    qc_alternative.addQubitRegister(2);
    qc_alternative.emplace_back<qc::StandardOperation>(2, 0, qc::H);
    qc_alternative.emplace_back<qc::StandardOperation>(2, 1, qc::H);
    qc_alternative.emplace_back<qc::StandardOperation>(2, dd::Control{1}, 0, qc::X);
    qc_alternative.emplace_back<qc::StandardOperation>(2, 1, qc::H);
    qc_alternative.emplace_back<qc::StandardOperation>(2, 0, qc::H);

    ec::CompilationFlowEquivalenceChecker ec(qc_original, qc_alternative);
    auto                                  results = ec.check();
    EXPECT_TRUE(results.consideredEquivalent());
}

TEST_F(GeneralTest, FixOutputPermutationMismatch) {
    qc_original.addQubitRegister(2);
    qc_original.emplace_back<qc::StandardOperation>(2, 0, qc::X);
    qc_original.emplace_back<qc::StandardOperation>(2, 1, qc::X);
    qc_original.setLogicalQubitAncillary(1);
    std::cout << qc_original << std::endl;

    qc_alternative.addQubitRegister(3);
    qc_alternative.emplace_back<qc::StandardOperation>(3, 0, qc::X);
    qc_alternative.emplace_back<qc::StandardOperation>(3, 1, qc::I);
    qc_alternative.emplace_back<qc::StandardOperation>(3, 2, qc::X);
    qc_alternative.outputPermutation.erase(1);
    qc_alternative.setLogicalQubitAncillary(1);
    qc_alternative.setLogicalQubitGarbage(1);
    std::cout << qc_alternative << std::endl;

    ec::EquivalenceChecker ec(qc_original, qc_alternative);
    auto                   results = ec.check();

    EXPECT_TRUE(results.consideredEquivalent());
}

TEST_F(GeneralTest, DynamicCircuit) {
    auto s   = qc::BitString(3);
    auto bv  = qc::BernsteinVazirani(s);
    auto dbv = qc::BernsteinVazirani(s, true);

    auto checker = ec::ImprovedDDEquivalenceChecker(bv, dbv);

    auto config                    = ec::Configuration{};
    config.transformDynamicCircuit = true;

    auto result = checker.check(config);
    EXPECT_EQ(result.equivalence, ec::Equivalence::Equivalent);
}
