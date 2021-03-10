/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include <string>
#include <algorithm>
#include <functional>

#include "gtest/gtest.h"
#include "SimulationBasedEquivalenceChecker.hpp"
#include "CompilationFlowEquivalenceChecker.hpp"

class CompilationFlowTest : public testing::TestWithParam<std::string> {

protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_transpiled;

	std::string test_original_dir = "./circuits/original/";
	std::string test_transpiled_dir = "./circuits/transpiled/";

	void SetUp() override {
		qc_original.import(test_original_dir + GetParam() + ".real");
		qc_transpiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");
	}
};

INSTANTIATE_TEST_SUITE_P(CompilationFlowTest, CompilationFlowTest,
                         testing::Values(
		                         "dk27_225",
		                         "pcler8_248",
		                         "5xp1_194",
		                         "alu1_198",
		                         "mlp4_245",
		                         "dk17_224",
		                         "add6_196",
		                         "C7552_205",
		                         "cu_219",
		                         "example2_231",
		                         "c2_181",
		                         "rd73_312",
		                         "cm150a_210",
		                         "cm163a_213",
		                         "c2_182",
		                         "sym9_317",
		                         "mod5adder_306",
		                         "rd84_313",
		                         "cm151a_211",
		                         "apla_203"),
                         [](const testing::TestParamInfo<CompilationFlowTest::ParamType>& info) {
							 auto s = info.param;
							 std::replace( s.begin(), s.end(), '-', '_');
	                         return s;});

TEST_P(CompilationFlowTest, EquivalenceCompilationFlow) {
	ec::CompilationFlowEquivalenceChecker ec_flow(qc_original, qc_transpiled);
	auto results = ec_flow.check();
	results.printCSVEntry();
	EXPECT_TRUE(results.consideredEquivalent());
}
