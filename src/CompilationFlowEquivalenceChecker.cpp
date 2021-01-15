/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "CompilationFlowEquivalenceChecker.hpp"

namespace ec {

	void CompilationFlowEquivalenceChecker::check(const ec::Configuration& config) {
		if (!validInstance())
			return;

		setTolerance(config.tolerance);

		auto start = std::chrono::high_resolution_clock::now();

		qc::CircuitOptimizer::swapGateFusion(qc1);
		qc::CircuitOptimizer::swapGateFusion(qc2);

		qc::CircuitOptimizer::singleQubitGateFusion(qc1);
		qc::CircuitOptimizer::singleQubitGateFusion(qc2);

		if (config.removeDiagonalGatesBeforeMeasure) {
			qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc1);
			qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc2);
		}

		#if DEBUG_MODE_EC
		std::cout << "QC1: ";
		qc1->printRegisters();
		qc1->print();
		std::cout << "QC2: ";
		qc2->printRegisters();
		qc2->print();
		#endif

		qc::permutationMap perm1 = initial1;
		qc::permutationMap perm2 = initial2;
		results.result = createInitialMatrix();

		#if DEBUG_MODE_EC
		visited.clear();
		auto nc = nodecount(results.result, visited);
		maxSize = std::max(maxSize, nc-1);
		addToAverage(nc-1);

		std::stringstream eiss{};
		eiss << "flow_initial_" << filename1 << ".dot";
		dd->export2Dot(results.result, eiss.str().c_str());
		#endif

		it1 = qc1.begin();
		it2 = qc2.begin();
		end1 = qc1.end();
		end2 = qc2.end();

		while (it1 != end1 && it2 != end2) {

			// apply possible swaps
			while (it1 != end1 && (*it1)->getType() == qc::SWAP) {
				applyGate(*it1, results.result, perm1, LEFT);
				++it1;
			}

			while (it2 != end2 && (*it2)->getType() == qc::SWAP) {
				applyGate(*it2, results.result, perm2, RIGHT);
				++it2;
			}

			if (it1 != end1 && it2 != end2) {
				unsigned short cost1 = costFunction((*it1)->getType(), (*it1)->getControls().size());
				unsigned short cost2 = costFunction((*it2)->getType(), (*it2)->getControls().size());

				for (int i = 0; i < cost2 && it1 != end1; ++i) {
					applyGate(it1, results.result, perm1, end1, LEFT);
					++it1;

					// apply possible swaps
					while (it1 != end1 && (*it1)->getType() == qc::SWAP) {
						applyGate(*it1, results.result, perm1, LEFT);
						++it1;
					}

					#if DEBUG_MODE_EC
					visited.clear();
					auto nc = nodecount(results.result, visited);
					maxSize = std::max(maxSize, nc-1);
					addToAverage(nc-1);
					#endif
				}

				for (int i = 0; i < cost1 && it2 != end2; ++i) {
					applyGate(it2, results.result, perm2, end2, RIGHT);
					++it2;

					// apply possible swaps
					while (it2 != end2 && (*it2)->getType() == qc::SWAP) {
						applyGate(*it2, results.result, perm2, RIGHT);
						++it2;
					}

					#if DEBUG_MODE_EC
					visited.clear();
					auto nc = nodecount(results.result, visited);
					maxSize = std::max(maxSize, nc-1);
					addToAverage(nc-1);
					#endif
				}
			}
		}
		// finish first circuit
		while (it1 != end1) {
			applyGate(it1, results.result, perm1, end1, LEFT);
			++it1;

			#if DEBUG_MODE_EC
			visited.clear();
			auto nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc-1);
			addToAverage(nc-1);
			#endif
		}

		//finish second circuit
		while (it2 != end2) {
			applyGate(it2, results.result, perm2, end2, RIGHT);
			++it2;

			#if DEBUG_MODE_EC
			visited.clear();
			auto nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc-1);
			addToAverage(nc-1);
			#endif
		}

		qc::QuantumComputation::changePermutation(results.result, perm1, output1, line, dd, LEFT);
		qc::QuantumComputation::changePermutation(results.result, perm2, output2, line, dd, RIGHT);
		results.result = reduceGarbage(results.result, garbage1, LEFT);
		results.result = reduceGarbage(results.result, garbage2, RIGHT);
		results.result = reduceAncillae(results.result, ancillary1, LEFT);
		results.result = reduceAncillae(results.result, ancillary2, RIGHT);

		auto goal_matrix = createGoalMatrix();
		results.equivalence = equals(results.result, goal_matrix);

		#if DEBUG_MODE_EC
		std::stringstream ss{};
		ss << "result_flow_" << filename2 << ".dot";
		dd->export2Dot(results.result, ss.str().c_str());
		std::cout << "Max size: " << maxSize << std::endl;
		std::cout << "Avg size: " << average << std::endl;
		#endif

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();
		results.maxActive = dd->maxActive;

	}

	unsigned short IBMCostFunction(const qc::OpType& gate, unsigned short nc) {
		switch (gate) {
			case qc::I:
				return 1;

			case qc::X:
				if (nc <= 1) return 1;
				else {
					return 2 * (nc-2) * (2 * IBMCostFunction(qc::Phase, 0) + 2 * IBMCostFunction(qc::U2, 0) + 3 * IBMCostFunction(qc::X, 1))
					+ 6 * IBMCostFunction(qc::X, 1) + 8 * IBMCostFunction(qc::U3, 0);
				}
				//if (nc == 3) return 2 * (4 * IBMCostFunction(qc::U1, 1) + 3 * IBMCostFunction(qc::X, 1)) + IBMCostFunction(qc::X, 2);
				//7 * IBMCostFunction(qc::U1, 1) + 14 * IBMCostFunction(qc::H, 0) + 6 * IBMCostFunction(qc::X, 1);
				//if (nc == 4) return
				//2 * IBMCostFunction(qc::U1, 1) + 4 * IBMCostFunction(qc::H, 0) + 3 * IBMCostFunction(qc::X, 3);
				//else {
				//	int n = std::ceil(nc / 2.);
				//	return 2 * IBMCostFunction(qc::X, n+1) + 2 * IBMCostFunction(qc::X, nc-n+1);
				//}

			case qc::U3:
			case qc::U2:
			case qc::V:
			case qc::Vdag:
			case qc::RX:
			case qc::RY:
			case qc::H:
			case qc::SX:
			case qc::SXdag:
				if (nc == 0) return 1;
				if (nc == 1) return 2 * IBMCostFunction(qc::X, 1) + 4 * IBMCostFunction(qc::U3, 0);
				else
					return 2 * IBMCostFunction(qc::X, nc) + 4 * IBMCostFunction(qc::U3, 0); // heuristic

			case qc::Phase:
			case qc::S:
			case qc::Sdag:
			case qc::T:
			case qc::Tdag:
			case qc::RZ:
				if (nc == 0) return 1;
				if (nc == 1) return 2 * IBMCostFunction(qc::X, 1) + 3 * IBMCostFunction(qc::Phase, 0);
				else
					return 2 * IBMCostFunction(qc::X, nc) + 3 * IBMCostFunction(qc::U3, 0); // heuristic

			case qc::Y: case qc::Z:
				if (nc == 0) return 1;
				else
					return IBMCostFunction(qc::X, nc) + 2 * IBMCostFunction(qc::U3, 0);

			case qc::SWAP:
				return IBMCostFunction(qc::X, nc) + 2 * IBMCostFunction(qc::X, 1);

			case qc::iSWAP:
				return IBMCostFunction(qc::SWAP, nc) + 2 * IBMCostFunction(qc::S, nc-1) + IBMCostFunction(qc::Z, nc);

			case qc::Peres:
			case qc::Peresdag:
				return IBMCostFunction(qc::X, nc) + IBMCostFunction(qc::X, nc-1);

			case qc::Compound: // this assumes that compound operations only arise from single qubit fusion
			case qc::Measure:
			case qc::Barrier:
			case qc::ShowProbabilities:
			case qc::Snapshot:
				return 1; // these operations are assumed to incur no cost, but to advance the procedure 1 is used

			default:
				throw QCECException("No cost for gate " + std::to_string(gate));
		}
	}
}
