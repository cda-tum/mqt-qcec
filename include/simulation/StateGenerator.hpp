/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_STATEGENERATOR_HPP
#define QCEC_STATEGENERATOR_HPP

#include "StateType.hpp"
#include "TaskManager.hpp"
#include "algorithms/RandomCliffordCircuit.hpp"
#include "dd/Package.hpp"

#include <functional>
#include <random>

namespace ec {
    class StateGenerator {
    public:
        explicit StateGenerator(std::size_t seed = 0);

        qc::VectorDD generateRandomState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0, StateType type = StateType::ComputationalBasis);

        qc::VectorDD generateRandomComputationalBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0);

        qc::VectorDD generateRandom1QBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0);

        qc::VectorDD generateRandomStabilizerState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0);

        void seedGenerator(std::size_t seed);

    protected:
        std::size_t     seed = 0U;
        std::mt19937_64 mt;

        std::unordered_set<std::size_t>            generatedComputationalBasisStates{};
        std::uniform_int_distribution<std::size_t> random1QBasisDistribution;
    };
} // namespace ec

#endif //QCEC_STATEGENERATOR_HPP
