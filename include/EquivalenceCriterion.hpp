/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include <exception>
#include <iostream>

namespace ec {
    enum class EquivalenceCriterion {
        NotEquivalent             = 0,
        Equivalent                = 1,
        NoInformation             = 2,
        ProbablyEquivalent        = 3,
        EquivalentUpToGlobalPhase = 4,
        EquivalentUpToPhase       = 5
    };

    inline std::string toString(const EquivalenceCriterion& criterion) noexcept {
        switch (criterion) {
            case EquivalenceCriterion::NotEquivalent:
                return "not_equivalent";
            case EquivalenceCriterion::Equivalent:
                return "equivalent";
            case EquivalenceCriterion::ProbablyEquivalent:
                return "probably_equivalent";
            case EquivalenceCriterion::EquivalentUpToGlobalPhase:
                return "equivalent_up_to_global_phase";
            case EquivalenceCriterion::EquivalentUpToPhase:
                return "equivalent_up_to_phase";
            case EquivalenceCriterion::NoInformation:
            default:
                return "no_information";
        }
    }

    inline EquivalenceCriterion fromString(const std::string& criterion) {
        if (criterion == "not_equivalent" || criterion == "0") {
            return EquivalenceCriterion::NotEquivalent;
        } else if (criterion == "equivalent" || criterion == "1") {
            return EquivalenceCriterion::Equivalent;
        } else if (criterion == "probably_equivalent" || criterion == "2") {
            return EquivalenceCriterion::ProbablyEquivalent;
        } else if (criterion == "equivalent_up_to_global_phase" || criterion == "3") {
            return EquivalenceCriterion::EquivalentUpToGlobalPhase;
        } else if (criterion == "equivalent_up_to_phase" || criterion == "4") {
            return EquivalenceCriterion::EquivalentUpToPhase;
        } else if (criterion == "no_information" || criterion == "5") {
            return EquivalenceCriterion::NoInformation;
        } else {
            throw std::runtime_error("Unknown equivalence criterion: " + criterion);
        }
    }

    inline std::istream& operator>>(std::istream& in, EquivalenceCriterion& criterion) {
        std::string token;
        in >> token;

        if (token.empty()) {
            in.setstate(std::istream::failbit);
            return in;
        }

        criterion = fromString(token);
        return in;
    }

    inline std::ostream& operator<<(std::ostream& out, EquivalenceCriterion& criterion) {
        out << toString(criterion);
        return out;
    }
} // namespace ec
