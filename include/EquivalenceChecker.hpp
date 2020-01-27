/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP

#include <string>
#include <memory>

#include "QuantumComputation.hpp"
#include "EquivalenceCheckingResults.hpp"

#define DEBUG_MODE_EC 0

namespace ec {
	enum Direction: bool { LEFT = true, RIGHT = false };

	struct Configuration {
		bool augmentQubitRegisters = false;
		bool printCSV = false;
	};

	class EquivalenceChecker {
	protected:
		std::string filename1;
		std::string filename2;

		qc::QuantumComputation* qc1;
		qc::QuantumComputation* qc2;

		std::unique_ptr<dd::Package> dd;

		unsigned short nqubits1 = 0;
		unsigned short nqubits2 = 0;
		unsigned short nqubits = 0;

		bool validInstance();

		// apply swaps 'on' DD in order to change 'from' to 'to'
		// where |from| >= |to|
		void changePermutation(dd::Edge& on, qc::permutationMap& from, const qc::permutationMap& to, std::array<short, qc::MAX_QUBITS>& line, const Direction& dir = LEFT);

		void augmentQubits(qc::QuantumComputation* circuit_to_augment, qc::QuantumComputation* circuit_to_match);

	public:
		EquivalenceCheckingResults results;

		EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2);

		virtual ~EquivalenceChecker() = default;

		// TODO: also allow equivalence by relative phase
		static Equivalence equals(dd::Edge e, dd::Edge f);

		virtual void check(const Configuration& config);

		void expectEquivalent() { results.expected = Equivalent; }
		void expectNonEquivalent() { results.expected = NonEquivalent; }
		void expectNothing() { results.expected = NoInformation; }
		void suggestEquivalent() { results.expected = ProbablyEquivalent; }

		unsigned int getResultingDDSize() {
			return dd->size(results.result) - 1;
		}

		virtual std::ostream& printResult(std::ostream& out) {
			return results.print(out);
		}

		virtual std::ostream & printCSVEntry(std::ostream& out) { return results.printCSVEntry(out); }
		virtual std::ostream & printCSVHeader(std::ostream& out) { return ec::EquivalenceCheckingResults::printCSVHeader(out); }

		virtual bool error() {
			return results.error();
		}

		void exportResultAsDot(const std::string& filename) {
			dd->export2Dot(results.result, filename.c_str());
		}

	};


}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
