/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_IMPROVEDDDEQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_IMPROVEDDDEQUIVALENCECHECKER_HPP

#include <memory>
#include <array>

#include "EquivalenceChecker.hpp"
#include "Operation.hpp"

namespace ec {
	enum Method {Reference, Naive, Proportional, Lookahead};

	class ImprovedDDEquivalenceChecker: public EquivalenceChecker {
		decltype(qc1->begin()) it1;
		decltype(qc2->begin()) it2;
		decltype(qc1->end()) end1;
		decltype(qc1->end()) end2;

		void setName() {
			switch (method) {
				case Reference:
					results.method = "Reference";
					break;
				case Naive:
					results.method = "Naive";
					break;
				case Proportional:
					results.method = "Proportional";
					break;
				case Lookahead:
					results.method = "Lookahead";
					break;
			}
		}

		/// Alternate between LEFT and RIGHT applications
		void checkNaive();
		/// Alternate according to the gate count ratio between LEFT and RIGHT applications
		void checkProportional();
		/// Look-ahead LEFT and RIGHT and choose the more promising option
		void checkLookahead();

	protected:
		std::array<short, qc::MAX_QUBITS> line{};
		Method method = Naive;

		/// Take operation and apply it either from the left or (inverted) from the right
		/// \param op operation to apply
		/// \param to DD to apply the operation to
		/// \param dir LEFT or RIGHT
		void applyGate(std::unique_ptr<qc::Operation>& op, dd::Edge& to, Direction dir = LEFT);

	public:
		ImprovedDDEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, Method method = Proportional):
				EquivalenceChecker(qc1, qc2), method(method){
			line.fill(-1);
			setName();
		}

		Method getMethod() {
			return  method;
		}

		void setMethod(Method m) {
			method = m;
			setName();
		}

		/// Use dedicated method to check the equivalence of both provided circuits
		void check() override;
	};

}


#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_IMPROVEDDDEQUIVALENCECHECKER_HPP
