/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "PowerOfSimulationEquivalenceChecker.hpp"

namespace ec {

	void PowerOfSimulationEquivalenceChecker::check(const Configuration& config) {
		results.method = PowerOfSimulation;
		fidelity_limit = config.fidelity_limit;
		max_sims = config.max_sims;
		results.nsims = 0;
		end1 = qc1.end();
		end2 = qc2.end();

		if (!validInstance())
			return;

		auto start = std::chrono::high_resolution_clock::now();
		while (results.nsims < max_sims) {
			// generate distinct stimulus
			auto new_stimulus = stimuli.insert(stimuliGenerator());
			while (!new_stimulus.second) {
				new_stimulus = stimuli.insert(stimuliGenerator());
			}
			results.basisState = *new_stimulus.first;
			std::bitset<qc::MAX_QUBITS> stimulusBits(results.basisState);
			line.fill(qc::LINE_DEFAULT);

			qc::permutationMap map = initial1;
			dd::Edge e = dd->makeBasisState(nqubits, stimulusBits);
			dd->incRef(e);
			it1 = qc1.begin();

			while (it1 != end1) {
				applyGate(it1, e, map, end1);
				++it1;
			}
			// correct permutation if necessary
			qc::QuantumComputation::changePermutation(e, map, output1, line, dd);
			e = reduceGarbage(e, garbage1);

			map = initial2;
			dd::Edge f = dd->makeBasisState(nqubits, stimulusBits);;
			dd->incRef(f);
			it2 = qc2.begin();

			while (it2 != end2) {
				applyGate(it2, f, map, end2);
				++it2;
			}

			// correct permutation if necessary
			qc::QuantumComputation::changePermutation(f, map, output2, line, dd);
			f = reduceGarbage(f, garbage2);

			results.fidelity = dd->fidelity(e, f);

			results.nsims++;
			results.maxActive = std::max(dd->maxActive, results.maxActive);

			if (results.fidelity < fidelity_limit) {
				results.equivalence = NonEquivalent;
				dd->decRef(e);
				dd->decRef(f);
				dd->garbageCollect(true);
				break;
			} else if (results.nsims == (unsigned long long) std::pow((long double)2, nqubits_for_stimuli)) {
				results.equivalence = Equivalent;
				dd->decRef(e);
				dd->decRef(f);
				dd->garbageCollect(true);
				break;
			} else {
				results.equivalence = ProbablyEquivalent;
				dd->decRef(e);
				dd->decRef(f);
				dd->garbageCollect(true);

				auto end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> diff = end - start;
				results.time = diff.count();
				if (config.printCSV) {
					printCSVEntry(std::cout);
					std::cout << std::flush;
				}
			}
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();
	}

}
