/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "StateType.hpp"
#include "checker/dd/TaskManager.hpp"
#include "algorithms/RandomCliffordCircuit.hpp"
#include "dd/Package.hpp"

#include <functional>
#include <random>

namespace ec {
    class StateGenerator {
    public:
        explicit StateGenerator(std::size_t seed = 0U);

        qc::VectorDD generateRandomState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U, StateType type = StateType::ComputationalBasis);

        qc::VectorDD generateRandomComputationalBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U);

        qc::VectorDD generateRandom1QBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U);

        qc::VectorDD generateRandomStabilizerState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0U);

        void seedGenerator(std::size_t seed);

    protected:
        std::size_t     seed = 0U;
        std::mt19937_64 mt;

        std::unordered_set<std::size_t>            generatedComputationalBasisStates{};
        std::uniform_int_distribution<std::size_t> random1QBasisDistribution;
    };
} // namespace ec
