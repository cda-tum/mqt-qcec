/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "EquivalenceCheckingResults.hpp"

namespace ec {

    std::string toString(const Method& method) {
        switch (method) {
            case Method::Reference:
                return "reference";
            case Method::G_I_Gp:
                return "G -> I <- G'";
            case Method::Simulation:
                return "simulation";
        }
        return " ";
    }

    std::string toString(const Equivalence& equivalence) {
        switch (equivalence) {
            case Equivalence::NotEquivalent:
                return "not equivalent";
            case Equivalence::Equivalent:
                return "equivalent";
            case Equivalence::NoInformation:
                return "no information";
            case Equivalence::ProbablyEquivalent:
                return "probably equivalent";
            case Equivalence::EquivalentUpToGlobalPhase:
                return "equivalent up to global phase";
        }
        return " ";
    }

    std::string toString(const Strategy& strategy) {
        switch (strategy) {
            case Strategy::Naive:
                return "naive";
            case Strategy::Proportional:
                return "proportional";
            case Strategy::Lookahead:
                return "lookahead";
            case Strategy::CompilationFlow:
                return "compilation flow";
        }
        return " ";
    }

    std::string toString(const StimuliType& stimuliType) {
        switch (stimuliType) {
            case StimuliType::Classical:
                return "classical";
            case StimuliType::LocalQuantum:
                return "local quantum";
            case StimuliType::GlobalQuantum:
                return "global quantum";
        }
        return " ";
    }

    std::ostream& EquivalenceCheckingResults::print(std::ostream& out) const {
        out << "[" << verificationTime;
        if (preprocessingTime > 1e-4) {
            out << " (+" << preprocessingTime << ")";
        }
        out << "]\t";
        if (equivalence == Equivalence::NoInformation) {
            out << "No information on the equivalence of " << name;
        } else if (equivalence == Equivalence::Equivalent) {
            out << "Shown " << name << " equivalent";
        } else if (equivalence == Equivalence::NotEquivalent) {
            out << "Shown " << name << " non-equivalent";
            if (method == Method::Simulation) {
                out << " (performed " << nsims << " sims using " << ec::toString(stimuliType) << " stimuli)";
            }
        } else if (equivalence == Equivalence::ProbablyEquivalent) {
            out << "Suggesting " << name << " to be-equivalent (performed " << nsims << " sims)";
        } else if (equivalence == Equivalence::EquivalentUpToGlobalPhase) {
            out << "Shown " << name << " equivalent up to global phase";
        }
        out << " with the " << ec::toString(method) << " method (";
        if (method == Method::G_I_Gp) {
            out << "using the " << ec::toString(strategy) << " strategy ";
        }
        out << "and a maximum of " << maxActive << " active nodes)\n";
        return out;
    }

    std::string EquivalenceCheckingResults::produceCSVEntry() const {
        std::stringstream ss{};
        ss << circuit1.name << ";" << circuit1.nqubits << ";" << circuit1.ngates << ";" << circuit2.name << ";" << circuit2.nqubits << ";" << circuit2.ngates << ";" << ec::toString(equivalence) << ";" << preprocessingTime << ";" << verificationTime << ";" << maxActive << ";" << ec::toString(method) << ";";
        if (method == Method::G_I_Gp) {
            ss << ec::toString(strategy);
        }
        if (nsims > 0) {
            ss << ";" << nsims;
            ss << ";" << ec::toString(stimuliType);
        } else {
            ss << ";;";
        }
        return ss.str();
    }

    nlohmann::json EquivalenceCheckingResults::produceJSON() const {
        nlohmann::json resultJSON{};
        resultJSON["circuit1"] = {};
        auto& circ1            = resultJSON["circuit1"];
        circ1["name"]          = circuit1.name;
        circ1["n_qubits"]      = circuit1.nqubits;
        circ1["n_gates"]       = circuit1.ngates;

        resultJSON["circuit2"] = {};
        auto& circ2            = resultJSON["circuit2"];
        circ2["name"]          = circuit2.name;
        circ2["n_qubits"]      = circuit2.nqubits;
        circ2["n_gates"]       = circuit2.ngates;

        resultJSON["equivalence"]   = ec::toString(equivalence);
        resultJSON["statistics"]    = {};
        auto& stats                 = resultJSON["statistics"];
        stats["preprocessing_time"] = preprocessingTime;
        stats["verification_time"]  = verificationTime;
        stats["max_nodes"]          = maxActive;
        stats["method"]             = ec::toString(method);
        if (method == Method::Simulation) {
            stats["n_sims"]       = nsims;
            stats["stimuli_type"] = ec::toString(stimuliType);
        } else if (method == Method::G_I_Gp) {
            stats["strategy"] = ec::toString(strategy);
        }

        if (method == Method::Simulation) {
            if (!cexInput.empty() || !circuit1.cexOutput.empty() || !circuit2.cexOutput.empty()) {
                resultJSON["verification_cex"] = {};
                if (!cexInput.empty()) {
                    to_json(resultJSON["verification_cex"]["input"], cexInput);
                }
                if (!circuit1.cexOutput.empty()) {
                    to_json(resultJSON["verification_cex"]["output1"], circuit1.cexOutput);
                }
                if (!circuit2.cexOutput.empty()) {
                    to_json(resultJSON["verification_cex"]["output2"], circuit2.cexOutput);
                }
            }
        }

        return resultJSON;
    }
} // namespace ec
