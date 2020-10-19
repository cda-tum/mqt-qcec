/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP

#include <string>
#include <iostream>

#include "DDpackage.h"
#include "nlohmann/json.hpp"

namespace ec {
	enum Equivalence {
		NonEquivalent, Equivalent, NoInformation, ProbablyEquivalent, EquivalentUpToGlobalPhase
	};

	// map ec::Equivalence values to JSON as strings
	NLOHMANN_JSON_SERIALIZE_ENUM( Equivalence, {
		{NonEquivalent, "not equivalent"},
		{Equivalent, "equivalent"},
		{EquivalentUpToGlobalPhase, "equivalent up to global phase"},
		{ProbablyEquivalent, "probably equivalent"},
		{NoInformation, "no information"},
	})

	enum Method {
		Reference, Naive, Proportional, Lookahead, CompilationFlow, PowerOfSimulation
	};

	// map ec::Method values to JSON as strings
	NLOHMANN_JSON_SERIALIZE_ENUM( ec::Method, {
		{ec::Reference, "reference"},
		{ec::Naive, "naive"},
		{ec::Proportional, "proportional"},
		{ec::Lookahead, "lookahead"},
		{ec::PowerOfSimulation, "simulation"},
		{ec::CompilationFlow, "compilationflow"}
	})

	static std::string toString(const Method method) {
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

	static std::string toString(const Equivalence equivalence) {
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

	struct EquivalenceCheckingResults {
		std::string name1;
		std::string name2;
		std::string name;
		unsigned long ngates1 = 0;
		unsigned long ngates2 = 0;
		unsigned short nqubits1 = 0;
		unsigned short nqubits2 = 0;
		unsigned short nqubits = 0;

		Method method;

		// error flags
		bool timeout = false;
		bool tooManyQubits = false;
		bool differentNrQubits = false;
		Equivalence expected = NoInformation;

		// results
		Equivalence equivalence = NoInformation;
		double time = 0.0;
		unsigned long maxActive = 0;
		unsigned long long nsims = 0;
		unsigned long long basisState = 0;
		fp fidelity = 0.0;
		dd::Edge result = dd::Package::DDzero;

		virtual ~EquivalenceCheckingResults() = default;

		bool error() const {
			return tooManyQubits || differentNrQubits;
		}

		bool consideredEquivalent() const {
			return equivalence == Equivalent || equivalence == EquivalentUpToGlobalPhase || equivalence == ProbablyEquivalent;
		}

		virtual std::ostream& print() { return print(std::cout); }
		virtual std::ostream& print(std::ostream& out) {
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
						out << " (performed " << nsims << " sims)";
					}
				} else if (equivalence == ProbablyEquivalent) {
					out << "Rightfully suggesting " << name << " to be equivalent (performed " << nsims << " sims)";
				} else if (equivalence == EquivalentUpToGlobalPhase) {
					out << "Proven " << name << " equivalent up to global phase";
				}
			} else if (expected == NonEquivalent) {
				if (equivalence == Equivalent)
					out << "\033[1;31m[FALSE POSITIVE]\033[0m Expected " << name << " to be non-equivalent but showed equivalent";
				else if (equivalence == NonEquivalent) {
					out << "Proven " << name << " non-equivalent";
					if (method == PowerOfSimulation) {
						out << " (performed " << nsims << " sims)";
					}
				} else if (equivalence == ProbablyEquivalent) {
					out << "\033[1;31mWrongfully suggesting " << name << " to be equivalent (performed " << nsims << " sims)\033[0m";
				} else if (equivalence == EquivalentUpToGlobalPhase) {
					out << "\033[1;31m[FALSE POSITIVE]\033[0m Expected " << name << " to be non-equivalent but showed equivalent up to global phase";
				}
			} else if (expected == NoInformation) {
				if (equivalence == Equivalent) {
					out << "Shown " << name << " equivalent";
				} else if (equivalence == NonEquivalent) {
					out << "Shown " << name << " non-equivalent";
					if (method == PowerOfSimulation) {
						out << " (performed " << nsims << " sims)";
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

		virtual std::ostream& printJSON(bool printStatistics) { return printJSON(std::cout, printStatistics); }
		virtual std::ostream& printJSON(std::ostream& out, bool printStatistics) {
			out << "{\n";
			out << "\t\"circuit1\": {\n";
			out << "\t\t\"name\": \"" << name1 << "\",\n";
			out << "\t\t\"n_qubits\": " << nqubits1 << ",\n";
			out << "\t\t\"n_gates\": " << ngates1 << "\n";
			out << "\t},\n";
			out << "\t\"circuit2\": {\n";
			out << "\t\t\"name\": \"" << name2 << "\",\n";
			out << "\t\t\"n_qubits\": " << nqubits2 << ",\n";
			out << "\t\t\"n_gates\": " << ngates2 << "\n";
			out << "\t},\n";
			out << "\t\"equivalence\": \"" << toString(equivalence) << "\"";
			if (printStatistics) {
				out << ",\n\t\"statistics\": {\n";
				out << "\t\t\"verification_time\": " << (timeout? "\"timeout\"": std::to_string(time)) << ",\n";
				out << "\t\t\"max_nodes\": " << maxActive << ",\n";
				out << "\t\t\"method\": \"" << toString(method) << "\"";
				if (method == PowerOfSimulation) {
					out << ",\n\t\t\"n_sims\": " << nsims;
				}
				out << "\n\t}";
			}
			out << "\n}\n";
			return out;
		}

		virtual nlohmann::json produceJSON(bool statistics) {
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
				}
			}

			return resultJSON;
		}

		static std::ostream& printCSVHeader(std::ostream& out = std::cout) {
			out << "filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive;nsims";
			return out;
		}

		virtual std::ostream& printCSVEntry() { return printCSVEntry(std::cout ); }
		virtual std::ostream& printCSVEntry(std::ostream& out) {
			if (error())
				return out;
			out << produceCSVEntry() << std::endl;
			return out;
		}
		virtual std::string produceCSVEntry() {
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
			} else {
				ss << ";-";
			}
			return ss.str();
		}
	};
}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
