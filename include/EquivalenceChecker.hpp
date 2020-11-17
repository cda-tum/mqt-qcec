/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP

#include <string>
#include <memory>
#include <chrono>

#include "QuantumComputation.hpp"
#include "CircuitOptimizer.hpp"
#include "EquivalenceCheckingResults.hpp"

#define DEBUG_MODE_EC 0

namespace ec {
	enum Direction: bool { LEFT = true, RIGHT = false };

	struct Configuration {
		bool printCSV = false;
		bool printStatistics = false;
		fp tolerance = CN::TOLERANCE;

		// configuration options for optimizations
		bool singleQubitGateFusion = false;
		bool swapGateFusion = false;
		bool removeDiagonalGatesBeforeMeasure = false;

		// configuration options for PowerOfSimulation equivalence checker
		double fidelity_limit = 0.999;
		unsigned long long max_sims = 16;
		unsigned long long timeout = 60000;
		StimuliType stimuliType = Classical;
		bool storeCEXinput = false;
		bool storeCEXoutput = false;
	};

	class QCECException : public std::invalid_argument {
		std::string msg;
	public:
		explicit QCECException(std::string  msg) : std::invalid_argument("QCEC Exception"), msg(std::move(msg)) { }

		const char *what() const noexcept override {
			return msg.c_str();
		}
	};

	class EquivalenceChecker {
	protected:
		qc::QuantumComputation& qc1;
		qc::QuantumComputation& qc2;

		std::unique_ptr<dd::Package> dd;

		unsigned short nqubits = 0;

		std::bitset<qc::MAX_QUBITS> ancillary1{};
		std::bitset<qc::MAX_QUBITS> ancillary2{};
		std::bitset<qc::MAX_QUBITS> garbage1{};
		std::bitset<qc::MAX_QUBITS> garbage2{};

		qc::permutationMap initial1;
		qc::permutationMap initial2;
		qc::permutationMap output1;
		qc::permutationMap output2;

		decltype(qc1.begin()) it1;
		decltype(qc2.begin()) it2;
		decltype(qc1.end()) end1;
		decltype(qc1.end()) end2;

		std::array<short, qc::MAX_QUBITS> line{};

		unsigned long long maxSize = 0;
		double average = 0.;
		size_t count = 0;
		std::unordered_set<dd::NodePtr> visited{};
		unsigned long long counter = 0;

		dd::Edge reduceAncillae(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& ancillary, bool regular = true);
		dd::Edge reduceAncillaeRecursion(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& ancillary, unsigned short lowerbound, bool regular = true);
		dd::Edge reduceGarbage(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& garbage, bool regular = true);
		dd::Edge reduceGarbageRecursion(dd::Edge& e, std::bitset<qc::MAX_QUBITS>& garbage, unsigned short lowerbound, bool regular = true);

		/// Take operation and apply it either from the left or (inverted) from the right
		/// \param op operation to apply
		/// \param to DD to apply the operation to
		/// \param dir LEFT or RIGHT
		void applyGate(std::unique_ptr<qc::Operation>& op, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, Direction dir = LEFT);
		void applyGate(decltype(qc1.begin())& opIt, dd::Edge& to, std::map<unsigned short, unsigned short>& permutation, decltype(qc1.end())& end , Direction dir = LEFT);

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

				for (auto& edge : node->p->e) {
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

		static void setTolerance(fp tol) { dd::Package::setComplexNumberTolerance(tol); }

		unsigned int getResultingDDSize() {
			return dd->size(results.result) - 1;
		}

		virtual std::ostream& printResult() { return printResult(std::cout); }
		virtual std::ostream& printResult(std::ostream& out) {
			return results.print(out);
		}

		virtual std::ostream& printJSONResult(bool printStatistics) { return printJSONResult(std::cout, printStatistics); }
		virtual std::ostream& printJSONResult(std::ostream& out, bool printStatistics) {
			return results.printJSON(out, printStatistics);
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
	};


}

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
