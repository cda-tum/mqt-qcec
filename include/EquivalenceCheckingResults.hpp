/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP

#include "DDpackage.h"
#include "nlohmann/json.hpp"

#include <iostream>
#include <string>

namespace ec {
    enum class Equivalence {
        NotEquivalent,
        Equivalent,
        NoInformation,
        ProbablyEquivalent,
        EquivalentUpToGlobalPhase
    };

    enum class Method {
        Reference,
        G_I_Gp,
        Simulation
    };

    enum class Strategy {
        Naive,
        Proportional,
        Lookahead,
        CompilationFlow
    };

    enum class StimuliType {
        Classical,
        LocalQuantum,
        GlobalQuantum
    };

    std::string toString(const Method& method);
    std::string toString(const Equivalence& equivalence);
    std::string toString(const Strategy& method);
    std::string toString(const StimuliType& stimuliType);

    struct EquivalenceCheckingResults {
        struct CircuitInfo {
            std::string        name;
            unsigned short     nqubits = 0;
            unsigned long long ngates  = 0;
            dd::Package::CVec  cexOutput{};

            [[nodiscard]] std::string toString() const {
                std::stringstream ss{};
                if (!name.empty()) {
                    ss << name << " ";
                }
                ss << nqubits << " qubits, ";
                ss << ngates << " gates";
                return ss.str();
            }
        };

        CircuitInfo    circuit1{};
        CircuitInfo    circuit2{};
        std::string    name;
        unsigned short nqubits = 0;

        Method      method;
        Strategy    strategy    = Strategy::Proportional;
        StimuliType stimuliType = StimuliType::Classical;

        // results
        Equivalence        equivalence       = Equivalence::NoInformation;
        double             preprocessingTime = 0.0;
        double             verificationTime  = 0.0;
        unsigned long      maxActive         = 0;
        unsigned long long nsims             = 0;
        dd::Package::CVec  cexInput{};
        fp                 fidelity = 0.0;
        dd::Edge           result   = dd::Package::DDzero;

        [[nodiscard]] bool consideredEquivalent() const {
            return equivalence == Equivalence::Equivalent || equivalence == Equivalence::EquivalentUpToGlobalPhase || equivalence == Equivalence::ProbablyEquivalent;
        }

        std::ostream& print(std::ostream& out = std::cout) const;

        static void to_json(nlohmann::json& j, const dd::Package::CVec& stateVector) {
            j = nlohmann::json::array();
            for (const auto& amp: stateVector) {
                j.emplace_back(amp);
            }
        }
        static void from_json(const nlohmann::json& j, dd::Package::CVec& stateVector) {
            for (unsigned long long i = 0; i < j.size(); ++i) {
                stateVector[i] = j.at(i).get<std::pair<fp, fp>>();
            }
        }
        [[nodiscard]] nlohmann::json produceJSON() const;
        [[nodiscard]] std::string    toString() const {
            return produceJSON().dump(2);
        }
        std::ostream& printJSON(std::ostream& out = std::cout) const {
            out << toString() << std::endl;
            return out;
        }

        static std::string getCSVHeader() {
            return "filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;equivalent;t_pre;t_ver;maxActive;method;strategy;nsims;stimuliType";
        }
        static std::ostream& printCSVHeader(std::ostream& out = std::cout) {
            out << getCSVHeader();
            return out;
        }
        [[nodiscard]] std::string produceCSVEntry() const;
        std::ostream&             printCSVEntry(std::ostream& out = std::cout) const {
            out << produceCSVEntry() << std::endl;
            return out;
        }
    };
} // namespace ec

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
