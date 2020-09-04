/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QCEC_POWEROFSIMULATIONEQUIVALENCECHECKER_HPP
#define QCEC_POWEROFSIMULATIONEQUIVALENCECHECKER_HPP

#include <bitset>
#include <unordered_set>
#include <functional>
#include <random>
#include <cmath>
#include <chrono>
#include <sstream>
#include <array>
#include <algorithm>

#include "EquivalenceChecker.hpp"
#include "CircuitOptimizer.hpp"

#define DEBUG_MODE_SIMULATION 0

namespace ec {

	class PowerOfSimulationEquivalenceChecker: public EquivalenceChecker {
	protected:
		std::function<unsigned long long()> stimuliGenerator;
		std::unordered_set<unsigned long long> stimuli;

		double fidelity_limit = 0.9999;
		unsigned long long max_sims = 16;
		unsigned short nqubits_for_stimuli = 0;
		std::array<short, qc::MAX_QUBITS> line{};

		unsigned long long seed = 0;
		std::mt19937_64 mt;
		std::uniform_int_distribution<unsigned long long> distribution;

	public:
		PowerOfSimulationEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, unsigned long seed = 0): EquivalenceChecker(qc1, qc2), seed(seed) {
			// augment the smaller circuit with ancillary qubits and change the qubits in the larger circuit to ancillary
			nqubits_for_stimuli = qc1.getNqubitsWithoutAncillae();

			if(seed == 0) {
				// this is probably overkill but better safe than sorry
				std::array<std::mt19937_64::result_type , std::mt19937_64::state_size> random_data{};
				std::random_device rd;
				std::generate(begin(random_data), end(random_data), [&](){return rd();});
				std::seed_seq seeds(begin(random_data), end(random_data));
				mt.seed(seeds);
			} else {
				mt.seed(seed);
			}
			distribution = std::uniform_int_distribution<unsigned long long>(0, (unsigned long long) (std::pow((long double) 2, nqubits_for_stimuli) - 1));
			stimuliGenerator = [&]() { return distribution(mt); };
			dd->setMode(dd::Vector);

			// optimization pass
			//qc::CircuitOptimizer::swapGateFusion(qc1);
			//qc::CircuitOptimizer::singleGateFusion(qc1);

			//qc::CircuitOptimizer::swapGateFusion(qc2);
			//qc::CircuitOptimizer::singleGateFusion(qc2);
		};

		void check(const Configuration& config) override;
		void check() override { return check(Configuration{}); }
	};

}

#endif //QCEC_POWEROFSIMULATIONEQUIVALENCECHECKER_HPP
