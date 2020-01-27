/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"
#include "CompilationFlowEquivalenceChecker.hpp"

int main() {
	std::string test_original = "./circuits/test_original.real";
	std::string test_alternative = "./circuits/test_alternative.real";
	std::string test_erroneous = "./circuits/test_erroneous.real";

	qc::QuantumComputation qc_original;
	qc_original.import(test_original);

	qc::QuantumComputation qc_alternative;
	qc_alternative.import(test_alternative);

	qc::QuantumComputation qc_erroneous;
	qc_erroneous.import(test_erroneous);

	std::vector<std::unique_ptr<ec::EquivalenceChecker>> eqs{};
	eqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_alternative, ec::Reference));
	eqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_alternative, ec::Naive));
	eqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_alternative, ec::Proportional));
	eqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_alternative, ec::Lookahead));

	for (auto& eq: eqs) {
		eq->expectEquivalent();
		eq->check(ec::Configuration{});
		eq->printResult(std::cout);
	}

	std::cout << "\n--------------------------------------------------\n\n";

	std::vector<std::unique_ptr<ec::EquivalenceChecker>> noneqs{};
	noneqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_erroneous, ec::Reference));
	noneqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_erroneous, ec::Naive));
	noneqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_erroneous, ec::Proportional));
	noneqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_erroneous, ec::Lookahead));

	for (auto& noneq: noneqs) {
		noneq->expectNonEquivalent();
		noneq->check(ec::Configuration{});
		noneq->printResult(std::cout);
	}
	return 0;
}
