/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
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

	enum StimuliType {
		Classical, LocalQuantum, GlobalQuantum\
	};

	// map ec::StimuliType values to JSON as strings
	NLOHMANN_JSON_SERIALIZE_ENUM( ec::StimuliType, {
		{ec::Classical, "classical"},
		{ec::LocalQuantum, "localquantum"},
		{ec::GlobalQuantum, "globalquantum"}
	})

	std::string toString(const Method& method);
	std::string toString(const Equivalence& equivalence);
	std::string toString(const StimuliType& stimuliType);

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
		StimuliType stimuliType = Classical;

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
		std::vector<dd::ComplexValue> cexInput{};
		std::vector<dd::ComplexValue> cexOutput1{};
		std::vector<dd::ComplexValue> cexOutput2{};
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
		virtual std::ostream& print(std::ostream& out);

		static void to_json(nlohmann::json& j, std::vector<dd::ComplexValue>& stateVector) {
			j = nlohmann::json::array();
			for (const auto amplitude: stateVector)
				j.emplace_back(std::array<fp, 2>{amplitude.r, amplitude.i});
		}
		static void from_json(const nlohmann::json& j, std::vector<dd::ComplexValue>& stateVector) {
			for (unsigned long long i=0; i<j.size(); ++i) {
				auto amplitude = j.at(i).get<std::pair<fp, fp>>();
				stateVector[i].r = amplitude.first;
				stateVector[i].i = amplitude.second;
			}
		}
		virtual nlohmann::json produceJSON(bool statistics);
		virtual std::ostream& printJSON(bool printStatistics) { return printJSON(std::cout, printStatistics); }
		virtual std::ostream& printJSON(std::ostream& out, bool printStatistics) {
			if (error())
				return out;
			out << produceJSON(printStatistics).dump(2) << std::endl;
			return out;
		}

		static std::ostream& printCSVHeader(std::ostream& out = std::cout) {
			out << "filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive;nsims;stimuliType";
			return out;
		}
		virtual std::string produceCSVEntry();
		virtual std::ostream& printCSVEntry() { return printCSVEntry(std::cout ); }
		virtual std::ostream& printCSVEntry(std::ostream& out) {
			if (error())
				return out;
			out << produceCSVEntry() << std::endl;
			return out;
		}
	};
}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
