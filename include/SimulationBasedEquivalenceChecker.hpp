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
        std::function<std::size_t()>    stimuliGenerator;
        std::function<unsigned short()> basisStateGenerator;
        std::unordered_set<std::size_t> stimuli;

        dd::QubitCount nqubits_for_stimuli = 0;

        std::size_t                                   seed = 0;
        std::mt19937_64                               mt;
        std::uniform_int_distribution<std::size_t>    distribution;
        std::uniform_int_distribution<unsigned short> basisStateDistribution;

        qc::VectorDD generateRandomStimulus(StimuliType type = StimuliType::Classical);
        qc::VectorDD generateRandomClassicalStimulus();
        qc::VectorDD generateRandomLocalQuantumStimulus();
        qc::VectorDD generateRandomGlobalQuantumStimulus();
        bool         simulateWithStimulus(const qc::VectorDD& stimulus, EquivalenceCheckingResults& results, const Configuration& config = Configuration{});
        void         checkWithStimulus(const qc::VectorDD& stimulus, EquivalenceCheckingResults& results, const Configuration& config = Configuration{});

    public:
        SimulationBasedEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, std::size_t seed = 0):
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
            distribution     = std::uniform_int_distribution<std::size_t>(0, static_cast<std::size_t>(std::pow(2.L, nqubits_for_stimuli) - 1));
            stimuliGenerator = [&]() { return distribution(mt); };

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
