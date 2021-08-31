/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP

#include "Definitions.hpp"
#include "dd/Package.hpp"
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
            dd::CVec           cexOutput{};

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
        dd::QubitCount nqubits = 0;

        Method      method{};
        Strategy    strategy    = Strategy::Proportional;
        StimuliType stimuliType = StimuliType::Classical;

        // results
        Equivalence  equivalence       = Equivalence::NoInformation;
        double       preprocessingTime = 0.0;
        double       verificationTime  = 0.0;
        std::size_t  maxActive         = 0;
        std::size_t  nsims             = 0;
        dd::CVec     cexInput{};
        dd::fp       fidelity = 0.0;
        qc::MatrixDD result   = qc::MatrixDD::zero;

        [[nodiscard]] bool consideredEquivalent() const {
            return equivalence == Equivalence::Equivalent || equivalence == Equivalence::EquivalentUpToGlobalPhase || equivalence == Equivalence::ProbablyEquivalent;
        }

        std::ostream& print(std::ostream& out = std::cout) const;

        static void to_json(nlohmann::json& j, const dd::CVec& stateVector) {
            j = nlohmann::json::array();
            for (const auto& amp: stateVector) {
                j.emplace_back(std::pair{amp.real(), amp.imag()});
            }
        }
        static void from_json(const nlohmann::json& j, dd::CVec& stateVector) {
            for (std::size_t i = 0; i < j.size(); ++i) {
                const auto& c  = j.at(i).get<std::pair<dd::fp, dd::fp>>();
                stateVector[i] = std::complex<dd::fp>(c.first, c.second);
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
