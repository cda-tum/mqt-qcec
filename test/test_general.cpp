/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <string>
#include <sstream>

#include "gtest/gtest.h"
#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"

class GeneralTest : public ::testing::Test {
protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_alternative;

	std::string test_original_dir = "./circuits/original/";
	std::string test_alternative_dir = "./circuits/alternative/";

	void TearDown() override {
		qc_original.reset();
		qc_alternative.reset();
	}
};

TEST_F(GeneralTest, InvalidInstance) {
	qc::Format format = qc::Format::Real;
	qc_original.import(test_original_dir + "3_17_13.real", format);
	qc_alternative.import(test_alternative_dir + "qft_16.real", format);

	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ec.check();
	EXPECT_TRUE(ec.error());

	ec::ImprovedDDEquivalenceChecker iec(qc_original, qc_alternative);
	iec.check();
	EXPECT_TRUE(iec.error());
}

TEST_F(GeneralTest, TooManyQubits) {
	qc_original.import("./circuits/test_error.qasm", qc::OpenQASM);
	qc_alternative.import("./circuits/test_error.qasm", qc::OpenQASM);

	ec::EquivalenceChecker ec(qc_original, qc_alternative);
	ec.check();
	EXPECT_TRUE(ec.error());

	ec::ImprovedDDEquivalenceChecker iec(qc_original, qc_alternative);
	iec.check();
	EXPECT_TRUE(iec.error());
}
