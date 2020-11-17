/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include <ImprovedDDEquivalenceChecker.hpp>

namespace ec {
	dd::Edge ImprovedDDEquivalenceChecker::createInitialMatrix() {
		dd::Edge e = dd->makeIdent(0, short(nqubits-1));
		dd->incRef(e);

		std::bitset<qc::MAX_QUBITS> ancillary{};
		for (int q=nqubits-1; q>=0; --q) {
			if (qc1.logicalQubitIsAncillary(q) && qc2.logicalQubitIsAncillary(q)) {
				bool found1 = false;
				bool isidle1 = false;
				for (const auto& in1: initial1) {
					if (in1.second == q) {
						found1 = true;
						isidle1 = qc1.isIdleQubit(in1.first);
						break;
					}
				}
				bool found2 = false;
				bool isidle2 = false;
				for (const auto& in2: initial2) {
					if (in2.second == q) {
						found2 = true;
						isidle2 = qc2.isIdleQubit(in2.first);
						break;
					}
				}

				// qubit only really exists or is acted on in one of the circuits
				if ((found1 ^ found2) || (isidle1 ^ isidle2)) {
					ancillary.set(q);
				}
			}
		}
		e = reduceAncillae(e, ancillary);
		return e;
	}

	dd::Edge ImprovedDDEquivalenceChecker::createGoalMatrix() {
		auto goalMatrix = dd->makeIdent(0, short(nqubits - 1));
		dd->incRef(goalMatrix);
		goalMatrix = reduceAncillae(goalMatrix, ancillary2, RIGHT);
		goalMatrix = reduceGarbage(goalMatrix, garbage2, RIGHT);
		goalMatrix = reduceAncillae(goalMatrix, ancillary1, LEFT);
		goalMatrix = reduceGarbage(goalMatrix, garbage1, LEFT);
		return goalMatrix;
	}

	/// Use dedicated method to check the equivalence of both provided circuits
	void ImprovedDDEquivalenceChecker::check(const Configuration& config) {
		if (method == Reference) {
			EquivalenceChecker::check(config);
			return;
		}

		if (!validInstance())
			return;

		setTolerance(config.tolerance);

		auto start = std::chrono::high_resolution_clock::now();

		if (config.swapGateFusion) {
			qc::CircuitOptimizer::swapGateFusion(qc1);
			qc::CircuitOptimizer::swapGateFusion(qc2);
		}

		if (config.singleQubitGateFusion) {
			qc::CircuitOptimizer::singleQubitGateFusion(qc1);
			qc::CircuitOptimizer::singleQubitGateFusion(qc2);
		}

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
		maxSize = std::max(maxSize, nc);
		addToAverage(nc-1);
		std::cout << "Initial: " << nc-1 << std::endl;
		std::stringstream ss{};
		ss << toString(results.method) << "_Initial.dot";
		dd->export2Dot(results.result, ss.str().c_str());
		++counter;
		#endif

		it1 = qc1.begin();
		it2 = qc2.begin();
		end1 = qc1.end();
		end2 = qc2.end();

		switch (method) {
			case Naive: checkNaive(perm1, perm2);
				break;
			case Proportional: checkProportional(perm1, perm2);
				break;
			case Lookahead: checkLookahead(perm1, perm2);
				break;
			default:
				std::cerr << "Method " << toString(method) << " not supported by ImprovedDDEquivalenceChecker" << std::endl;
				exit(1);
		}

		// finish first circuit
		while (it1 != end1) {
			applyGate(it1, results.result, perm1, end1, LEFT);
			++it1;

			#if DEBUG_MODE_EC
				visited.clear();
				nc = nodecount(results.result, visited);
				maxSize = std::max(maxSize, nc);
				addToAverage(nc-1);
				std::cout << nc-1 << " L " << std::endl;
				std::stringstream ss{};
				ss << toString(results.method) << "_" << counter << "_L.dot";
				dd->export2Dot(results.result, ss.str().c_str());
				++counter;
			#endif
		}

		//finish second circuit
		while (it2 != end2) {
			applyGate(it2, results.result, perm2, end2, RIGHT);
			++it2;

			#if DEBUG_MODE_EC
				visited.clear();
				nc = nodecount(results.result, visited);
				maxSize = std::max(maxSize, nc);
				addToAverage(nc-1);
				std::cout << nc-1 << " R " << std::endl;
				std::stringstream ss{};
				ss << toString(results.method) << "_" << counter << "_R.dot";
				dd->export2Dot(results.result, ss.str().c_str());
				++counter;
			#endif
		}

		qc::QuantumComputation::changePermutation(results.result, perm1, output1, line, dd, LEFT);
		qc::QuantumComputation::changePermutation(results.result, perm2, output2, line, dd, RIGHT);
		results.result = reduceGarbage(results.result, garbage1, LEFT);
		results.result = reduceGarbage(results.result, garbage2, RIGHT);
		results.result = reduceAncillae(results.result, ancillary1, LEFT);
		results.result = reduceAncillae(results.result, ancillary2, RIGHT);

		results.equivalence = equals(results.result, createGoalMatrix());

		#if DEBUG_MODE_EC
		std::stringstream ss{};
		ss << "result_improved_" << filename2 << ".dot";
		dd->export2Dot(results.result, ss.str().c_str());
		std::cout << "Avg size: " << average << std::endl;
		std::cout << "Max size: " << maxSize << std::endl;
		#endif

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();
		results.maxActive = dd->maxActive;
	}

	/// Alternate between LEFT and RIGHT applications
	void ImprovedDDEquivalenceChecker::checkNaive(qc::permutationMap& perm1, qc::permutationMap& perm2) {

		while (it1 != end1 && it2 != end2) {
			applyGate(it1, results.result, perm1, end1, LEFT);

			#if DEBUG_MODE_EC
			visited.clear();
			auto nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc);
			addToAverage(nc-1);
			std::cout << nc-1 << " L " << std::endl;
			std::stringstream ss{};
			ss << toString(results.method) << "_" << counter << "_L.dot";
			dd->export2Dot(results.result, ss.str().c_str());
			++counter;
			#endif

			applyGate(it2, results.result, perm2, end2, RIGHT);

			#if DEBUG_MODE_EC
			visited.clear();
			nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc);
			addToAverage(nc-1);
			std::cout << nc-1 << " R " << std::endl;
			ss = std::stringstream("");
			ss << toString(results.method) << "_" << counter << "_R.dot";
			dd->export2Dot(results.result, ss.str().c_str());
			++counter;
			#endif

			++it1;
			++it2;
		}

	}

	/// Alternate according to the gate count ratio between LEFT and RIGHT applications
	void ImprovedDDEquivalenceChecker::checkProportional(qc::permutationMap& perm1, qc::permutationMap& perm2) {

		unsigned int ratio = (unsigned int)
				std::round((double)std::max(qc1.getNops(), qc2.getNops()) / std::min(qc1.getNops(), qc2.getNops()));
		unsigned int ratio1 = (qc1.getNops() > qc2.getNops())? ratio: 1;
		unsigned int ratio2 = (qc1.getNops() > qc2.getNops())? 1: ratio;

		while (it1 != end1 && it2 != end2) {
			for (unsigned int i = 0; i < ratio1 && it1 != end1; ++i) {
				applyGate(it1, results.result, perm1, end1, LEFT);
				++it1;

				#if DEBUG_MODE_EC
				visited.clear();
				auto nc = nodecount(results.result, visited);
				maxSize = std::max(maxSize, nc);
				addToAverage(nc-1);
				std::cout << nc-1 << " L " << std::endl;
				std::stringstream ss{};
				ss << toString(results.method) << "_" << counter << "_L.dot";
				dd->export2Dot(results.result, ss.str().c_str());
				++counter;
				#endif
			}
			for (unsigned int i = 0; i < ratio2 && it2 != end2; ++i) {
				applyGate(it2, results.result, perm2, end2, RIGHT);
				++it2;

				#if DEBUG_MODE_EC
				visited.clear();
				auto nc = nodecount(results.result, visited);
				maxSize = std::max(maxSize, nc);
				addToAverage(nc-1);
				std::cout << nc-1 << " R " << std::endl;
				std::stringstream ss{};
				ss << toString(results.method) << "_" << counter << "_R.dot";
				dd->export2Dot(results.result, ss.str().c_str());
				++counter;
				#endif
			}
		}
	}

	/// Look-ahead LEFT and RIGHT and choose the more promising option
	void ImprovedDDEquivalenceChecker::checkLookahead(qc::permutationMap& perm1, qc::permutationMap& perm2) {
		dd::Edge lookLeft{}, lookRight{}, left{}, right{}, saved{};
		bool cachedLeft = false, cachedRight = false;
		std::unordered_set<dd::NodePtr> visited1{};
		std::unordered_set<dd::NodePtr> visited2{};

		while (it1 != end1 && it2 != end2) {
			if(!cachedLeft) {
				// stop if measurement is encountered
				if ((*it1)->getType() == qc::Measure)
					break;

				auto nq = (*it1)->getNqubits();
				(*it1)->setNqubits(nqubits);
				left = (*it1)->getDD(dd, line, perm1);
				dd->incRef(left);
				(*it1)->setNqubits(nq);
				++it1;
				cachedLeft = true;
			}

			if (!cachedRight) {
				// stop if measurement is encountered
				if ((*it2)->getType() == qc::Measure)
					break;

				auto nq = (*it2)->getNqubits();
				(*it2)->setNqubits(nqubits);
				right = (*it2)->getInverseDD(dd, line, perm2);
				dd->incRef(right);
				(*it2)->setNqubits(nq);
				++it2;
				cachedRight = true;
			}

			saved = results.result;
			lookLeft = dd->multiply(left, saved);
			lookRight = dd->multiply(saved, right);

			visited1.clear();
			visited2.clear();
			auto nc1 = nodecount(lookLeft, visited1);
			auto nc2 = nodecount(lookRight, visited2);

			#if DEBUG_MODE_EC
			std::cout << "(" << nc1 << "/" << nc2 << ") ";
			#endif

			if (nc1 <= nc2) {
				results.result = lookLeft;
				dd->decRef(left);
				cachedLeft = false;

				#if DEBUG_MODE_EC
				maxSize = std::max(maxSize, nc1);
				addToAverage(nc1-1);
				std::cout << "L " << std::endl;
				std::stringstream ss{};
				ss << toString(results.method) << "_" << counter << "_L.dot";
				dd->export2Dot(results.result, ss.str().c_str());
				++counter;
				#endif
			} else {
				results.result = lookRight;
				dd->decRef(right);
				cachedRight = false;

				#if DEBUG_MODE_EC
				maxSize = std::max(maxSize, nc2);
				addToAverage(nc2-1);
				std::cout << "R " << std::endl;
				std::stringstream ss{};
				ss << toString(results.method) << "_" << counter << "_R.dot";
				dd->export2Dot(results.result, ss.str().c_str());
				++counter;
				#endif
			}
			dd->incRef(results.result);
			dd->decRef(saved);
			dd->garbageCollect();
		}

		if (cachedLeft) {
			saved = results.result;
			results.result = dd->multiply(left, saved);
			dd->incRef(results.result);
			dd->decRef(saved);
			dd->decRef(left);
			dd->garbageCollect();

			#if DEBUG_MODE_EC
			visited.clear();
			auto nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc);
			addToAverage(nc-1);
			std::cout << nc-1 << " L " << std::endl;
			std::stringstream ss{};
			ss << toString(results.method) << "_" << counter << "_L.dot";
			dd->export2Dot(results.result, ss.str().c_str());
			++counter;
			#endif
		}

		if (cachedRight) {
			saved = results.result;
			results.result = dd->multiply(saved, right);
			dd->incRef(results.result);
			dd->decRef(saved);
			dd->decRef(right);
			dd->garbageCollect();

			#if DEBUG_MODE_EC
			visited.clear();
			auto nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc);
			addToAverage(nc-1);
			std::cout << nc-1 << " R " << std::endl;
			std::stringstream ss{};
			ss << toString(results.method) << "_" << counter << "_R.dot";
			dd->export2Dot(results.result, ss.str().c_str());
			++counter;
			#endif
		}

	}

}
