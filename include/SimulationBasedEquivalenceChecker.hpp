/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QCEC_SIMULATIONBASEDEQUIVALENCECHECKER_HPP
#define QCEC_SIMULATIONBASEDEQUIVALENCECHECKER_HPP

#include "CircuitOptimizer.hpp"
#include "EquivalenceChecker.hpp"
#include "algorithms/RandomCliffordCircuit.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cmath>
#include <functional>
#include <random>
#include <sstream>
#include <unordered_set>

#define DEBUG_MODE_SIMULATION 0

namespace ec {

    class SimulationBasedEquivalenceChecker: public EquivalenceChecker {
    protected:
        std::function<unsigned long long()>    stimuliGenerator;
        std::function<unsigned short()>        basisStateGenerator;
        std::unordered_set<unsigned long long> stimuli;

        unsigned short nqubits_for_stimuli = 0;

        unsigned long long                                seed = 0;
        std::mt19937_64                                   mt;
        std::uniform_int_distribution<unsigned long long> distribution;
        std::uniform_int_distribution<unsigned short>     basisStateDistribution;

        dd::Edge generateRandomStimulus(StimuliType type = StimuliType::Classical);
        dd::Edge generateRandomClassicalStimulus();
        dd::Edge generateRandomLocalQuantumStimulus();
        dd::Edge generateRandomGlobalQuantumStimulus();
        bool     simulateWithStimulus(const dd::Edge& stimulus, EquivalenceCheckingResults& results, const Configuration& config = Configuration{});
        void     checkWithStimulus(const dd::Edge& stimulus, EquivalenceCheckingResults& results, const Configuration& config = Configuration{});

    public:
        SimulationBasedEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, unsigned long seed = 0):
            EquivalenceChecker(qc1, qc2), seed(seed) {
            method              = ec::Method::Simulation;
            nqubits_for_stimuli = qc1.getNqubitsWithoutAncillae();

            if (seed == 0) {
                // this is probably overkill but better safe than sorry
                std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> random_data{};
                std::random_device                                                    rd;
                std::generate(begin(random_data), end(random_data), std::ref(rd));
                std::seed_seq seeds(begin(random_data), end(random_data));
                mt.seed(seeds);
            } else {
                mt.seed(seed);
            }
            distribution     = std::uniform_int_distribution<unsigned long long>(0, static_cast<unsigned long long>(std::pow(2.L, nqubits_for_stimuli) - 1));
            stimuliGenerator = [&]() { return distribution(mt); };
            dd->setMode(dd::Vector);

            basisStateDistribution = std::uniform_int_distribution<unsigned short>(0, 5);
            basisStateGenerator    = [&]() { return basisStateDistribution(mt); };
        };

        EquivalenceCheckingResults check(const Configuration& config) override;
        EquivalenceCheckingResults check() override { return check(ec::Configuration{}); }
        EquivalenceCheckingResults checkZeroState(const Configuration& config = Configuration{});
        EquivalenceCheckingResults checkPlusState(const Configuration& config = Configuration{});
    };

} // namespace ec

#endif //QCEC_SIMULATIONBASEDEQUIVALENCECHECKER_HPP
