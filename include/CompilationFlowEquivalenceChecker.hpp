/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP
#define QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP

#include <unordered_map>
#include <functional>
#include <utility>

#include "ImprovedDDEquivalenceChecker.hpp"

namespace ec {
	using CostFunction = std::function<unsigned short(const qc::OpType&, unsigned short)>;

	unsigned short IBMCostFunction(const qc::OpType& gate, unsigned short nc);

	class CompilationFlowEquivalenceChecker: public ImprovedDDEquivalenceChecker {

		CostFunction costFunction;

	public:
		CompilationFlowEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, CostFunction  costFunction = IBMCostFunction): ImprovedDDEquivalenceChecker(qc1, qc2), costFunction(std::move(costFunction)) {
			method = results.method = CompilationFlow;
		}

		void check(const Configuration& config) override;
		void check() override { return check(Configuration{}); }
	};
}

#endif //QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP
