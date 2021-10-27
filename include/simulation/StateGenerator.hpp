/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_STATEGENERATOR_HPP
#define QCEC_STATEGENERATOR_HPP

#include "algorithms/RandomCliffordCircuit.hpp"
#include "dd/Package.hpp"

#include <functional>
#include <random>

namespace ec {
    enum class StateType {
        ComputationalBasis,
        Random1QBasis,
        Stabilizer
    };

    std::string toString(const StateType& type) {
        switch (type) {
            case StateType::ComputationalBasis:
                return "computational_basis";
            case StateType::Random1QBasis:
                return "random_1Q_basis";
            case StateType::Stabilizer:
                return "stabilizer";
        }
        return " ";
    }

    StateType stimuliTypeFromString(const std::string& type) {
        if (type == "computational_basis" || type == "0") {
            return StateType::ComputationalBasis;
        } else if (type == "random_1Q_basis" || type == "1") {
            return StateType::Random1QBasis;
        } else if (type == "stabilizer" || type == "2") {
            return StateType::Stabilizer;
        } else {
            throw std::runtime_error("Unknown quantum state type: " + type);
        }
    }

    std::istream& operator>>(std::istream& in, StateType& type) {
        std::string token;
        in >> token;
        type = stimuliTypeFromString(token);
        return in;
    }

    std::ostream& operator<<(std::ostream& out, StateType& type) {
        out << toString(type);
        return out;
    }

    class StateGenerator {
    public:
        explicit StateGenerator(std::size_t seed = 0);

        qc::VectorDD generateRandomState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0, StateType type = StateType::ComputationalBasis);

        qc::VectorDD generateRandomComputationalBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0);

        qc::VectorDD generateRandom1QBasisState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0);

        qc::VectorDD generateRandomStabilizerState(std::unique_ptr<dd::Package>& dd, dd::QubitCount totalQubits, dd::QubitCount ancillaryQubits = 0);

    protected:
        std::size_t     seed = 0U;
        std::mt19937_64 mt;

        std::function<std::size_t()>    computationalBasisStateGenerator;
        std::unordered_set<std::size_t> generatedComputationalBasisStates{};

        std::function<unsigned short()>               random1QBasisGenerator;
        std::uniform_int_distribution<unsigned short> random1QBasisDistribution;
    };
} // namespace ec

#endif //QCEC_STATEGENERATOR_HPP
