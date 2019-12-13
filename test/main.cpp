/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"

int main() {
	std::string test_original = "./circuits/test_original.real";
	std::string test_alternative = "./circuits/test_alternative.real";
	std::string test_erroneous = "./circuits/test_erroneous.real";
	qc::Format format = qc::Real;

	qc::QuantumComputation qc_original;
	qc_original.import(test_original, format);

	qc::QuantumComputation qc_alternative;
	qc_alternative.import(test_alternative, format);

	qc::QuantumComputation qc_erroneous;
	qc_erroneous.import(test_erroneous, format);

	std::vector<std::unique_ptr<ec::EquivalenceChecker>> eqs{};
	eqs.emplace_back(std::make_unique<ec::EquivalenceChecker>(qc_original, qc_alternative));
	eqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_alternative, ec::Naive));
	eqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_alternative, ec::Proportional));
	eqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_alternative, ec::Lookahead));

	for (auto& eq: eqs) {
		eq->expectEquivalent();
		eq->check();
		eq->printResult(std::cout);
		//eq->printSequence(std::cout);
	}

	std::cout << "\n--------------------------------------------------\n\n";

	std::vector<std::unique_ptr<ec::EquivalenceChecker>> noneqs{};
	noneqs.emplace_back(std::make_unique<ec::EquivalenceChecker>(qc_original, qc_erroneous));
	noneqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_erroneous, ec::Naive));
	noneqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_erroneous, ec::Proportional));
	noneqs.emplace_back(std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc_original, qc_erroneous, ec::Lookahead));

	for (auto& noneq: noneqs) {
		noneq->expectNonEquivalent();
		noneq->check();
		noneq->printResult(std::cout);
		//noneq->printSequence(std::cout);
	}

	return 0;
}
