/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "EquivalenceChecker.hpp"

#include <chrono>

void ec::EquivalenceChecker::check() {
	results.method = "Reference";

	if (!validInstance())
		return;

	#if DEBUG_OUTPUT
	std::cout << "Starting 1st circuit\n" << std::flush;
	#endif

	auto start = std::chrono::high_resolution_clock::now();
	dd::Edge e = qc1->buildFunctionality(dd);

	#if DEBUG_OUTPUT
		std::cout << "Built 1st circuit. Starting 2nd circuit.\n" << std::flush;
	#endif

	dd::Edge f = qc2->buildFunctionality(dd);

	#if DEBUG_OUTPUT
		std::cout << "Built 2nd circuit\n" << std::flush;
	#endif

	results.maxActive = dd->maxActive;

	results.equivalence = dd->equals(e, f)? Equivalent: NonEquivalent;
	if (results.equivalence == NonEquivalent) {
		#if DEBUG_OUTPUT
		std::cout << "Starting difference computation.\n" << std::flush;
		#endif

		results.result = dd->multiply(e, dd->conjugateTranspose(f));
		dd->decRef(e);
		dd->decRef(f);
		dd->incRef(results.result);

		#if DEBUG_OUTPUT
		std::cout << "Finished difference computation.\n" << std::flush;
		#endif
	} else {
		results.result = e;
		dd->decRef(f);
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	results.time = diff.count();
}

bool ec::EquivalenceChecker::validInstance() {
	if (qc1->getNqubits() != qc2->getNqubits()) {
		results.differentNrQubits = true;
		std::cerr << "Circuits act on a different number of qubits." << std::endl;
		return false;
	}

	if (qc1->getNqubits() > dd::MAXN) {
		results.tooManyQubits = true;
		std::cerr << "Circuit contains too many qubits to be manageable by DD package" << std::endl;
		return false;
	}

	return true;
}
