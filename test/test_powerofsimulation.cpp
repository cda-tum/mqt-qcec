/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <functional>

#include "gtest/gtest.h"
#include "PowerOfSimulationEquivalenceChecker.hpp"

class PowerOfSimulationTest : public testing::TestWithParam<std::string> {
protected:
	qc::QuantumComputation qc_original;
	qc::QuantumComputation qc_transpiled;
	ec::Configuration config;

	std::string test_original_dir = "./circuits/original/";
	std::string test_transpiled_dir = "./circuits/transpiled/";

	std::mt19937_64 mt;
	std::function<unsigned long long()> rng;
	unsigned short tries = 20;
	double expected_success_probability = 0.50;

	void SetUp() override {
		qc_original.import(test_original_dir + GetParam() + ".real");
		qc_transpiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");

		std::array<std::mt19937_64::result_type , std::mt19937_64::state_size> random_data{};
		std::random_device rd;
		std::generate(begin(random_data), end(random_data), [&](){return rd();});
		std::seed_seq seeds(begin(random_data), end(random_data));
		mt.seed(seeds);
		std::uniform_int_distribution<unsigned long long> distribution(0, qc_transpiled.getNops()-1);
		rng = bind(distribution, ref(mt));

		#if DEBUG_MODE_SIMULATION
		tries = 100;
		#endif
	}

	void TearDown() override {

	}

};

INSTANTIATE_TEST_SUITE_P(PowerOfSimulationTest, PowerOfSimulationTest,
                         testing::Values(
		                         "4gt11_84", "4gt11-v1_85", "3_17_13", "3_17_15", "4_49_16", "4gt4-v0_72", "4mod5-v0_18", "4mod5-v0_19", "ham3_103", "0410184_169", "decod24-v1_42", "decod24-v3_46", "hwb4_51", "rd32-v0_67", "dk27_225"/*, "hwb8_118", "clip_206"*/),
                         [](const testing::TestParamInfo<PowerOfSimulationTest::ParamType>& info) {
	                         auto s = info.param;
	                         std::replace( s.begin(), s.end(), '-', '_');
	                         return s;});

TEST_P(PowerOfSimulationTest, NonEQRemove_1) {
	unsigned short gates_to_remove = std::min(qc_transpiled.getNops(), (size_t)1);
	unsigned short successes = 0;
	for (int i=0; i<tries; ++i) {
		std::set<unsigned long long> already_removed{};
		qc_original.import(test_original_dir + GetParam() + ".real");
		qc_transpiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");

		for (int j=0; j < gates_to_remove; ++j) {
			auto gate_to_remove = rng() % qc_transpiled.getNops();
			while (already_removed.count(gate_to_remove)) {
				gate_to_remove = rng() % qc_transpiled.getNops();
			}
			already_removed.insert(gate_to_remove);
			auto it = qc_transpiled.begin();
			if (it == qc_transpiled.end()) continue;
			std::advance(it, gate_to_remove);
			#if DEBUG_MODE_SIMULATION
			std::cout << "Removed gate index " << gate_to_remove << std::endl;
			//(*it)->print(std::cout, qc_transpiled.initialLayout);
			//std::cout << std::endl;
			#endif

			qc_transpiled.erase(it);
			#if DEBUG_MODE_SIMULATION
			qc_transpiled.dump(qc_transpiled.getName() + ".qasm");
			#endif
		}

		ASSERT_NO_THROW(ec::PowerOfSimulationEquivalenceChecker noneq_sim(qc_original, qc_transpiled);
		noneq_sim.expectNonEquivalent();
		noneq_sim.check(config);
		if(noneq_sim.results.equivalence == ec::NonEquivalent) {
			successes++;
		});
	}

	std::cout << "n: " << qc_transpiled.getNqubits() << ", m: " << qc_original.getNops() << "/" << qc_transpiled.getNops() << ", num_seeds: " << tries << ", p_success: " << (double)successes/(double)tries << std::endl;
	EXPECT_GE(successes, std::round(tries * expected_success_probability));
}

TEST_P(PowerOfSimulationTest, NonEQRemove_2) {
	unsigned short gates_to_remove = std::min(qc_transpiled.getNops(), (size_t)2);
	unsigned short successes = 0;
	for (int i=0; i<tries; ++i) {
		std::set<unsigned long long> already_removed{};
		qc_original.import(test_original_dir + GetParam() + ".real");
		qc_transpiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");

		for (int j=0; j < gates_to_remove; ++j) {
			auto gate_to_remove = rng() % qc_transpiled.getNops();
			while (already_removed.count(gate_to_remove)) {
				gate_to_remove = rng() % qc_transpiled.getNops();
			}
			already_removed.insert(gate_to_remove);
			auto it = qc_transpiled.begin();
			if (it == qc_transpiled.end()) continue;
			std::advance(it, gate_to_remove);
			qc_transpiled.erase(it);
			}

		ASSERT_NO_THROW(ec::PowerOfSimulationEquivalenceChecker noneq_sim(qc_original, qc_transpiled);
		noneq_sim.expectNonEquivalent();
		noneq_sim.check(config);
		if(noneq_sim.results.equivalence == ec::NonEquivalent) {
			successes++;
		});
	}

	std::cout << "n: " << qc_transpiled.getNqubits() << ", m: " << qc_original.getNops() << "/" << qc_transpiled.getNops() << ", num_seeds: " << tries << ", p_success: " << (double)successes/(double)tries << std::endl;
	EXPECT_GE(successes, std::round(tries * expected_success_probability));
}

TEST_P(PowerOfSimulationTest, NonEQRemove_3) {
	unsigned short gates_to_remove = std::min(qc_transpiled.getNops(), (size_t)3);
	unsigned short successes = 0;
	for (int i=0; i<tries; ++i) {
		std::set<unsigned long long> already_removed{};
		qc_original.import(test_original_dir + GetParam() + ".real");
		qc_transpiled.import(test_transpiled_dir + GetParam() + "_transpiled.qasm");

		for (int j=0; j < gates_to_remove; ++j) {
			auto gate_to_remove = rng() % qc_transpiled.getNops();
			while (already_removed.count(gate_to_remove)) {
				gate_to_remove = rng() % qc_transpiled.getNops();
			}
			already_removed.insert(gate_to_remove);
			auto it = qc_transpiled.begin();
			if (it == qc_transpiled.end()) continue;
			std::advance(it, gate_to_remove);
			qc_transpiled.erase(it);
		}

		ASSERT_NO_THROW(
		ec::PowerOfSimulationEquivalenceChecker noneq_sim(qc_original, qc_transpiled);
		noneq_sim.expectNonEquivalent();
		noneq_sim.check(config);
		if(noneq_sim.results.equivalence == ec::NonEquivalent) {
			successes++;
		});
	}
	std::cout << "n: " << qc_transpiled.getNqubits() << ", m: " << qc_original.getNops() << "/" << qc_transpiled.getNops() << ", num_seeds: " << tries << ", p_success: " << (double)successes/(double)tries << std::endl;
	EXPECT_GE(successes, std::round(tries * expected_success_probability));
}
