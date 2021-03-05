/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "PowerOfSimulationEquivalenceChecker.hpp"

namespace ec {

	bool PowerOfSimulationEquivalenceChecker::checkWithStimulus(const dd::Edge& stimulus, const Configuration& config) {
		line.fill(qc::LINE_DEFAULT);
		qc::permutationMap map = initial1;
		dd::Edge e = stimulus;
		dd->incRef(e);
		it1 = qc1.begin();

		while (it1 != end1) {
			applyGate(it1, e, map, end1);
			++it1;
		}
		// correct permutation if necessary
		qc::QuantumComputation::changePermutation(e, map, output1, line, dd);
		e = dd->reduceGarbage(e, garbage1);
		e = dd->reduceAncillae(e, ancillary1);

		map = initial2;
		dd::Edge f = stimulus;
		dd->incRef(f);
		it2 = qc2.begin();

		while (it2 != end2) {
			applyGate(it2, f, map, end2);
			++it2;
		}

		// correct permutation if necessary
		qc::QuantumComputation::changePermutation(f, map, output2, line, dd);
		f = dd->reduceGarbage(f, garbage2);
		f = dd->reduceAncillae(f, ancillary2);

		results.fidelity = dd->fidelity(e, f);

		results.nsims++;
		results.maxActive = std::max(dd->maxActive, results.maxActive);

		if (results.fidelity < config.fidelity_limit) {
			results.equivalence = NonEquivalent;
			dd->decRef(e);
			dd->decRef(f);

			if (config.storeCEXinput) {
				results.cexInput = dd->getVector(stimulus);
			}
			if (config.storeCEXoutput) {
				results.cexOutput1 = dd->getVector(e);
				results.cexOutput2 = dd->getVector(f);
			}

			dd->garbageCollect(true);
			return true;
		} else if (results.nsims == static_cast<unsigned long long>(std::pow(2.L, nqubits_for_stimuli))) {
			results.equivalence = Equivalent;
			dd->decRef(e);
			dd->decRef(f);
			dd->garbageCollect(true);
			return true;
		} else {
			results.equivalence = ProbablyEquivalent;
			dd->decRef(e);
			dd->decRef(f);
			dd->garbageCollect(true);
			return false;
		}
	}

	bool PowerOfSimulationEquivalenceChecker::checkWithClassicalStimulus(unsigned long long stimulus, const Configuration& config) {
		results.basisState = stimulus;
		std::bitset<qc::MAX_QUBITS> stimulusBits(results.basisState);

		auto in = dd->makeBasisState(nqubits, stimulusBits);
		dd->incRef(in);
		bool done = checkWithStimulus(in, config);
		dd->decRef(in);
		return done;
	}

	bool PowerOfSimulationEquivalenceChecker::checkWithLocalQuantumStimulus(const std::vector<dd::BasisStates>& stimulus, const Configuration& config) {
		auto in = dd->makeBasisState(nqubits, stimulus);
		dd->incRef(in);
		bool done = checkWithStimulus(in, config);
		dd->decRef(in);
		return done;
	}

	bool PowerOfSimulationEquivalenceChecker::checkWithGlobalQuantumStimulus(const qc::RandomCliffordCircuit& rcs, const Configuration& config) {
		auto stabilizer = rcs.simulate(dd->makeZeroState(nqubits_for_stimuli), dd);
		dd::Edge edges[4];
		edges[1] = edges[2] = edges[3] = dd->DDzero;

		auto initial = stabilizer;
		for (short p = 0; p < static_cast<short>(nqubits-nqubits_for_stimuli); p++) {
			edges[0] = initial;
			initial = dd->makeNonterminal(p+nqubits_for_stimuli, edges);
		}
		dd->incRef(initial);
		bool done = checkWithStimulus(initial, config);
		dd->decRef(initial);
		dd->decRef(stabilizer);
		return done;
	}

	void PowerOfSimulationEquivalenceChecker::checkWithClassicalStimuli(const Configuration& config) {
		auto done = false;
		while (!done && results.nsims < config.max_sims) {
			// generate distinct stimulus
			auto new_stimulus = stimuli.insert(stimuliGenerator());
			while (!new_stimulus.second) {
				new_stimulus = stimuli.insert(stimuliGenerator());
			}
			done = checkWithClassicalStimulus(*new_stimulus.first, config);
		}
	}

	void PowerOfSimulationEquivalenceChecker::checkWithLocalQuantumStimuli(const Configuration& config) {
		bool done = false;
		while (!done && results.nsims < config.max_sims) {
			auto stimulus = std::vector<dd::BasisStates>(nqubits, dd::BasisStates::zero);
			for (int i=0; i<nqubits_for_stimuli; ++i) {
				switch (basisStateGenerator()) {
					case 0:
						stimulus.at(i) = dd::BasisStates::zero;
						break;
					case 1:
						stimulus.at(i) = dd::BasisStates::one;
						break;
					case 2:
						stimulus.at(i) = dd::BasisStates::plus;
						break;
					case 3:
						stimulus.at(i) = dd::BasisStates::minus;
						break;
					case 4:
						stimulus.at(i) = dd::BasisStates::right;
						break;
					case 5:
						stimulus.at(i) = dd::BasisStates::left;
						break;
					default:
						stimulus.at(i) = dd::BasisStates::zero;
				}
			}
			done = checkWithLocalQuantumStimulus(stimulus, config);
		}
	}

	void PowerOfSimulationEquivalenceChecker::checkWithGlobalQuantumStimuli(const Configuration& config) {
		bool done = false;
		while (!done && results.nsims < config.max_sims) {
			results.timeout = false;
			auto rcs = qc::RandomCliffordCircuit(nqubits_for_stimuli, static_cast<unsigned int>(std::round(std::log2(nqubits_for_stimuli))), stimuliGenerator());
			done = checkWithGlobalQuantumStimulus(rcs, config);
		}
	}

	void PowerOfSimulationEquivalenceChecker::check(const Configuration& config) {
		results.method = PowerOfSimulation;
		results.stimuliType = config.stimuliType;
		setTolerance(config.tolerance);
		results.nsims = 0;
		end1 = qc1.end();
		end2 = qc2.end();

		if (!validInstance())
			return;

		auto start = std::chrono::steady_clock::now();

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

		switch (config.stimuliType) {
			case Classical:
				checkWithClassicalStimuli(config);
				break;
			case LocalQuantum:
				checkWithLocalQuantumStimuli(config);
				break;
			case GlobalQuantum:
				checkWithGlobalQuantumStimuli(config);
				break;
		}

		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();
		if (config.printCSV) {
			printCSVEntry(std::cout);
			std::cout << std::flush;
		}
	}
}
