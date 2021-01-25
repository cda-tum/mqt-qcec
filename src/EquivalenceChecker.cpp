/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "EquivalenceChecker.hpp"

#include <chrono>
namespace ec {

	EquivalenceChecker::EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2) : qc1(qc1), qc2(qc2) {
		dd = std::make_unique<dd::Package>();
		dd->setMode(dd::Matrix);

		results.name1 = qc1.getName();
		results.name2 = qc2.getName();
		results.name = results.name1 + " and " + results.name2;

		results.nqubits1 = qc1.getNqubits();
		results.nqubits2 = qc2.getNqubits();

		results.ngates1 = qc1.getNops();
		results.ngates2 = qc2.getNops();

		line.fill(qc::LINE_DEFAULT);

		qc1.stripIdleQubits();
		qc2.stripIdleQubits();
		auto& larger_circuit = qc1.getNqubits() > qc2.getNqubits() ? qc1 : qc2;
		auto& smaller_circuit = qc1.getNqubits() > qc2.getNqubits() ? qc2 : qc1;

		if (std::max(qc1.getNqubits(), qc2.getNqubits()) <= qc::MAX_QUBITS) {
			// setup of ancillaries and garbage outputs
			unsigned short nqubits_to_remove = larger_circuit.getNqubits() - smaller_circuit.getNqubits();
			std::vector<std::pair<unsigned short, short>> removed{ };
			std::bitset<qc::MAX_QUBITS> garbage{ };
			for (unsigned short i = 0; i < nqubits_to_remove; ++i) {
				auto logical_qubit_index = qc::QuantumComputation::getHighestLogicalQubitIndex(larger_circuit.initialLayout);
				smaller_circuit.setLogicalQubitAncillary(logical_qubit_index);

				// store whether qubit was garbage
				garbage[logical_qubit_index] = larger_circuit.logicalQubitIsGarbage(logical_qubit_index);
				removed.push_back(larger_circuit.removeQubit(logical_qubit_index));
			}

			for (auto it = removed.rbegin(); it != removed.rend(); ++it) {
				larger_circuit.addAncillaryQubit(it->first, it->second);
				smaller_circuit.setLogicalQubitGarbage(larger_circuit.getNqubits() - 1);
				// restore garbage
				if (garbage[larger_circuit.getNqubits() - 1]) {
					larger_circuit.setLogicalQubitGarbage(larger_circuit.getNqubits() - 1);
				}
			}
		}

		initial1 = qc1.initialLayout;
		initial2 = qc2.initialLayout;
		output1 = qc1.outputPermutation;
		output2 = qc2.outputPermutation;
		ancillary1 = qc1.ancillary;
		ancillary2 = qc2.ancillary;
		garbage1 = qc1.garbage;
		garbage2 = qc2.garbage;
		nqubits = results.nqubits = qc1.getNqubitsWithoutAncillae() + std::max(qc1.getNancillae(), qc2.getNancillae());

		if (qc1.getNqubitsWithoutAncillae() != qc2.getNqubitsWithoutAncillae()) {
			std::cerr << "[QCEC] Warning: circuits have different number of primary inputs! Proceed with caution!" << std::endl;
		}

//		if (output1.size() != output2.size()) {
//			std::cerr << "[QCEC] Warning: circuits have different number of primary outputs! Proceed with caution!" << std::endl;
//		}

		if (std::max(qc1.getNqubits(), qc2.getNqubits()) <= qc::MAX_QUBITS) {
			// Try to fix potential mismatches in output permutations
//			auto& larger_initial = qc1.getNqubits() > qc2.getNqubits() ? initial1 : initial2;
			auto& smaller_initial = qc1.getNqubits() > qc2.getNqubits() ? initial2 : initial1;
			auto& larger_output = qc1.getNqubits() > qc2.getNqubits() ? output1 : output2;
			auto& smaller_output = qc1.getNqubits() > qc2.getNqubits() ? output2 : output1;
//			auto& larger_ancillary = qc1.getNqubits() > qc2.getNqubits() ? ancillary1 : ancillary2;
			auto& smaller_ancillary = qc1.getNqubits() > qc2.getNqubits() ? ancillary2 : ancillary1;
			auto& larger_garbage = qc1.getNqubits() > qc2.getNqubits() ? garbage1 : garbage2;
			auto& smaller_garbage = qc1.getNqubits() > qc2.getNqubits() ? garbage2 : garbage1;

			for (const auto& o: larger_output) {
				unsigned short output_qubit_in_larger_circuit = o.second;
//				unsigned short physical_qubit_in_larger_circuit = o.first;
//			std::cout << "Output logical qubit " << output_qubit_in_larger_circuit << " at physical qubit " << physical_qubit_in_larger_circuit;
				unsigned short nout = 1;
				for (int i = 0; i < output_qubit_in_larger_circuit; ++i) {
					if (!larger_garbage.test(i))
						++nout;
				}
//			std::cout << " which is logical output qubit number " << nout << std::endl;

				unsigned short outcount = nout;
				unsigned short output_qubit_in_smaller_circuit = 0;
				bool exists_in_smaller = false;
				for (int i = 0; i < smaller_circuit.getNqubits(); ++i) {
					if (!smaller_garbage.test(i)) {
						--outcount;
					}
					if (outcount == 0) {
						output_qubit_in_smaller_circuit = i;
						exists_in_smaller = true;
						break;
					}
				}
				// algorithm has logical qubit that does not exist in the smaller circuit
				if (!exists_in_smaller)
					continue;

//			std::cout << "This is logical qubit " << output_qubit_in_smaller_circuit << " in the smaller circuit";
				unsigned short physical_qubit_in_smaller_circuit = 0;
				for (const auto& out: smaller_output) {
					if (out.second == output_qubit_in_smaller_circuit) {
						physical_qubit_in_smaller_circuit = out.first;
						break;
					}
				}
//			std::cout << " which is assigned to physical qubit " << physical_qubit_in_smaller_circuit << " at the end of the circuit" << std::endl;

				if (output_qubit_in_larger_circuit != output_qubit_in_smaller_circuit) {
//				std::cout << "Mismatch in the logical output qubits" << std::endl;
					if (smaller_ancillary.test(output_qubit_in_larger_circuit) && smaller_garbage.test(output_qubit_in_larger_circuit)) {
						bool found = false;
						unsigned short physical_index_of_larger_in_smaller = 0;
						for (const auto& in: smaller_initial) {
							if (in.second == output_qubit_in_larger_circuit) {
								found = true;
								physical_index_of_larger_in_smaller = in.first;
								break;
							}
						}
//					if (found) {
//						std::cout << "Found logical qubit " << output_qubit_in_larger_circuit << " in smaller circuit at physical qubit " << physical_index_of_larger_in_smaller << std::endl;
//						std::cout << "This qubit is idle: " << smaller_circuit.isIdleQubit(physical_index_of_larger_in_smaller) << std::endl;
//					}
						if (!found || smaller_circuit.isIdleQubit(physical_index_of_larger_in_smaller)) {
//						std::cout << "Logical qubit " << output_qubit_in_smaller_circuit << " can be moved to logical qubit " << output_qubit_in_larger_circuit << std::endl;
							for (auto& in: smaller_initial) {
								if (in.second == output_qubit_in_smaller_circuit) {
									in.second = output_qubit_in_larger_circuit;
//								std::cout << "Physical qubit " << in.first << " has been assigned logical qubit " << in.second << " as input" << std::endl;
									break;
								}
							}
							smaller_output[physical_qubit_in_smaller_circuit] = output_qubit_in_larger_circuit;
//						std::cout << "Physical qubit " << physical_qubit_in_smaller_circuit << " has been assigned logical qubit " << output_qubit_in_larger_circuit << " as output" << std::endl;
							smaller_ancillary[output_qubit_in_larger_circuit] = smaller_ancillary.test(output_qubit_in_smaller_circuit);
							smaller_ancillary.set(output_qubit_in_smaller_circuit);
//						std::cout << "Logical qubit " << output_qubit_in_larger_circuit << " was assigned the ancillary status of qubit " << output_qubit_in_smaller_circuit << " (i.e., " << smaller_ancillary.test(output_qubit_in_larger_circuit) << ")" << std::endl;
							smaller_garbage.reset(output_qubit_in_larger_circuit);
							smaller_garbage.set(output_qubit_in_smaller_circuit);
//						std::cout << "Logical qubit " << output_qubit_in_larger_circuit << " has been removed from the garbage outputs" << std::endl;
						}
					} else {
						std::cerr << "Uncorrected mismatch in output qubits!" << std::endl;
					}
				}

			}
		}
	}

	/// Take operation and apply it either from the left or (inverted) from the right
	/// \param op operation to apply
	/// \param to DD to apply the operation to
	/// \param dir LEFT or RIGHT
	void EquivalenceChecker::applyGate(std::unique_ptr<qc::Operation>& op, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, Direction dir) {
		// set appropriate qubit count to generate correct DD
		auto nq = op->getNqubits();
		op->setNqubits(nqubits);

		auto saved = to;
		if (dir == LEFT) {
			to = dd->multiply(op->getDD(dd, line, permutation), to);
		} else {
			to = dd->multiply(to, op->getInverseDD(dd, line, permutation));
		}
		dd->incRef(to);
		dd->decRef(saved);
		dd->garbageCollect();

		// reset qubit count
		op->setNqubits(nq);
	}

	void EquivalenceChecker::applyGate(decltype(qc1.begin())& opIt, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, decltype(qc1.end())& end, Direction dir) {
		// Measurements at the end of the circuit are considered NOPs.
		if ((*opIt)->getType() == qc::Measure) {
			if (!qc::QuantumComputation::isLastOperationOnQubit(opIt, end)) {
				throw QCECException("Intermediate measurements currently not supported. Defer your measurements to the end.");
			}
			return;
		}
		applyGate(*opIt, to, permutation, dir);
	}

	void EquivalenceChecker::check(const Configuration& config) {
		results.method = Reference;

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
		dd::Edge e = dd->makeIdent(0, short(nqubits-1));
		dd->incRef(e);
		e = reduceAncillae(e, ancillary1);
		it1 = qc1.begin();
		end1 = qc1.end();

		#if DEBUG_MODE_EC
		visited.clear();
		auto nc = nodecount(e, visited);
		addToAverage(nc-1);
		std::cout << "Initial: " << nc-1 << std::endl;
		std::stringstream ss{};
		ss << toString(results.method) << "_initial_L.dot";
		dd->export2Dot(e, ss.str().c_str());
		++counter;

		std::stringstream eiss{};
		eiss << "e_initial_" << filename1 << ".dot";
		dd->export2Dot(e, eiss.str().c_str());
		#endif

		while (it1 != end1) {
			applyGate(it1, e, perm1, end1);
			++it1;
		}
		qc::QuantumComputation::changePermutation(e, perm1, output1, line, dd);
		e = reduceAncillae(e, ancillary1);
		e = reduceGarbage(e, garbage1);

		#if DEBUG_MODE_EC
		std::cout << "-------" << std::endl << std::endl;
		std::stringstream ess{};
		ess << "e_" << filename2 << ".dot";
		dd->export2Dot(e, ess.str().c_str());
		#endif

		qc::permutationMap perm2 = initial2;
		dd::Edge f = dd->makeIdent(0, short(nqubits-1));
		dd->incRef(f);
		f = reduceAncillae(f, ancillary2);
		it2 = qc2.begin();
		end2 = qc2.end();

		#if DEBUG_MODE_EC
		visited.clear();
		nc = nodecount(f, visited);
		addToAverage(nc-1);
		std::cout << "Initial: " << nc-1 << std::endl;
		ss = std::stringstream("");
		ss << toString(results.method) << "_initial_R.dot";
		dd->export2Dot(f, ss.str().c_str());
		counter = 1;

		std::stringstream fiss{};
		fiss << "f_initial_" << filename2 << ".dot";
		dd->export2Dot(f, fiss.str().c_str());
		#endif

		while (it2 != end2) {
			applyGate(it2, f, perm2, end2);
			++it2;
		}
		qc::QuantumComputation::changePermutation(f, perm2, output2, line, dd);
		f = reduceAncillae(f, ancillary2);
		f = reduceGarbage(f, garbage2);

		#if DEBUG_MODE_EC
		std::stringstream fss{};
		fss << "f_" << filename2 << ".dot";
		dd->export2Dot(f, fss.str().c_str());
		#endif

		results.maxActive = dd->maxActive;

		results.equivalence = equals(e, f);
		if (results.equivalence == NonEquivalent) {

			results.result = dd->multiply(e, dd->conjugateTranspose(f));
			dd->decRef(e);
			dd->decRef(f);
			dd->incRef(results.result);
			#if DEBUG_MODE_EC
			visited.clear();
			nc = nodecount(results.result, visited);
			maxSize = std::max(maxSize, nc);
			addToAverage(nc-1);
			std::cout << nc-1 << " NEQ " << std::endl;
			#endif
		} else {
			results.result = e;
			dd->decRef(f);
		}
		#if DEBUG_MODE_EC
		std::stringstream ss{};
		ss << "result_" << filename2 << ".dot";
		dd->export2Dot(results.result, ss.str().c_str());
		std::cout << "Max size: " << maxSize << std::endl;
		std::cout << "Avg size: " << average << std::endl;
		#endif

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();
	}

	bool EquivalenceChecker::validInstance() {
		if (qc1.getNqubits() > qc::MAX_QUBITS || qc2.getNqubits() > qc::MAX_QUBITS) {
			results.tooManyQubits = true;
			std::cerr << "Circuit contains too many qubits to be manageable by QFR library" << std::endl;
			return false;
		}

		return true;
	}

	Equivalence EquivalenceChecker::equals(dd::Edge e, dd::Edge f) {
		if (e.p != f.p) return NonEquivalent;

		if (!dd::ComplexNumbers::equals(e.w, f.w)) return EquivalentUpToGlobalPhase;

		return Equivalent;
	}

	dd::Edge EquivalenceChecker::reduceAncillae(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& ancillary, bool regular) {
		// return if no more garbage left
		if (!ancillary.any() || e.p == nullptr) return e;
		unsigned short lowerbound = 0;
		for (size_t i = 0; i < ancillary.size(); ++i) {
			if (ancillary.test(i)) {
				lowerbound = i;
				break;
			}
		}
		if (e.p->v < lowerbound) return e;
		return reduceAncillaeRecursion(e, ancillary, lowerbound, regular);
	}

	dd::Edge EquivalenceChecker::reduceAncillaeRecursion(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& ancillary, unsigned short lowerbound, bool regular) {
		if(e.p->v < lowerbound) return e;

		dd::Edge f = e;

		std::array<dd::Edge, 4> edges{ };
		std::bitset<4> handled{};
		for (int i = 0; i < 4; ++i) {
			if (!handled.test(i)) {
				if (dd->isTerminal(e.p->e[i])) {
					edges[i] = e.p->e[i];
				} else {
					edges[i] = reduceAncillaeRecursion(f.p->e[i], ancillary, lowerbound, regular);
					for (int j = i+1; j < 4; ++j) {
						if (e.p->e[i].p == e.p->e[j].p) {
							edges[j] = edges[i];
							handled.set(j);
						}
					}
				}
				handled.set(i);
			}
		}
		f = dd->makeNonterminal(f.p->v, edges);

		// something to reduce for this qubit
		if (f.p->v >= 0 && ancillary.test(f.p->v)) {
			if (regular) {
				if (f.p->e[1].w != CN::ZERO || f.p->e[3].w != CN::ZERO){
					f = dd->makeNonterminal(f.p->v, { f.p->e[0], dd::Package::DDzero, f.p->e[2], dd::Package::DDzero });
				}
			} else {
				if (f.p->e[2].w != CN::ZERO || f.p->e[3].w != CN::ZERO) {
					f = dd->makeNonterminal(f.p->v, { f.p->e[0], f.p->e[1], dd::Package::DDzero, dd::Package::DDzero });
				}
			}
		}

		auto c = dd->cn.mulCached(f.w, e.w);
		f.w = dd->cn.lookup(c);
		dd->cn.releaseCached(c);
		dd->incRef(f);
		return f;
	}

	dd::Edge EquivalenceChecker::reduceGarbage(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& garbage, bool regular) {
		// return if no more garbage left
		if (!garbage.any() || e.p == nullptr) return e;
		unsigned short lowerbound = 0;
		for (size_t i=0; i<garbage.size(); ++i) {
			if (garbage.test(i)) {
				lowerbound = i;
				break;
			}
		}
		if(e.p->v < lowerbound) return e;
		return reduceGarbageRecursion(e, garbage, lowerbound, regular);
	}

	dd::Edge EquivalenceChecker::reduceGarbageRecursion(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& garbage, unsigned short lowerbound, bool regular) {
		if(e.p->v < lowerbound) return e;

		dd::Edge f = e;

		std::array<dd::Edge, 4> edges{ };
		std::bitset<4> handled{};
		for (int i = 0; i < 4; ++i) {
			if (!handled.test(i)) {
				if (dd->isTerminal(e.p->e[i])) {
					edges[i] = e.p->e[i];
				} else {
					edges[i] = reduceGarbageRecursion(f.p->e[i], garbage, lowerbound, regular);
					for (int j = i+1; j < 4; ++j) {
						if (e.p->e[i].p == e.p->e[j].p) {
							edges[j] = edges[i];
							handled.set(j);
						}
					}
				}
				handled.set(i);
			}
		}
		f = dd->makeNonterminal(f.p->v, edges);

		// something to reduce for this qubit
		if (f.p->v >= 0 && garbage.test(f.p->v)) {
			if (regular) {
				if (f.p->e[2].w != CN::ZERO || f.p->e[3].w != CN::ZERO) {
					dd::Edge g{ };
					if (f.p->e[0].w == CN::ZERO && f.p->e[2].w != CN::ZERO) {
						g = f.p->e[2];
					} else if (f.p->e[2].w != CN::ZERO) {
						g = dd->add(f.p->e[0], f.p->e[2]);
					} else {
						g = f.p->e[0];
					}
					dd::Edge h{ };
					if (f.p->e[1].w == CN::ZERO && f.p->e[3].w != CN::ZERO) {
						h = f.p->e[3];
					} else if (f.p->e[3].w != CN::ZERO) {
						h = dd->add(f.p->e[1], f.p->e[3]);
					} else {
						h = f.p->e[1];
					}
					f = dd->makeNonterminal(e.p->v, { g, h, dd::Package::DDzero, dd::Package::DDzero });
				}
			} else {
				if (f.p->e[1].w != CN::ZERO || f.p->e[3].w != CN::ZERO) {
					dd::Edge g{ };
					if (f.p->e[0].w == CN::ZERO && f.p->e[1].w != CN::ZERO) {
						g = f.p->e[1];
					} else if (f.p->e[1].w != CN::ZERO) {
						g = dd->add(f.p->e[0], f.p->e[1]);
					} else {
						g = f.p->e[0];
					}
					dd::Edge h{ };
					if (f.p->e[2].w == CN::ZERO && f.p->e[3].w != CN::ZERO) {
						h = f.p->e[3];
					} else if (f.p->e[3].w != CN::ZERO) {
						h = dd->add(f.p->e[2], f.p->e[3]);
					} else {
						h = f.p->e[2];
					}
					f = dd->makeNonterminal(e.p->v, { g, dd::Package::DDzero, h, dd::Package::DDzero });
				}
			}
		}

		auto c = dd->cn.mulCached(f.w, e.w);
		f.w = dd->cn.lookup(c);
		dd->cn.releaseCached(c);
		// Quick-fix for normalization bug
		if (CN::mag2(f.w) > 1.0)
			f.w = CN::ONE;
		dd->incRef(f);
		return f;
	}

}
