/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include <iostream>

namespace ec {
    enum class StateType {
        ComputationalBasis = 0,
        Random1QBasis      = 1,
        Stabilizer         = 2
    };

    inline std::string toString(const StateType& type) noexcept {
        switch (type) {
            case StateType::Random1QBasis:
                return "random_1Q_basis";
            case StateType::Stabilizer:
                return "stabilizer";
            case StateType::ComputationalBasis:
            default:
                return "computational_basis";
        }
        return " ";
    }

    inline StateType stateTypeFromString(const std::string& type) {
        if (type == "computational_basis" || type == "0" || type == "classical") {
            return StateType::ComputationalBasis;
        } else if (type == "random_1Q_basis" || type == "1" || type == "local_quantum") {
            return StateType::Random1QBasis;
        } else if (type == "stabilizer" || type == "2" || type == "global_quantum") {
            return StateType::Stabilizer;
        } else {
            throw std::runtime_error("Unknown quantum state type: " + type);
        }
    }

    inline std::istream& operator>>(std::istream& in, StateType& type) {
        std::string token;
        in >> token;

        if (token.empty()) {
            in.setstate(std::istream::failbit);
            return in;
        }

        type = stateTypeFromString(token);
        return in;
    }

    inline std::ostream& operator<<(std::ostream& out, StateType& type) {
        out << toString(type);
        return out;
    }
} // namespace ec
