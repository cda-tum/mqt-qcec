/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP

#include <string>
#include <memory>

#include "QuantumComputation.hpp"
#include "DDpackage.h"
#include "EquivalenceCheckingResults.hpp"

#define DEBUG_OUTPUT 0

namespace ec {
	enum Direction: bool { LEFT = true, RIGHT=false };

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

		//std::vector<Direction> performedSequence{};

		bool validInstance();

	public:
		EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2):
		qc1(&qc1), qc2(&qc2) {

			dd = std::make_unique<dd::Package>();
			dd->useMatrixNormalization(true);

			filename1 = results.name1 = qc1.getName();
			filename2 = results.name2 = qc2.getName();
			results.name = filename1 + " and " + filename2;

			nqubits1 = results.nqubits1 = qc1.getNqubits();
			nqubits2 = results.nqubits2 = qc2.getNqubits();
			nqubits = results.nqubits = std::max(nqubits1, nqubits2);

			results.ngates1 = qc1.getNindividualOps();
			results.ngates2 = qc2.getNindividualOps();
		}

		virtual ~EquivalenceChecker() = default;

		EquivalenceCheckingResults results;

		const qc::QuantumComputation* getCircuit1() const {
			return qc1;
		}
		const qc::QuantumComputation* getCircuit2() const {
			return qc2;
		}
		const std::unique_ptr<dd::Package>& getDDPackage() const {
			return dd;
		}

		virtual void check();

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

		virtual std::ostream & printCSVEntry(std::ostream& out) {
			return results.printCSVEntry(out);
		}

		virtual std::ostream & printCSVHeader(std::ostream& out) {
			return ec::EquivalenceCheckingResults::printCSVHeader(out);
		}

		virtual bool error() {
			return results.error();
		}

		void exportResultAsDot(const std::string& filename) {
			dd->export2Dot(results.result, filename.c_str());
		}

		/*virtual std::ostream& printSequence(std::ostream& out) {
			for(auto& step: performedSequence) {
				if(step == LEFT)
					out << "\033[32m-> \033[0m";
				else {
					out << "\033[36m<- \033[0m";
				}
			}
			out << '\n';
			return out;
		}*/
	};


}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
