/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
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
#include "algorithms/RandomCliffordCircuit.hpp"

#define DEBUG_MODE_SIMULATION 0

namespace ec {

	class PowerOfSimulationEquivalenceChecker: public EquivalenceChecker {
	protected:
		std::function<unsigned long long()> stimuliGenerator;
		std::function<unsigned short()> basisStateGenerator;
		std::unordered_set<unsigned long long> stimuli;
		std::vector<dd::BasisStates> basisStates;

		unsigned short nqubits_for_stimuli = 0;

		unsigned long long seed = 0;
		std::mt19937_64 mt;
		std::uniform_int_distribution<unsigned long long> distribution;
		std::uniform_int_distribution<unsigned short> basisStateDistribution;

		void checkWithClassicalStimuli(const Configuration& config = Configuration{});
		void checkWithLocalQuantumStimuli(const Configuration& config = Configuration{});
		void checkWithGlobalQuantumStimuli(const Configuration& config = Configuration{});

		template<size_t N>
		static void nextPath(std::bitset<N>& path) {
			for (size_t i=0; i<N; ++i) {
				if (path[i] == 0) {
					path[i] = 1;
					break;
				}
				path[i] = 0;
			}
		}
		dd::ComplexValue getStateVectorAmplitude(dd::Edge e, const std::bitset<dd::MAXN>& path) const;
		std::vector<dd::ComplexValue> getStateVector(dd::Edge e) const;

	public:
		PowerOfSimulationEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, unsigned long seed = 0): EquivalenceChecker(qc1, qc2), seed(seed) {
			nqubits_for_stimuli = qc1.getNqubitsWithoutAncillae();
			basisStates = std::vector<dd::BasisStates>(nqubits);

			if(seed == 0) {
				// this is probably overkill but better safe than sorry
				std::array<std::mt19937_64::result_type , std::mt19937_64::state_size> random_data{};
				std::random_device rd;
				std::generate(begin(random_data), end(random_data), std::ref(rd));
				std::seed_seq seeds(begin(random_data), end(random_data));
				mt.seed(seeds);
			} else {
				mt.seed(seed);
			}
			distribution = std::uniform_int_distribution<unsigned long long>(0, (unsigned long long) (std::pow((long double) 2, nqubits_for_stimuli) - 1));
			stimuliGenerator = [&]() { return distribution(mt); };
			dd->setMode(dd::Vector);

			basisStateDistribution = std::uniform_int_distribution<unsigned short>(0, 5);
			basisStateGenerator = [&]() { return basisStateDistribution(mt); };
		};

		void check(const Configuration& config) override;
		void check() override { return check(Configuration{}); }
	};

}

#endif //QCEC_POWEROFSIMULATIONEQUIVALENCECHECKER_HPP
