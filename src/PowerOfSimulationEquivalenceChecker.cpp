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

		#if DEBUG_MODE_SIMULATION
		std::cout << "Simulating max. " << max_sims << " times with target fidelity " << fidelity_limit << std::endl;
		#endif

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
			std::bitset<64> stimulusBits(results.basisState);

			#if DEBUG_MODE_SIMULATION
			std::cout << "\033[32mSim " << results.nsims << ": " << results.basisState << "\033[0m" << std::endl;
			#endif

			dd::Edge in1 = dd->makeBasisState(qc1.getNqubits(), stimulusBits);
			std::array<short, qc::MAX_QUBITS> l{};
			l.fill(qc::LINE_DEFAULT);
			qc::permutationMap map = qc1.initialLayout;

			dd::Edge e = in1;
			dd->incRef(e);

			#if DEBUG_MODE_SIMULATION
				visited.clear();
				auto nc = nodecount(e, visited);
				maxSize = std::max(maxSize, nc-1);
				addToAverage(nc-1);
			#endif

			for (auto & op : qc1) {
				auto tmp = dd->multiply(op->getDD(dd, l, map), e);
				dd->incRef(tmp);
				dd->decRef(e);
				e = tmp;
				dd->garbageCollect();

				#if DEBUG_MODE_SIMULATION
					visited.clear();
					nc = nodecount(e, visited);
					maxSize = std::max(maxSize, nc-1);
					addToAverage(nc-1);
				#endif
			}

			// correct permutation if necessary
			qc::QuantumComputation::changePermutation(e, map, qc1.outputPermutation, l, dd);

			qc1.reduceAncillae(e, dd);

			#if DEBUG_MODE_SIMULATION
			std::stringstream ss1 {};
			ss1 << "e_" << filename1 << "_sim_" << results.nsims << ".dot";
			dd->export2Dot(e, ss1.str().c_str(), true);
			std::cout << "[after 1st circ] Complex count: " << dd->cn.count << std::endl;
			#endif

			dd::Edge in2 = dd->makeBasisState(qc2.getNqubits(), stimulusBits);

			l.fill(qc::LINE_DEFAULT);
			map = qc2.initialLayout;

			dd::Edge f = in2;
			dd->incRef(f);

			#if DEBUG_MODE_SIMULATION
				visited.clear();
				nc = nodecount(f, visited);
				maxSize = std::max(maxSize, nc-1);
				addToAverage(nc-1);
			#endif

			for (auto & op : qc2) {
				auto tmp = dd->multiply(op->getDD(dd, l, map), f);
				dd->incRef(tmp);
				dd->decRef(f);
				f = tmp;
				dd->garbageCollect();

				#if DEBUG_MODE_SIMULATION
					visited.clear();
					nc = nodecount(f, visited);
					maxSize = std::max(maxSize, nc-1);
					addToAverage(nc-1);
				#endif
			}

			// correct permutation if necessary
			qc::QuantumComputation::changePermutation(f, map, qc2.outputPermutation, l, dd);

			qc2.reduceAncillae(f, dd);

			#if DEBUG_MODE_SIMULATION
			std::stringstream ss2 {};
			ss2 << "f_" << filename1 << "_sim_" << results.nsims << ".dot";
			dd->export2Dot(f, ss2.str().c_str(), true);
			std::cout << "[after 2nd circ] Complex count: " << dd->cn.count << std::endl;
			#endif

			results.fidelity = dd->fidelity(e, f);

			#if DEBUG_MODE_SIMULATION
			std::cout << "\033[33mFidelity: " << results.fidelity << "\033[0m" << std::endl;
			#endif

			results.nsims++;
			results.maxActive = std::max(dd->maxActive, results.maxActive);

			if (results.fidelity < fidelity_limit) {
				results.equivalence = NonEquivalent;
				dd->decRef(e);
				dd->decRef(f);
				dd->garbageCollect(true);

				#if DEBUG_MODE_SIMULATION
				std::cout << "[after success] Complex count: " << dd->cn.count << std::endl;
				if (dd->cn.count > 4) {
					std::cout << "\033[33m!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[0m" << std::endl;
				}
				#endif
				break;
			} else if (results.nsims == (unsigned long long) std::pow((long double)2, nqubits_for_stimuli)) {
				results.equivalence = Equivalent;
				dd->decRef(e);
				dd->decRef(f);
				dd->garbageCollect(true);

				#if DEBUG_MODE_SIMULATION
				std::cout << "[after success] Complex count: " << dd->cn.count << std::endl;
				#endif
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
				#if DEBUG_MODE_SIMULATION
				std::cout << "[end of sim] Complex count: " << dd->cn.count << std::endl;
				#endif
			}
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();

		#if DEBUG_MODE_SIMULATION
		std::cout << "Max size: " << maxSize << std::endl;
		std::cout << "Avg size: " << average << std::endl;
		#endif
	}

}
