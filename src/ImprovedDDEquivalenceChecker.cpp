/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <chrono>
#include <ImprovedDDEquivalenceChecker.hpp>

namespace ec {
	/// Take operation and apply it either from the left or (inverted) from the right
	/// \param op operation to apply
	/// \param to DD to apply the operation to
	/// \param dir LEFT or RIGHT
	void ImprovedDDEquivalenceChecker::applyGate(std::unique_ptr<qc::Operation>& op, dd::Edge& to, Direction dir) {
		auto saved = to;
		if (dir == LEFT) {
			to = dd->multiply(op->getDD(dd, line), to);
		} else {
			to = dd->multiply(to, op->getInverseDD(dd, line));
		}
		dd->incRef(to);
		dd->decRef(saved);
		dd->garbageCollect();
	}

	/// Use dedicated method to check the equivalence of both provided circuits
	void ImprovedDDEquivalenceChecker::check() {
		if (!validInstance())
			return;

		auto start = std::chrono::high_resolution_clock::now();
		results.result = dd->makeIdent(0, short(nqubits-1));
		dd->incRef(results.result);

		it1 = qc1->begin();
		it2 = qc2->begin();
		end1 = qc1->end();
		end2 = qc2->end();

		switch (method) {
			case Reference:
				EquivalenceChecker::check();
				return;
			case Naive:
				checkNaive();
				break;
			case Proportional:
				checkProportional();
				break;
			case Lookahead:
				checkLookahead();
				break;
		}

		// finish first circuit
		while (it1 != end1) {
			applyGate(*it1, results.result, LEFT);
			++it1;
			//performedSequence.push_back(LEFT);
		}

		//finish second circuit
		while (it2 != end2) {
			applyGate(*it2, results.result, RIGHT);
			++it2;
			//performedSequence.push_back(RIGHT);
		}

		if(dd->equals(results.result, dd->makeIdent(0, short(nqubits-1)))) {
			results.equivalence = Equivalent;
		} else {
			results.equivalence = NonEquivalent;
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();
		results.maxActive = dd->maxActive;

	}

	/// Alternate between LEFT and RIGHT applications
	void ImprovedDDEquivalenceChecker::checkNaive() {

		while (it1 != end1 && it2 != end2) {
			applyGate(*it1, results.result, LEFT);
			applyGate(*it2, results.result, RIGHT);
			++it1;
			++it2;
			//performedSequence.push_back(LEFT);
			//performedSequence.push_back(RIGHT);
		}

	}

	/// Alternate according to the gate count ratio between LEFT and RIGHT applications
	void ImprovedDDEquivalenceChecker::checkProportional() {

		unsigned int ratio = (unsigned int)
				std::round((double)std::max(qc1->getNops(), qc2->getNops()) / std::min(qc1->getNops(), qc2->getNops()));
		unsigned int ratio1 = (qc1->getNops() > qc2->getNops())? ratio: 1;
		unsigned int ratio2 = (qc1->getNops() > qc2->getNops())? 1: ratio;

		while (it1 != end1 && it2 != end2) {
			for (int i = 0; i < ratio1 && it1 != end1; ++i) {
				applyGate(*it1, results.result, LEFT);
				++it1;
				//performedSequence.push_back(LEFT);
			}
			for (int i = 0; i < ratio2 && it2 != end2; ++i) {
				applyGate(*it2, results.result, RIGHT);
				++it2;
				//performedSequence.push_back(RIGHT);
			}
		}

	}

	/// Look-ahead LEFT and RIGHT and choose the more promising option
	void ImprovedDDEquivalenceChecker::checkLookahead() {
		dd::Edge lookLeft{}, lookRight{}, left{}, right{}, saved{};
		bool cachedLeft = false, cachedRight = false;
		std::unordered_set<dd::NodePtr> visited1{};
		std::unordered_set<dd::NodePtr> visited2{};

		while (it1 != end1 && it2 != end2) {
			if(!cachedLeft) {
				left = (*it1)->getDD(dd, line);
				dd->incRef(left);
				++it1;
				cachedLeft = true;
			}

			if (!cachedRight) {
				right = (*it2)->getInverseDD(dd, line);
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
			if (nc1 <= nc2) {
				results.result = lookLeft;
				dd->decRef(left);
				cachedLeft = false;
				//performedSequence.push_back(LEFT);
			} else {
				results.result = lookRight;
				dd->decRef(right);
				cachedRight = false;
				//performedSequence.push_back(RIGHT);
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
			//performedSequence.push_back(LEFT);
		}

		if (cachedRight) {
			saved = results.result;
			results.result = dd->multiply(saved, right);
			dd->incRef(results.result);
			dd->decRef(saved);
			dd->decRef(right);
			dd->garbageCollect();
			//performedSequence.push_back(RIGHT);
		}

	}

}
