/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include <exception>
#include <iostream>

namespace ec {
    enum class EquivalenceCriterion {
        NotEquivalent,
        Equivalent,
        NoInformation,
        ProbablyEquivalent,
        EquivalentUpToGlobalPhase,
        EquivalentUpToPhase
    };

    inline std::string toString(const EquivalenceCriterion& criterion) {
        switch (criterion) {
            case EquivalenceCriterion::NotEquivalent:
                return "not_equivalent";
            case EquivalenceCriterion::Equivalent:
                return "equivalent";
            case EquivalenceCriterion::NoInformation:
                return "no_information";
            case EquivalenceCriterion::ProbablyEquivalent:
                return "probably_equivalent";
            case EquivalenceCriterion::EquivalentUpToGlobalPhase:
                return "equivalent_up_to_global_phase";
            case EquivalenceCriterion::EquivalentUpToPhase:
                return "equivalent_up_to_phase";
        }
        return " ";
    }

    inline EquivalenceCriterion fromString(const std::string& criterion) {
        if (criterion == "not_equivalent" || criterion == "0") {
            return EquivalenceCriterion::NotEquivalent;
        } else if (criterion == "equivalent" || criterion == "1") {
            return EquivalenceCriterion::Equivalent;
        } else if (criterion == "no_information" || criterion == "2") {
            return EquivalenceCriterion::NoInformation;
        } else if (criterion == "probably_equivalent" || criterion == "3") {
            return EquivalenceCriterion::ProbablyEquivalent;
        } else if (criterion == "equivalent_up_to_global_phase" || criterion == "3") {
            return EquivalenceCriterion::EquivalentUpToGlobalPhase;
        } else if (criterion == "equivalent_up_to_phase" || criterion == "4") {
            return EquivalenceCriterion::EquivalentUpToPhase;
        } else {
            throw std::runtime_error("Unknown equivalence criterion: " + criterion);
        }
    }

    inline std::istream& operator>>(std::istream& in, EquivalenceCriterion& criterion) {
        std::string token;
        in >> token;
        criterion = fromString(token);
        return in;
    }

    inline std::ostream& operator<<(std::ostream& out, EquivalenceCriterion& criterion) {
        out << toString(criterion);
        return out;
    }
} // namespace ec
