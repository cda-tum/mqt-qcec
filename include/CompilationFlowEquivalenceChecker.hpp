/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP
#define QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP

#include <unordered_map>
#include <functional>
#include <utility>

#include "ImprovedDDEquivalenceChecker.hpp"
#include "CircuitOptimizer.hpp"

namespace ec {
	using CostFunction = std::function<unsigned short(const qc::Gate&, unsigned short)>;

	unsigned short IBMCostFunction(const qc::Gate& gate, unsigned short nc);

	class CompilationFlowEquivalenceChecker: public ImprovedDDEquivalenceChecker {

		CostFunction costFunction;

	public:
		CompilationFlowEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, CostFunction  costFunction = IBMCostFunction): ImprovedDDEquivalenceChecker(qc1, qc2), costFunction(std::move(costFunction)) {
			method = results.method = CompilationFlow;
			qc::CircuitOptimizer::swapGateFusion(qc1);
			qc::CircuitOptimizer::swapGateFusion(qc2);
		}

		void check(const Configuration& config) override;
	};
}

#endif //QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP
