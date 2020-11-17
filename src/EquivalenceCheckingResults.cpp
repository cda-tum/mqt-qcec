/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "EquivalenceCheckingResults.hpp"

namespace ec {

	std::string toString(const Method& method) {
		switch (method) {
			case Reference:
				return "Reference";
			case Naive:
				return "Naive";
			case Proportional:
				return "Proportional";
			case Lookahead:
				return "Lookahead";
			case CompilationFlow:
				return "CompilationFlow";
			case PowerOfSimulation:
				return "PowerOfSimulation";
		}
		return " ";
	}

	std::string toString(const Equivalence& equivalence) {
		switch (equivalence) {
			case NonEquivalent:
				return "NEQ ";
			case Equivalent:
				return "EQ  ";
			case NoInformation:
				return "    ";
			case ProbablyEquivalent:
				return "PEQ ";
			case EquivalentUpToGlobalPhase:
				return "EQGP";
		}
		return " ";
	}

	std::string toString(const StimuliType& stimuliType) {
		switch (stimuliType) {
			case Classical:
				return "classical";
			case LocalQuantum:
				return "local quantum";
			case GlobalQuantum:
				return "global quantum";
		}
		return " ";
	}

	std::ostream& EquivalenceCheckingResults::print(std::ostream& out) {
		if (error()) {
			out << name << ": No equivalence check performed!\n";
			return out;
		}
		out << "[" << time << "]\t";
		if (equivalence == NoInformation) {
			out << "No information on the equivalence of " << name;
		} else if (expected == Equivalent || expected == ProbablyEquivalent || expected == EquivalentUpToGlobalPhase) {
			if (equivalence == Equivalent) {
				out << "Proven " << name << " equivalent";
			} else if (equivalence == NonEquivalent) {
				out << "\033[1;31m[FALSE NEGATIVE]\033[0m Expected " << name << " to be equivalent but shown non-equivalent";
				if (method == PowerOfSimulation) {
					out << " (performed " << nsims << " sims using " << toString(stimuliType) << " stimuli)";
				}
			} else if (equivalence == ProbablyEquivalent) {
				out << "Rightfully suggesting " << name << " to be equivalent (performed " << nsims << " sims using " << toString(stimuliType) << " stimuli)";
			} else if (equivalence == EquivalentUpToGlobalPhase) {
				out << "Proven " << name << " equivalent up to global phase";
			}
		} else if (expected == NonEquivalent) {
			if (equivalence == Equivalent)
				out << "\033[1;31m[FALSE POSITIVE]\033[0m Expected " << name << " to be non-equivalent but showed equivalent";
			else if (equivalence == NonEquivalent) {
				out << "Proven " << name << " non-equivalent";
				if (method == PowerOfSimulation) {
					out << " (performed " << nsims << " sims using " << toString(stimuliType) << " stimuli)";
				}
			} else if (equivalence == ProbablyEquivalent) {
				out << "\033[1;31mWrongfully suggesting " << name << " to be equivalent (performed " << nsims << " sims using " << toString(stimuliType) << " stimuli)\033[0m";
			} else if (equivalence == EquivalentUpToGlobalPhase) {
				out << "\033[1;31m[FALSE POSITIVE]\033[0m Expected " << name << " to be non-equivalent but showed equivalent up to global phase";
			}
		} else if (expected == NoInformation) {
			if (equivalence == Equivalent) {
				out << "Shown " << name << " equivalent";
			} else if (equivalence == NonEquivalent) {
				out << "Shown " << name << " non-equivalent";
				if (method == PowerOfSimulation) {
					out << " (performed " << nsims << " sims using " << toString(stimuliType) << " stimuli)";
				}
			} else if (equivalence == ProbablyEquivalent) {
				out << "Suggesting " << name << " to be-equivalent (performed " << nsims << " sims)";
			} else if (equivalence == EquivalentUpToGlobalPhase) {
				out << "Shown " << name << " equivalent up to global phase";
			}
		}
		out << " with " << toString(method) << " method (and a maximum of " << maxActive << " active nodes)\n";
		return out;
	}

	std::string EquivalenceCheckingResults::produceCSVEntry() {
		if (error())
			return "";
		std::stringstream ss{};
		ss << name1 << ";" << nqubits1 << ";" << ngates1 << ";" << name2 << ";" << nqubits2 << ";" << ngates2 << ";" << toString(expected) << ";" << toString(equivalence) << ";" << toString(method) << ";";
		if (timeout) {
			ss << "TO";
		} else {
			ss << time ;
		}
		ss << ";" << maxActive;
		if (nsims > 0) {
			ss << ";" << nsims;
			ss << ";" << toString(stimuliType);
		} else {
			ss << ";-;-";
		}
		return ss.str();
	}

	nlohmann::json EquivalenceCheckingResults::produceJSON(bool statistics) {
		nlohmann::json resultJSON{};
		resultJSON["circuit1"] = {};
		auto& circuit1 = resultJSON["circuit1"];
		circuit1["name"] = name1;
		circuit1["n_qubits"] = nqubits1;
		circuit1["n_gates"] = ngates1;

		resultJSON["circuit2"] = {};
		auto& circuit2 = resultJSON["circuit2"];
		circuit2["name"] = name2;
		circuit2["n_qubits"] = nqubits2;
		circuit2["n_gates"] = ngates2;

		resultJSON["equivalence"] = equivalence;
		if (statistics) {
			resultJSON["statistics"] = {};
			auto& stats = resultJSON["statistics"];
			if (timeout)
				stats["timeout"] = timeout;
			stats["verification_time"] = time;
			stats["max_nodes"] = maxActive;
			stats["method"] = method;
			if (method == PowerOfSimulation) {
				stats["n_sims"] = nsims;
				stats["stimuliType"] = stimuliType;
			}
		}
		if (method == PowerOfSimulation) {
			if (!cexInput.empty() || !cexOutput1.empty() || !cexOutput2.empty()) {
				resultJSON["verification_cex"] = {};
				if (!cexInput.empty()) {
					to_json(resultJSON["verification_cex"]["input"] ,cexInput);
				}
				if (!cexOutput1.empty()) {
					to_json(resultJSON["verification_cex"]["output1"], cexOutput1);
				}
				if (!cexOutput2.empty()) {
					to_json(resultJSON["verification_cex"]["output2"], cexOutput2);
				}
			}
		}

		return resultJSON;
	}
}
