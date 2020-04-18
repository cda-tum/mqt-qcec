/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "EquivalenceChecker.hpp"

#include <chrono>

void ec::EquivalenceChecker::check(const Configuration& config) {
	results.method = Reference;

	if (!validInstance())
		return;

	auto start = std::chrono::high_resolution_clock::now();

	if (config.augmentQubitRegisters) {
		augmentQubits(qc1, qc2);
	} else {
		if (qc1.getNqubits() != qc2.getNqubits()) {
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

	std::array<short, qc::MAX_QUBITS> line{};
	line.fill(qc::LINE_DEFAULT);

	qc::permutationMap perm1 = qc1.initialLayout;
	dd::Edge e = qc1.createInitialMatrix(dd);
	dd->incRef(e);

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

	for (auto & op : qc1) {
		if (!op->isUnitary()) {
			std::cerr << "Functionality not unitary." << std::endl;
			exit(1);
		}

		#if DEBUG_MODE_EC
		std::cout << "before: " << std::endl;
		qc::QuantumComputation::printPermutationMap(perm1);
		#endif

		auto tmp = dd->multiply(op->getDD(dd, line,perm1), e);

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
	}
	qc::QuantumComputation::changePermutation(e, perm1, qc1.outputPermutation, line, dd);
	qc1.reduceAncillae(e, dd);
	qc1.reduceGarbage(e, dd);

	#if DEBUG_MODE_EC
	std::cout << "-------" << std::endl << std::endl;
	std::stringstream ess{};
	ess << "e_" << filename2 << ".dot";
	dd->export2Dot(e, ess.str().c_str());
	#endif

	qc::permutationMap perm2 = qc2.initialLayout;
	dd::Edge f = qc2.createInitialMatrix(dd);
	dd->incRef(f);

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

	for (auto & op : qc2) {
		if (!op->isUnitary()) {
			std::cerr << "Functionality not unitary." << std::endl;
			exit(1);
		}

		#if DEBUG_MODE_EC
		std::cout << "before: " << std::endl;
		qc::QuantumComputation::printPermutationMap(perm2);
		#endif

		auto tmp = dd->multiply(op->getDD(dd, line,perm2), f);

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
	}
	qc::QuantumComputation::changePermutation(f, perm2, qc2.outputPermutation, line, dd);
	qc2.reduceAncillae(f, dd);
	qc2.reduceGarbage(f, dd);

	#if DEBUG_MODE_EC
	std::stringstream fss{};
	fss << "f_" << filename2 << ".dot";
	dd->export2Dot(f, fss.str().c_str());
	#endif

	results.maxActive = dd->maxActive;

	results.equivalence = equals(e,f);
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

bool ec::EquivalenceChecker::validInstance() {
	if (qc1.getNqubits() > qc::MAX_QUBITS || qc2.getNqubits() > qc::MAX_QUBITS) {
		results.tooManyQubits = true;
		std::cerr << "Circuit contains too many qubits to be manageable by QFR library" << std::endl;
		return false;
	}

	return true;
}


void ec::EquivalenceChecker::augmentQubits(qc::QuantumComputation& circuit_to_augment, qc::QuantumComputation& circuit_to_match) {
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
	for (int i=0; i<nancillae_to_augment; ++i) {
		// find first unoccupied physical qubit
		while (circuit_to_augment.initialLayout.count(physical_qubit_index)) {
			physical_qubit_index++;
		}
		#if DEBUG_MODE_EC
		std::cout << "Found unoccupied physical qubit: " << physical_qubit_index << std::endl;
		#endif
		circuit_to_augment.addAncillaryQubit(physical_qubit_index, -1);
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
	unsigned short nqubits_to_remove = circuit_to_match.getNqubits()-circuit_to_augment.getNqubitsWithoutAncillae();
	std::vector<std::pair<unsigned short, short>> removed{};
	for (unsigned short i=0; i < nqubits_to_remove; ++i) {
		auto logical_qubit_index = circuit_to_match.getHighestLogicalQubitIndex();
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

	for (auto it=removed.rbegin(); it != removed.rend(); ++it) {
		circuit_to_match.addAncillaryQubit(it->first, it->second);
	}

	#if DEBUG_MODE_EC
	std::cout << "Resulting in: " << std::endl;
	std::cout << "Circuit to match: ";
	circuit_to_match->printRegisters();
	circuit_to_match->print();
	#endif

}

ec::EquivalenceChecker::EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2): qc1(qc1), qc2(qc2) {
	dd = std::make_unique<dd::Package>();
	dd->useMatrixNormalization(true);

	filename1 = results.name1 = qc1.getName();
	filename2 = results.name2 = qc2.getName();
	results.name = filename1 + " and " + filename2;

	nqubits1 = results.nqubits1 = qc1.getNqubits();
	nqubits2 = results.nqubits2 = qc2.getNqubits();
	nqubits = results.nqubits = std::max(nqubits1, nqubits2);

	results.ngates1 = qc1.getNops();
	results.ngates2 = qc2.getNops();
}

ec::Equivalence ec::EquivalenceChecker::equals(dd::Edge e, dd::Edge f) {
	if (e.p != f.p) return NonEquivalent;

	if(!dd::ComplexNumbers::equals(e.w, f.w)) return EquivalentUpToGlobalPhase;

	return Equivalent;
}
