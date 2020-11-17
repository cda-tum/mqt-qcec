/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "PowerOfSimulationEquivalenceChecker.hpp"

namespace ec {

	void PowerOfSimulationEquivalenceChecker::check(const Configuration& config) {
		results.method = PowerOfSimulation;
		results.stimuliType = config.stimuliType;
		setTolerance(config.tolerance);
		results.nsims = 0;
		end1 = qc1.end();
		end2 = qc2.end();

		if (!validInstance())
			return;

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

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - start;
		results.time = diff.count();
		if (config.printCSV) {
			printCSVEntry(std::cout);
			std::cout << std::flush;
		}
	}

	void PowerOfSimulationEquivalenceChecker::checkWithClassicalStimuli(const Configuration& config) {
		while (results.nsims < config.max_sims) {
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
			dd::Edge f = dd->makeBasisState(nqubits, stimulusBits);
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

			if (results.fidelity < config.fidelity_limit) {
				results.equivalence = NonEquivalent;
				dd->decRef(e);
				dd->decRef(f);

				if (config.storeCEXinput) {
					results.cexInput = getStateVector(dd->makeBasisState(nqubits, stimulusBits));
				}
				if (config.storeCEXoutput) {
					results.cexOutput1 = getStateVector(e);
					results.cexOutput2 = getStateVector(f);
				}

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
			}
		}
	}


	void PowerOfSimulationEquivalenceChecker::checkWithLocalQuantumStimuli(const Configuration& config) {
		while (results.nsims < config.max_sims) {
			results.timeout = false;
			basisStates.clear();
			for (int i=0; i<nqubits_for_stimuli; ++i) {
				dd::BasisStates basisState;
				switch (basisStateGenerator()) {
					case 0:
						basisState = dd::BasisStates::zero;
						break;
					case 1:
						basisState = dd::BasisStates::one;
						break;
					case 2:
						basisState = dd::BasisStates::plus;
						break;
					case 3:
						basisState = dd::BasisStates::minus;
						break;
					case 4:
						basisState = dd::BasisStates::right;
						break;
					case 5:
						basisState = dd::BasisStates::left;
						break;
					default:
						basisState = dd::BasisStates::zero;
				}
				basisStates.emplace_back(basisState);
			}
			for (int j=0; j<(nqubits-nqubits_for_stimuli); ++j) {
				basisStates.emplace_back(dd::BasisStates::zero); // ancillaries are always initialized to zero
			}

			line.fill(qc::LINE_DEFAULT);
			qc::permutationMap map = initial1;
			dd::Edge e = dd->makeBasisState(nqubits, basisStates);
			dd->incRef(e);

			auto timelimit = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(config.timeout);

			for (auto & op : qc1) {
				if (std::chrono::high_resolution_clock::now() >= timelimit) {
					results.timeout = true;
					dd->decRef(e);
					dd->garbageCollect(true);
					break;
				}

				auto tmp = dd->multiply(op->getDD(dd, line, map), e);
				dd->incRef(tmp);
				dd->decRef(e);
				e = tmp;
				dd->garbageCollect();
			}

			if (!results.timeout) {
				// correct permutation if necessary
				qc::QuantumComputation::changePermutation(e, map, output1, line, dd);
				e = reduceGarbage(e, garbage1);
				e = reduceAncillae(e, ancillary1);

				line.fill(qc::LINE_DEFAULT);
				map = initial2;
				dd::Edge f = dd->makeBasisState(nqubits, basisStates);
				dd->incRef(f);

				timelimit = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(config.timeout);

				for (auto & op : qc2) {
					if (std::chrono::high_resolution_clock::now() >= timelimit) {
						results.timeout = true;
						dd->decRef(f);
						dd->garbageCollect(true);
						break;
					}

					auto tmp = dd->multiply(op->getDD(dd, line, map), f);
					dd->incRef(tmp);
					dd->decRef(f);
					f = tmp;
					dd->garbageCollect();
				}

				if (!results.timeout) {
					// correct permutation if necessary
					qc::QuantumComputation::changePermutation(f, map, output2, line, dd);
					f = reduceGarbage(f, garbage2);
					f = reduceAncillae(f, ancillary2);

					results.fidelity = dd->fidelity(e, f);

					++results.nsims;
					results.maxActive = std::max(dd->maxActive, results.maxActive);

					if (results.fidelity < config.fidelity_limit) {
						results.equivalence = NonEquivalent;
						dd->decRef(e);
						dd->decRef(f);

						if (config.storeCEXinput) {
							results.cexInput = getStateVector(dd->makeBasisState(nqubits, basisStates));
						}
						if (config.storeCEXoutput) {
							results.cexOutput1 = getStateVector(e);
							results.cexOutput2 = getStateVector(f);
						}

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
					}
				}
			}
		}
	}

	void PowerOfSimulationEquivalenceChecker::checkWithGlobalQuantumStimuli(const Configuration& config) {
		while (results.nsims < config.max_sims) {
			results.timeout = false;
			auto rcs = qc::RandomCliffordCircuit(nqubits_for_stimuli, static_cast<unsigned int>(std::round(std::log2(nqubits_for_stimuli))), stimuliGenerator());
			auto initial = rcs.simulate(dd->makeZeroState(nqubits_for_stimuli), dd);
			dd::Edge edges[4];
			edges[1] = edges[2] = edges[3] = dd->DDzero;

			for (short p = 0; p < short(nqubits-nqubits_for_stimuli); p++) {
				edges[0] = initial;
				initial = dd->makeNonterminal(short(p+nqubits_for_stimuli), edges);
				initial.p->ref++;
				dd->activeNodeCount++;
				dd->maxActive = std::max(dd->maxActive, dd->activeNodeCount);
			}

			line.fill(qc::LINE_DEFAULT);
			qc::permutationMap map = initial1;
			dd::Edge e = initial;
			dd->incRef(e);

			auto timelimit = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(10*config.timeout);

			for (auto & op : qc1) {
				if (std::chrono::high_resolution_clock::now() >= timelimit) {
					results.timeout = true;
					dd->decRef(e);
					dd->garbageCollect(true);
					break;
				}

				auto tmp = dd->multiply(op->getDD(dd, line, map), e);
				dd->incRef(tmp);
				dd->decRef(e);
				e = tmp;
				dd->garbageCollect();
			}

			if (!results.timeout) {
				// correct permutation if necessary
				qc::QuantumComputation::changePermutation(e, map, output1, line, dd);
				e = reduceGarbage(e, garbage1);
				e = reduceAncillae(e, ancillary1);

				line.fill(qc::LINE_DEFAULT);
				map = initial2;
				dd::Edge f = initial;
				dd->incRef(f);

				timelimit = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(15*config.timeout);

				for (auto & op : qc2) {
					if (std::chrono::high_resolution_clock::now() >= timelimit) {
						results.timeout = true;
						dd->decRef(f);
						dd->garbageCollect(true);
						break;
					}

					auto tmp = dd->multiply(op->getDD(dd, line, map), f);
					dd->incRef(tmp);
					dd->decRef(f);
					f = tmp;
					dd->garbageCollect();
				}

				if (!results.timeout) {
					// correct permutation if necessary
					qc::QuantumComputation::changePermutation(f, map, output2, line, dd);
					f = reduceGarbage(f, garbage2);
					f = reduceAncillae(f, ancillary2);

					results.fidelity = dd->fidelity(e, f);
					++results.nsims;
					results.maxActive = std::max(dd->maxActive, results.maxActive);

					if (results.fidelity < config.fidelity_limit) {
						results.equivalence = NonEquivalent;
						dd->decRef(e);
						dd->decRef(f);

						if (config.storeCEXinput) {
							results.cexInput = getStateVector(initial);
						}
						if (config.storeCEXoutput) {
							results.cexOutput1 = getStateVector(e);
							results.cexOutput2 = getStateVector(f);
						}

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
					}
				}
			}
		}
	}

	dd::ComplexValue PowerOfSimulationEquivalenceChecker::getStateVectorAmplitude(dd::Edge e, const std::bitset<dd::MAXN>& path) const {
		if(dd::Package::isTerminal(e)) {
			return {dd::ComplexNumbers::val(e.w.r), dd::ComplexNumbers::val(e.w.i)};
		}

		auto c = dd->cn.getTempCachedComplex(1, 0);
		do {
			dd::ComplexNumbers::mul(c, c, e.w);
			unsigned short choice = path[e.p->v];
			e = e.p->e[2*choice];
		} while(!dd::Package::isTerminal(e));
		dd::ComplexNumbers::mul(c, c, e.w);

		return {dd::ComplexNumbers::val(c.r), dd::ComplexNumbers::val(c.i)};
	}

	std::vector<dd::ComplexValue> PowerOfSimulationEquivalenceChecker::getStateVector(dd::Edge e) const {
		assert(nqubits < 60); // On 64bit system the vector can hold up to (2^60)-1 elements, if memory permits
		std::bitset<dd::MAXN> path{};
		std::vector<dd::ComplexValue> results(1ull << nqubits, {0,0});
		for(unsigned long long i = 0; i < 1ull << nqubits; ++i) {
			results[i] = getStateVectorAmplitude(e, path);
			nextPath(path);
		}
		return results;
	}

}
