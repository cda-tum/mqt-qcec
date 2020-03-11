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
		bool trackMaxDDsize = false;

		// configuration options for PowerOfSimulation equivalence checker
		double fidelity_limit = 0.999;
		unsigned long long max_sims = 16;
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

		unsigned long long maxSize = 0;
		double average = 0.;
		size_t count = 0;
		std::unordered_set<dd::NodePtr> visited{};
		unsigned long long counter = 0;

		void addToAverage(unsigned long long x) {
			if (count == 0) {
				average = count;
			} else {
				average = (average * count + double(x)) / double(count + 1);
			}
			++count;
		}

		bool validInstance();

		unsigned long long nodecount(const dd::Edge& e, std::unordered_set<dd::NodePtr>& v) {
			v.insert(e.p);
			unsigned long long sum = 1;
			if(!dd->isTerminal(e)) {
				for (const auto& edge: e.p->e) {
					if (edge.p != nullptr && !v.count(edge.p)) {
						sum += nodecount(edge, v);
					}
				}
			}
			return sum;
		}

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

		static void augmentQubits(qc::QuantumComputation* circuit_to_augment, qc::QuantumComputation* circuit_to_match);
	};


}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
