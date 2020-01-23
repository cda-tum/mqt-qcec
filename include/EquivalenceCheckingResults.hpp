/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP

#include <string>
#include <iostream>

#include "DDpackage.h"

namespace ec {
	enum Equivalence {
		Equivalent, NonEquivalent, NoInformation, ProbablyEquivalent
	};

	enum Method {
		Reference, Naive, Proportional, Lookahead
	};

	static std::string methodToString(const Method method) {
		switch (method) {
			case Reference:
				return "Reference";
			case Naive:
				return "Naive";
			case Proportional:
				return "Proportional";
			case Lookahead:
				return "Lookahead";
		}
		return "Unknown strategy";
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
		dd::Edge result = dd::Package::DDzero;

		virtual ~EquivalenceCheckingResults() = default;

		bool error() {
			return tooManyQubits || differentNrQubits;
		}

		virtual std::ostream& print(std::ostream& out) {
			if (error()) {
				out << name << ": No equivalence check performed!\n";
				return out;
			}
			out << "[" << time << "]\t";
			if (equivalence == NoInformation) {
				out << "No information on the equivalence of " << name;
			} else if (expected == Equivalent || expected == ProbablyEquivalent) {
				if (equivalence == Equivalent)
					out << "Proven " << name << " equivalent";
				else if (equivalence == NonEquivalent)
					out << "[FALSE NEGATIVE] Expected " << name << " to be equivalent but shown non-equivalent";
				else if (equivalence == ProbablyEquivalent)
					out << "Rightfully suggesting " << name << "to be equivalent";
			} else if (expected == NonEquivalent) {
				if (equivalence == Equivalent)
					out << "[FALSE POSITIVE] Expected " << name << " to be non-equivalent but showed equivalent";
				else if (equivalence == NonEquivalent)
					out << "Proven " << name << " non-equivalent";
				else if (equivalence == ProbablyEquivalent)
					out << "Wrongfully suggesting " << name << "to be equivalent";
			} else if (expected == NoInformation) {
				if (equivalence == Equivalent)
					out << "Shown " << name << " equivalent";
				else if (equivalence == NonEquivalent)
					out << "Shown " << name << " non-equivalent";
				else if (equivalence == ProbablyEquivalent)
					out << "Suggesting " << name << " to be-equivalent";
			}
			out << " with " << methodToString(method) << " method (and a maximum of " << maxActive << " active nodes)\n";
			return out;
		}

		static std::ostream& printCSVHeader(std::ostream& out = std::cout) {
			out << "filename1;nqubits1;ngates1;filename2;nqubits2;ngates2;expectedEquivalent;equivalent;method;time;maxActive" << std::endl;
			return out;
		}

		virtual std::ostream& printCSVEntry(std::ostream& out) {
			if (error())
				return out;
			out << name1 << ";" << nqubits1 << ";" << ngates1 << ";" << name2 << ";" << nqubits2 << ";" << ngates2 << ";" << ((expected == Equivalent || expected == NonEquivalent)? std::to_string(expected): " ") << ";" << (equivalence == ec::Equivalent) << ";" << methodToString(method) << ";" << time << ";" << maxActive << std::endl;
			return out;
		}
	};
}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_BASERESULTS_HPP
