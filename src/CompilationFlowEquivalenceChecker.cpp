/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "CompilationFlowEquivalenceChecker.hpp"

namespace ec {

	void CompilationFlowEquivalenceChecker::check(const ec::Configuration& config) {
		if (!validInstance())
			return;

		auto start = std::chrono::high_resolution_clock::now();

		// reduce both circuits qubits to a minimum by stripping away idle qubits
		qc1.stripIdleQubits();
		qc2.stripIdleQubits();

		#if DEBUG_MODE_EC
		std::cout << "QC1: ";
		qc1->printRegisters();
		qc1->print();
		std::cout << "QC2: ";
		qc2->printRegisters();
		qc2->print();
		#endif

		// augment the smaller circuit with ancillary qubits and change the qubits in the larger circuit to ancillary
		augmentQubits(qc1, qc2);

		qc::permutationMap perm1 = qc1.initialLayout;
		qc::permutationMap perm2 = qc2.initialLayout;
		results.result = qc1.createInitialMatrix(dd);
		dd->incRef(results.result);

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
			while (it1 != end1 && dynamic_cast<qc::StandardOperation *>(it1->get())->getGate() == qc::SWAP) {
				applyGate(*it1, results.result, perm1, LEFT);
				++it1;
			}

			while (it2 != end2 && dynamic_cast<qc::StandardOperation *>(it2->get())->getGate() == qc::SWAP) {
				applyGate(*it2, results.result, perm2, RIGHT);
				++it2;
			}

			if (it1 != end1 && it2 != end2) {
				unsigned short cost1 = costFunction(dynamic_cast<qc::StandardOperation *>(it1->get())->getGate(), (*it1)->getControls().size());
				unsigned short cost2 = costFunction(dynamic_cast<qc::StandardOperation *>(it2->get())->getGate(), (*it2)->getControls().size());

				for (int i = 0; i < cost2 && it1 != end1; ++i) {
					applyGate(*it1, results.result, perm1, LEFT);
					++it1;

					// apply possible swaps
					while (it1 != end1 && dynamic_cast<qc::StandardOperation *>(it1->get())->getGate() == qc::SWAP) {
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
					applyGate(*it2, results.result, perm2, RIGHT);
					++it2;

					// apply possible swaps
					while (it2 != end2 && dynamic_cast<qc::StandardOperation *>(it2->get())->getGate() == qc::SWAP) {
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
			applyGate(*it1, results.result, perm1, LEFT);
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
			applyGate(*it2, results.result, perm2, RIGHT);
			++it2;

			#if DEBUG_MODE_EC
			visited.clear();
			auto nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc-1);
			addToAverage(nc-1);
			#endif
		}

		qc::QuantumComputation::changePermutation(results.result, perm1, qc1.outputPermutation, line, dd, LEFT);
		qc::QuantumComputation::changePermutation(results.result, perm2, qc2.outputPermutation, line, dd, RIGHT);
		qc1.reduceAncillae(results.result, dd);
		qc1.reduceGarbage(results.result, dd);

		results.equivalence = equals(results.result, qc1.createInitialMatrix(dd));

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

	unsigned short IBMCostFunction(const qc::Gate& gate, unsigned short nc) {
		switch (gate) {
			case qc::I:
				return 1;

			case qc::X:
				if (nc <= 1) return 1;
				else {
					return 2 * (nc-2) * (2 * IBMCostFunction(qc::U1, 0) + 2 * IBMCostFunction(qc::U2, 0) + 3 * IBMCostFunction(qc::X, 1))
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
				if (nc == 0) return 1;
				if (nc == 1) return 2 * IBMCostFunction(qc::X, 1) + 4 * IBMCostFunction(qc::U3, 0);
				else
					return 2 * IBMCostFunction(qc::X, nc) + 4 * IBMCostFunction(qc::U3, 0); // heuristic

			case qc::U1:
			case qc::S:
			case qc::Sdag:
			case qc::T:
			case qc::Tdag:
			case qc::RZ:
				if (nc == 0) return 1;
				if (nc == 1) return 2 * IBMCostFunction(qc::X, 1) + 3 * IBMCostFunction(qc::U1, 0);
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

			case qc::P:
			case qc::Pdag:
				return IBMCostFunction(qc::X, nc) + IBMCostFunction(qc::X, nc-1);

			default:
				std::cerr << "No cost for gate" << std::endl;
				exit(1);
		}
	}
}
