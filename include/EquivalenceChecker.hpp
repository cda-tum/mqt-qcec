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
#include "BaseResults.hpp"

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

		unsigned short nqubits = 0;

		//std::vector<Direction> performedSequence{};

		bool validInstance();

	public:
		EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2):
		qc1(&qc1), qc2(&qc2) {
			filename1 = qc1.getName();
			filename2 = qc2.getName();
			dd = std::make_unique<dd::Package>();
			dd->useMatrixNormalization(true);
			results.name = filename1 + " and " + filename2;
			nqubits = std::max(qc1.getNqubits(), qc2.getNqubits());
		}

		virtual ~EquivalenceChecker() = default;

		BaseResults results;

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
