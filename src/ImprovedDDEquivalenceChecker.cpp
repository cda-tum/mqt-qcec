/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <ImprovedDDEquivalenceChecker.hpp>

namespace ec {
	/// Take operation and apply it either from the left or (inverted) from the right
	/// \param op operation to apply
	/// \param to DD to apply the operation to
	/// \param dir LEFT or RIGHT
	void ImprovedDDEquivalenceChecker::applyGate(std::unique_ptr<qc::Operation>& op, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, Direction dir) {
		#if DEBUG_MODE_EC
		std::cout << "before: " << std::endl;
		qc::QuantumComputation::printPermutationMap(permutation);
		#endif

		auto saved = to;
		if (dir == LEFT) {
			to = dd->multiply(op->getDD(dd, line, permutation), to);
		} else {
			to = dd->multiply(to, op->getInverseDD(dd, line, permutation));
		}
		dd->incRef(to);
		dd->decRef(saved);
		dd->garbageCollect();

		#if DEBUG_MODE_EC
		std::cout << "after: " << std::endl;
		qc::QuantumComputation::printPermutationMap(permutation);
		std::cout << "-------" << std::endl;
		#endif
	}

	/// Use dedicated method to check the equivalence of both provided circuits
	void ImprovedDDEquivalenceChecker::check(const Configuration& config) {
		if (method == Reference) {
			EquivalenceChecker::check(config);
			return;
		}

		if (!validInstance())
			return;

		auto start = std::chrono::high_resolution_clock::now();

		if (config.augmentQubitRegisters) {
			augmentQubits(qc1, qc2);
		} else {
			if (qc1->getNqubits() != qc2->getNqubits()) {
				std::cerr << "Circuits operate on different number of qubits and 'augmentQubitRegisters' is not enabled" << std::endl;
				exit(1);
			}
		}

		#if DEBUG_MODE_EC
		std::cout << "QC1: ";
		qc1->printRegisters();
		qc1->print();
		std::cout << "QC2: ";
		qc2->printRegisters();
		qc2->print();
		#endif

		qc::permutationMap perm1 = qc1->initialLayout;
		qc::permutationMap perm2 = qc2->initialLayout;
		results.result = qc1->createInitialMatrix(dd);
		dd->incRef(results.result);

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

		it1 = qc1->begin();
		it2 = qc2->begin();
		end1 = qc1->end();
		end2 = qc2->end();

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
			applyGate(*it1, results.result, perm1, LEFT);
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
			applyGate(*it2, results.result, perm2, RIGHT);
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

		qc::QuantumComputation::changePermutation(results.result, perm1, qc1->outputPermutation, line, dd, LEFT);
		qc::QuantumComputation::changePermutation(results.result, perm2, qc2->outputPermutation, line, dd, RIGHT);
		qc1->reduceAncillae(results.result, dd);
		qc1->reduceGarbage(results.result, dd);

		results.equivalence = equals(results.result, qc1->createInitialMatrix(dd));

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
			applyGate(*it1, results.result, perm1, LEFT);

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

			applyGate(*it2, results.result, perm2, RIGHT);

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
				std::round((double)std::max(qc1->getNops(), qc2->getNops()) / std::min(qc1->getNops(), qc2->getNops()));
		unsigned int ratio1 = (qc1->getNops() > qc2->getNops())? ratio: 1;
		unsigned int ratio2 = (qc1->getNops() > qc2->getNops())? 1: ratio;

		while (it1 != end1 && it2 != end2) {
			for (unsigned int i = 0; i < ratio1 && it1 != end1; ++i) {
				applyGate(*it1, results.result, perm1, LEFT);
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
				applyGate(*it2, results.result, perm2, RIGHT);
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
				left = (*it1)->getDD(dd, line, perm1);
				dd->incRef(left);
				++it1;
				cachedLeft = true;
			}

			if (!cachedRight) {
				right = (*it2)->getInverseDD(dd, line, perm2);
				dd->incRef(right);
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
