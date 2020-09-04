/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP

#include <string>
#include <memory>
#include <chrono>

#include "QuantumComputation.hpp"
#include "EquivalenceCheckingResults.hpp"

#define DEBUG_MODE_EC 0

namespace ec {
	enum Direction: bool { LEFT = true, RIGHT = false };

	struct Configuration {
		bool printCSV = false;

		// configuration options for PowerOfSimulation equivalence checker
		double fidelity_limit = 0.999;
		unsigned long long max_sims = 16;
		unsigned long long timeout = 60000;
	};

	class EquivalenceChecker {
	protected:
		std::string filename1{};
		std::string filename2{};

		qc::QuantumComputation& qc1;
		qc::QuantumComputation& qc2;

		std::unique_ptr<dd::Package> dd;

		unsigned short nqubits1 = 0;
		unsigned short nqubits2 = 0;
		unsigned short nqubits = 0;

		std::bitset<qc::MAX_QUBITS> ancillary1{};
		std::bitset<qc::MAX_QUBITS> ancillary2{};
		std::bitset<qc::MAX_QUBITS> garbage1{};
		std::bitset<qc::MAX_QUBITS> garbage2{};

		qc::permutationMap initial1;
		qc::permutationMap initial2;
		qc::permutationMap output1;
		qc::permutationMap output2;

		std::array<short, qc::MAX_QUBITS> line{};

		unsigned long long maxSize = 0;
		double average = 0.;
		size_t count = 0;
		std::unordered_set<dd::NodePtr> visited{};
		unsigned long long counter = 0;

		dd::Edge reduceAncillae(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& ancillary, bool regular = true);
		dd::Edge reduceGarbage(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& garbage, bool regular = true);

		void addToAverage(unsigned long long x) {
			if (count == 0) {
				average = static_cast<double>(x);
			} else {
				average = (average * count + double(x)) / double(count + 1);
			}
			++count;
		}

		bool validInstance();

		static unsigned int nodecount(dd::Edge& e, std::unordered_set<dd::NodePtr>& v) {
			unsigned int sum = 0;
			std::queue<dd::Edge*> q{};
			q.push(&e);
			while (!q.empty()) {
				auto node = q.front();
				q.pop();
				++sum;
				if (dd::Package::isTerminal(*node))
					continue;

				for (auto edge : node->p->e) {
					if (edge.p != nullptr && v.insert(edge.p).second) {
						q.push(&edge);
					}
				}
			}
			return sum;
		}

	public:
		EquivalenceCheckingResults results{};

		EquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2);

		virtual ~EquivalenceChecker() = default;

		// TODO: also allow equivalence by relative phase or up to a permutation of the outputs
		static Equivalence equals(dd::Edge e, dd::Edge f);

		virtual void check() { check(Configuration{}); };
		virtual void check(const Configuration& config);

		void expectEquivalent() { results.expected = Equivalent; }
		void expectNonEquivalent() { results.expected = NonEquivalent; }
		void expectNothing() { results.expected = NoInformation; }
		void suggestEquivalent() { results.expected = ProbablyEquivalent; }

		unsigned int getResultingDDSize() {
			return dd->size(results.result) - 1;
		}

		virtual std::ostream& printResult() { return printResult(std::cout); }
		virtual std::ostream& printResult(std::ostream& out) {
			return results.print(out);
		}

		virtual std::ostream & printCSVEntry() { return printCSVEntry(std::cout); }
		virtual std::ostream & printCSVEntry(std::ostream& out) { return results.printCSVEntry(out); }
		virtual std::ostream & printCSVHeader() { return printCSVHeader(std::cout); }
		virtual std::ostream & printCSVHeader(std::ostream& out) { return ec::EquivalenceCheckingResults::printCSVHeader(out); }

		virtual bool error() {
			return results.error();
		}

		void exportResultAsDot(const std::string& filename) const {
			dd::export2Dot(results.result, filename);
		}

		static void augmentQubits(qc::QuantumComputation& circuit_to_augment, qc::QuantumComputation& circuit_to_match);
	};


}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
