/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "simulation/StateGenerator.hpp"

namespace ec {

    StateGenerator::StateGenerator(std::size_t seed) {
        if (seed == 0) {
            std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> random_data{};
            std::random_device                                                    rd;
            std::generate(begin(random_data), end(random_data), std::ref(rd));
            std::seed_seq seeds(begin(random_data), end(random_data));
            mt.seed(seeds);
        } else {
            mt.seed(seed);
        }

        // this generator produces random bases from the set { |0>, |1>, |+>, |->, |L>, |R> }
        random1QBasisDistribution = std::uniform_int_distribution<std::size_t>(0, 5);
    }

    qc::VectorDD StateGenerator::generateRandomState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits, StateType type) {
        switch (type) {
            case ec::StateType::ComputationalBasis:
                return generateRandomComputationalBasisState(dd, totalQubits, ancillaryQubits);
            case ec::StateType::Random1QBasis:
                return generateRandom1QBasisState(dd, totalQubits, ancillaryQubits);
            case ec::StateType::Stabilizer:
                return generateRandomStabilizerState(dd, totalQubits, ancillaryQubits);
        }
        return qc::VectorDD::zero;
    }

    qc::VectorDD StateGenerator::generateRandomComputationalBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits) {
        // determine how many qubits truly are random
        const auto randomQubits = totalQubits - ancillaryQubits;
        if (randomQubits > static_cast<dd::QubitCount>(std::mt19937_64::word_size) - 1) {
            throw std::runtime_error("Generation of computational basis states currently only supports up to 63 qubits.");
        }

        // check if there still is a unique computational basis state
        const std::uint_least64_t maxStates = (static_cast<std::uint_least64_t>(1U) << randomQubits);
        if (generatedComputationalBasisStates.size() == maxStates) {
            throw std::runtime_error("No more unique basis states available.");
        }

        // generate a unique computational basis state
        std::uniform_int_distribution<std::uint_least64_t> distribution(0, maxStates - 1);
        auto [randomState, success] = generatedComputationalBasisStates.insert(distribution(mt));
        while (!success) {
            std::tie(randomState, success) = generatedComputationalBasisStates.insert(distribution(mt));
        }

        // generate the bitvector corresponding to the random state
        std::vector<bool> stimulusBits(totalQubits, false);
        for (dd::QubitCount i = 0; i < randomQubits; ++i) {
            if (*randomState & (static_cast<std::uint_least64_t>(1U) << i)) {
                stimulusBits[i] = true;
            }
        }

        // return the appropriate decision diagram
        return dd->makeBasisState(totalQubits, stimulusBits);
    }

    qc::VectorDD StateGenerator::generateRandom1QBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits) {
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

    qc::VectorDD StateGenerator::generateRandomStabilizerState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits) {
        // determine how many qubits truly are random
        const auto randomQubits = totalQubits - ancillaryQubits;

        // generate a random Clifford circuit with appropriate depth
        auto rcs = qc::RandomCliffordCircuit(randomQubits, static_cast<std::size_t>(std::round(std::log2(randomQubits))), mt());

        // generate the associated stabilizer state by simulating the Clifford circuit
        auto stabilizer = rcs.simulate(dd->makeZeroState(randomQubits), dd);

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

} // namespace ec
