/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "EquivalenceChecker.hpp"

#include <chrono>
namespace ec {

	EquivalenceChecker::EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2) : qc1(qc1), qc2(qc2) {
		dd = std::make_unique<dd::Package>();
		dd->setMode(dd::Matrix);

		filename1 = results.name1 = qc1.getName();
		filename2 = results.name2 = qc2.getName();
		results.name = filename1 + " and " + filename2;

		nqubits1 = results.nqubits1 = qc1.getNqubits();
		nqubits2 = results.nqubits2 = qc2.getNqubits();

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

	void EquivalenceChecker::check(const Configuration& config) {
		results.method = Reference;

		if (!validInstance())
			return;

		auto start = std::chrono::high_resolution_clock::now();

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

		for (auto& op : qc1) {
			if (!op->isUnitary()) {
				std::cerr << "Functionality not unitary." << std::endl;
				exit(1);
			}

			#if DEBUG_MODE_EC
			std::cout << "before: " << std::endl;
			qc::QuantumComputation::printPermutationMap(perm1);
			#endif
			// set appropriate qubit count to generate correct DD
			auto nq = op->getNqubits();
			op->setNqubits(nqubits);

			auto tmp = dd->multiply(op->getDD(dd, line, perm1), e);

			#if DEBUG_MODE_EC
			std::cout << "after: " << std::endl;
			qc::QuantumComputation::printPermutationMap(perm1);
			#endif

			dd->incRef(tmp);
			dd->decRef(e);
			e = tmp;

			#if DEBUG_MODE_EC
			visited.clear();
			nc = nodecount(e, visited);
			maxSize = std::max(maxSize, nc);
			addToAverage(nc-1);
			std::cout << nc-1 << " L " << std::endl;
			std::stringstream ss{};
			ss << toString(results.method) << "_" << counter << "_L.dot";
			dd->export2Dot(e, ss.str().c_str());
			++counter;
			#endif
			dd->garbageCollect();

			// reset qubit count
			op->setNqubits(nq);
		}
		qc::QuantumComputation::changePermutation(e, perm1, output1, line, dd);
		reduceAncillae(e, ancillary1);
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

		for (auto& op : qc2) {
			if (!op->isUnitary()) {
				std::cerr << "Functionality not unitary." << std::endl;
				exit(1);
			}

			#if DEBUG_MODE_EC
			std::cout << "before: " << std::endl;
			qc::QuantumComputation::printPermutationMap(perm2);
			#endif
			// set appropriate qubit count to generate correct DD
			auto nq = op->getNqubits();
			op->setNqubits(nqubits);

			auto tmp = dd->multiply(op->getDD(dd, line, perm2), f);

			#if DEBUG_MODE_EC
			std::cout << "after: " << std::endl;
			qc::QuantumComputation::printPermutationMap(perm2);
			std::cout << "-------" << std::endl;
			#endif

			dd->incRef(tmp);
			dd->decRef(f);
			f = tmp;

			#if DEBUG_MODE_EC
			visited.clear();
			nc = nodecount(f, visited);
			maxSize = std::max(maxSize, nc);
			addToAverage(nc-1);
			std::cout << nc-1 << " R " << std::endl;
			std::stringstream ss{};
			ss << toString(results.method) << "_" << counter << "_R.dot";
			dd->export2Dot(f, ss.str().c_str());
			++counter;
			#endif
			dd->garbageCollect();

			// reset qubit count
			op->setNqubits(nq);
		}
		qc::QuantumComputation::changePermutation(f, perm2, output2, line, dd);
		reduceAncillae(f, ancillary2);
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

	void EquivalenceChecker::augmentQubits(qc::QuantumComputation& circuit_to_augment, qc::QuantumComputation& circuit_to_match) {
		// handle the case when the circuits operate on different amounts of qubits (typically ancillaries or device qubits)
		// it is assumed that these additional logical qubits are appended to the original list of qubits, i.e.
		// if the smaller circuit has qubits
		//      q0, q1, ..., qn,
		// the larger circuit follows the structure
		//      q0, q1, ..., qn, anc0, anc1, ..., ancm
		// note this holds for LOGICAL qubits and not for the physical assignments.
		if (circuit_to_augment.getNqubits() > circuit_to_match.getNqubits()) {
			// switch circuits if wrong order was passed
			augmentQubits(circuit_to_match, circuit_to_augment);
			return;
		} else if (circuit_to_augment.getNqubits() == circuit_to_match.getNqubits()) {
			// if both circuits operate on the same number of qubits nothing has to be done
			return;
		}

		#if DEBUG_MODE_EC
		std::cout << "Add additional qubits to circuit that shall be augmented" << std::endl;
		#endif
		unsigned short nancillae_to_augment = circuit_to_match.getNqubits() - circuit_to_augment.getNqubits();
		unsigned short physical_qubit_index = 0;
		for (int i = 0; i < nancillae_to_augment; ++i) {
			// find first unoccupied physical qubit
			while (circuit_to_augment.initialLayout.count(physical_qubit_index)) {
				physical_qubit_index++;
			}
			#if DEBUG_MODE_EC
			std::cout << "Found unoccupied physical qubit: " << physical_qubit_index << std::endl;
			#endif
			circuit_to_augment.addAncillaryQubit(physical_qubit_index, -1);
			// these added qubits are also garbage outputs
			circuit_to_augment.setLogicalQubitGarbage(circuit_to_augment.getNqubits() - 1);
			physical_qubit_index++;
		}
		#if DEBUG_MODE_EC
		std::cout << "Circuit to augment: ";
		circuit_to_augment->printRegisters();
		circuit_to_augment->print();
		std::cout << "Remove qubits from back of circuit that shall be matched" << std::endl;
		std::cout << "Circuit to match: ";
		circuit_to_match->printRegisters();
		circuit_to_match->print();
		#endif
		unsigned short nqubits_to_remove = circuit_to_match.getNqubits() - circuit_to_augment.getNqubitsWithoutAncillae();
		std::vector<std::pair<unsigned short, short>> removed{ };
		std::bitset<qc::MAX_QUBITS> garbage{ };
		for (unsigned short i = 0; i < nqubits_to_remove; ++i) {
			auto logical_qubit_index = qc::QuantumComputation::getHighestLogicalQubitIndex(circuit_to_match.initialLayout);
			if (circuit_to_match.logicalQubitIsAncillary(logical_qubit_index)) continue;

			// store wether qubit was garbage
			garbage[logical_qubit_index] = circuit_to_match.logicalQubitIsGarbage(logical_qubit_index);
			removed.push_back(circuit_to_match.removeQubit(logical_qubit_index));
		}
		#if DEBUG_MODE_EC
		std::cout << "Circuit to match: ";
		circuit_to_match->printRegisters();
		circuit_to_match->print();
		std::cout << "Add the removed qubits ";
		for(const auto& q:removed)
			std::cout << q.first << " ";
		std::cout << "back as ancillae" << std::endl;
		#endif

		for (auto it = removed.rbegin(); it != removed.rend(); ++it) {
			circuit_to_match.addAncillaryQubit(it->first, it->second);
			// restore garbage
			if (garbage[circuit_to_match.getNqubits() - 1])
				circuit_to_match.setLogicalQubitGarbage(circuit_to_match.getNqubits() - 1);
		}

		#if DEBUG_MODE_EC
		std::cout << "Resulting in: " << std::endl;
		std::cout << "Circuit to match: ";
		circuit_to_match->printRegisters();
		circuit_to_match->print();
		#endif

	}

	Equivalence EquivalenceChecker::equals(dd::Edge e, dd::Edge f) {
		if (e.p != f.p) return NonEquivalent;

		if (!dd::ComplexNumbers::equals(e.w, f.w)) return EquivalentUpToGlobalPhase;

		return Equivalent;
	}

	dd::Edge EquivalenceChecker::reduceAncillae(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& ancillary, bool regular) {
		// return if no more ancillaries left
		if (!ancillary.any() || e.p == nullptr) return e;
		unsigned short firstAncillary = 0;
		for (auto i=0; i<ancillary.size(); ++i) {
			if (ancillary.test(i)) {
				firstAncillary = i;
				break;
			}
		}
		if(e.p->v < firstAncillary) return e;

		dd::Edge f = e;

		std::array<dd::Edge, 4> edges{ };
		for (int i = 0; i < 4; ++i) {
			edges[i] = reduceAncillae(f.p->e[i], ancillary, regular);
		}
		f = dd->makeNonterminal(f.p->v, edges);

		// something to reduce for this qubit
		if (ancillary.test(f.p->v)) {
			if ((regular && (!CN::equalsZero(f.p->e[1].w) || !CN::equalsZero(f.p->e[3].w))) ||
			    (!regular && (!CN::equalsZero(f.p->e[2].w) || !CN::equalsZero(f.p->e[3].w)))) {
				if (regular) {
					f = dd->makeNonterminal(f.p->v, { f.p->e[0], dd::Package::DDzero, f.p->e[2], dd::Package::DDzero });
				} else {
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
		unsigned short firstGarbage = 0;
		for (auto i=0; i<garbage.size(); ++i) {
			if (garbage.test(i)) {
				firstGarbage = i;
				break;
			}
		}
		if(e.p->v < firstGarbage) return e;

		dd::Edge f = e;

		std::array<dd::Edge, 4> edges{ };
		for (int i = 0; i < 4; ++i) {
			edges[i] = reduceGarbage(f.p->e[i], garbage, regular);
		}
		f = dd->makeNonterminal(f.p->v, edges);

		// something to reduce for this qubit
		if (garbage.test(f.p->v)) {
			if ((regular && (!CN::equalsZero(f.p->e[2].w) || !CN::equalsZero(f.p->e[3].w))) ||
			    (!regular && (!CN::equalsZero(f.p->e[1].w) || !CN::equalsZero(f.p->e[3].w)))) {

				dd::Edge g{ };
				if (regular) {
					if (CN::equalsZero(f.p->e[0].w) && !CN::equalsZero(f.p->e[2].w)) {
						g = f.p->e[2];
					} else if (!CN::equalsZero(f.p->e[2].w)) {
						g = dd->add(f.p->e[0], f.p->e[2]);
					} else {
						g = f.p->e[0];
					}
				} else {
					if (CN::equalsZero(f.p->e[0].w) && !CN::equalsZero(f.p->e[1].w)) {
						g = f.p->e[1];
					} else if (!CN::equalsZero(f.p->e[1].w)) {
						g = dd->add(f.p->e[0], f.p->e[1]);
					} else {
						g = f.p->e[0];
					}
				}

				dd::Edge h{ };
				if (regular) {
					if (CN::equalsZero(f.p->e[1].w) && !CN::equalsZero(f.p->e[3].w)) {
						h = f.p->e[3];
					} else if (!CN::equalsZero(f.p->e[3].w)) {
						h = dd->add(f.p->e[1], f.p->e[3]);
					} else {
						h = f.p->e[1];
					}
				} else {
					if (CN::equalsZero(f.p->e[2].w) && !CN::equalsZero(f.p->e[3].w)) {
						h = f.p->e[3];
					} else if (!CN::equalsZero(f.p->e[3].w)) {
						h = dd->add(f.p->e[2], f.p->e[3]);
					} else {
						h = f.p->e[2];
					}
				}

				if (regular) {
					f = dd->makeNonterminal(e.p->v, { g, h, dd::Package::DDzero, dd::Package::DDzero });
				} else {
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
