/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "StateType.hpp"
#include "algorithms/RandomCliffordCircuit.hpp"
#include "checker/dd/TaskManager.hpp"
#include "dd/Package.hpp"
#include "dd/Simulation.hpp"

#include <functional>
#include <random>

namespace ec {
    class StateGenerator {
    public:
        explicit StateGenerator(std::size_t seed = 0U):
            seed(seed) {
            seedGenerator(seed);

            // this generator produces random bases from the set { |0>, |1>, |+>, |->, |L>, |R> }
            random1QBasisDistribution = std::uniform_int_distribution<std::size_t>(0U, 5U);
        }

        template<class DDPackage = dd::Package<>>
        qc::VectorDD generateRandomState(std::unique_ptr<DDPackage>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U, StateType type = StateType::ComputationalBasis) {
            switch (type) {
                case ec::StateType::Random1QBasis:
                    return generateRandom1QBasisState(dd, totalQubits, ancillaryQubits);
                case ec::StateType::Stabilizer:
                    return generateRandomStabilizerState(dd, totalQubits, ancillaryQubits);
                case ec::StateType::ComputationalBasis:
                default:
                    return generateRandomComputationalBasisState(dd, totalQubits, ancillaryQubits);
            }
        }

        template<class DDPackage = dd::Package<>>
        qc::VectorDD generateRandomComputationalBasisState(std::unique_ptr<DDPackage>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U) {
            // determine how many qubits truly are random
            const auto        randomQubits = totalQubits - ancillaryQubits;
            std::vector<bool> stimulusBits(totalQubits, false);

            // check if there still is a unique computational basis state
            if (randomQubits <= 63) {
                const std::uint_least64_t maxStates = (static_cast<std::uint_least64_t>(1U) << randomQubits);
                assert(generatedComputationalBasisStates.size() != maxStates);
                // generate a unique computational basis state
                std::uniform_int_distribution<std::uint_least64_t> distribution(0, maxStates - 1);
                auto [randomState, success] = generatedComputationalBasisStates.insert(distribution(mt));
                while (!success) {
                    std::tie(randomState, success) = generatedComputationalBasisStates.insert(distribution(mt));
                }

                // generate the bitvector corresponding to the random state
                for (dd::QubitCount i = 0; i < randomQubits; ++i) {
                    if (*randomState & (static_cast<std::uint_least64_t>(1U) << i)) {
                        stimulusBits[i] = true;
                    }
                }
            } else {
                // check how many 64bit numbers are needed for each random state
                const auto nr = static_cast<std::size_t>(std::ceil(randomQubits / 64.));
                // generate enough random numbers
                std::vector<std::mt19937_64::result_type> randomNumbers(nr, 0U);
                for (auto i = 0U; i < nr; ++i) {
                    randomNumbers[i] = mt();
                }
                // generate the corresponding bitvector
                for (dd::QubitCount i = 0U; i < randomQubits; ++i) {
                    if (randomNumbers[i / 64U] & (static_cast<std::uint_least64_t>(1U) << (i % 64U))) {
                        stimulusBits[i] = true;
                    }
                }
            }

            // return the appropriate decision diagram
            return dd->makeBasisState(totalQubits, stimulusBits);
        }

        template<class DDPackage = dd::Package<>>
        qc::VectorDD generateRandom1QBasisState(std::unique_ptr<DDPackage>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U) {
            // determine how many qubits truly are random
            const auto randomQubits = totalQubits - ancillaryQubits;

            // choose a random basis state for each qubit
            auto randomBasisState = std::vector<dd::BasisStates>(totalQubits, dd::BasisStates::zero);
            for (dd::QubitCount i = 0; i < randomQubits; ++i) {
                switch (random1QBasisDistribution(mt)) {
                    case 0:
                        randomBasisState[i] = dd::BasisStates::zero;
                        break;
                    case 1:
                        randomBasisState[i] = dd::BasisStates::one;
                        break;
                    case 2:
                        randomBasisState[i] = dd::BasisStates::plus;
                        break;
                    case 3:
                        randomBasisState[i] = dd::BasisStates::minus;
                        break;
                    case 4:
                        randomBasisState[i] = dd::BasisStates::right;
                        break;
                    case 5:
                        randomBasisState[i] = dd::BasisStates::left;
                        break;
                    default:
                        break;
                }
            }

            // return the appropriate decision diagram
            return dd->makeBasisState(totalQubits, randomBasisState);
        }

        template<class DDPackage = dd::Package<>>
        qc::VectorDD generateRandomStabilizerState(std::unique_ptr<DDPackage>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U) {
            // determine how many qubits truly are random
            const auto randomQubits = totalQubits - ancillaryQubits;

            // generate a random Clifford circuit with appropriate depth
            auto rcs = qc::RandomCliffordCircuit(randomQubits, static_cast<std::size_t>(std::round(std::log2(randomQubits))), mt());

            // generate the associated stabilizer state by simulating the Clifford circuit
            auto stabilizer = simulate(&rcs, dd->makeZeroState(randomQubits), dd);

            // decrease the ref count right after so that it stays correct later on
            dd->decRef(stabilizer);

            // add |0> edges for all the ancillary qubits
            auto initial = stabilizer;
            for (dd::QubitCount p = randomQubits; p < totalQubits; p++) {
                initial = dd->makeDDNode(static_cast<dd::Qubit>(p), std::array{initial, qc::VectorDD::zero});
            }

            // return the resulting decision diagram
            return initial;
        }

        void seedGenerator(std::size_t s) {
            seed = s;
            if (seed == 0U) {
                std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> random_data{};
                std::random_device                                                    rd;
                std::generate(std::begin(random_data), std::end(random_data), std::ref(rd));
                std::seed_seq seeds(std::begin(random_data), std::end(random_data));
                mt.seed(seeds);
            } else {
                mt.seed(seed);
            }
        }

        void clear() { generatedComputationalBasisStates.clear(); }

    protected:
        std::size_t     seed = 0U;
        std::mt19937_64 mt;

        std::unordered_set<std::size_t>            generatedComputationalBasisStates{};
        std::uniform_int_distribution<std::size_t> random1QBasisDistribution;
    };
} // namespace ec
