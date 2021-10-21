/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP

#include "CircuitOptimizer.hpp"
#include "Configuration.hpp"
#include "EquivalenceCheckingResults.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <utility>

namespace ec {
    enum Direction : bool { LEFT  = true,
                            RIGHT = false };

    class EquivalenceChecker {
    public:
        EquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration):
            qc1(qc1), qc2(qc2), configuration(configuration) {
            dd           = std::make_unique<dd::Package>(qc1.getNqubits());
            costFunction = [&](const qc::QuantumComputation&, const qc::QuantumComputation&, const std::unique_ptr<qc::Operation>&) { return 1U; };
        };

        EquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, CostFunction costFunction):
            EquivalenceChecker(qc1, qc2, configuration) {
            this->costFunction = std::move(costFunction);
        };
        virtual ~EquivalenceChecker() = default;

        template<class DDType>
        EquivalenceCriterion equals(const DDType& e, const DDType& f) {
            // both node pointers being equivalent is the strongest indication that the two decision diagrams are equivalent
            if (e.p == f.p) {
                // whenever the top edge weights differ, both decision diagrams are only equivalent up to a global phase
                if (!e.w.approximatelyEquals(f.w)) {
                    return EquivalenceCriterion::EquivalentUpToGlobalPhase;
                }
                return EquivalenceCriterion::Equivalent;
            }

            // in general, decision diagrams are canonic. This implies that if their top nodes differ, they are not
            // equivalent. However, numerical instabilities might create a scenario where two nodes differ besides
            // their underlying decision diagrams being extremely close (for some definition of `close`).
            if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
                // for matrices this is resolved by calculating their Frobenius inner product tr(U V^-1)
                // and comparing it to some threshold.
                dd::ComplexValue trace{};
                if (e.p->ident) {
                    trace = dd->trace(f);
                } else if (f.p->ident) {
                    trace = dd->trace(e);
                } else {
                    trace = dd->trace(dd->multiply(e, dd->conjugateTranspose(f)));
                }

                // whenever tr(U V^-1) ≃ 2^n, both decision diagrams should be considered equivalent
                const auto normalizedRealPart = trace.r / std::exp2(e.p->v);
                if (std::abs(normalizedRealPart - 1.0) < configuration.execution.traceThreshold) {
                    return EquivalenceCriterion::Equivalent;
                } else {
                    // whenever |tr(U V^-1)|^2 ≃ 2^n, both decision diagrams should be considered equivalent up to global phase
                    const auto normalizedSquaredMagnitude = (trace.r * trace.r + trace.i * trace.i) / std::exp2(e.p->v);
                    if (std::abs(normalizedSquaredMagnitude - 1.0) < configuration.execution.traceThreshold) {
                        return EquivalenceCriterion::EquivalentUpToGlobalPhase;
                    }
                }
            } else {
                // for vectors this is resolved by computing the inner product (or fidelity) between both decision
                // diagrams and comparing it to some threshold
                const auto innerProduct = dd->innerProduct(e, f);

                // whenever <e,f> ≃ 1, both decision diagrams should be considered equivalent
                if (std::abs(innerProduct.r - 1.) < configuration.simulation.fidelityLimit) {
                    return EquivalenceCriterion::Equivalent;
                }

                // whenever |<e,f>|^2 ≃ 1, both decision diagrams should be considered equivalent up to a phase
                const auto fidelity = innerProduct.r * innerProduct.r + innerProduct.i * innerProduct.i;
                if (std::abs(fidelity - 1.0) < configuration.simulation.fidelityLimit) {
                    return EquivalenceCriterion::EquivalentUpToPhase;
                }
            }

            return EquivalenceCriterion::NotEquivalent;
        }

        virtual EquivalenceCriterion run() = 0;

    protected:
        const qc::QuantumComputation& qc1;
        const qc::QuantumComputation& qc2;

        std::unique_ptr<dd::Package> dd;

        Configuration configuration;
        CostFunction  costFunction;

        double      runtime{};
        std::size_t maxActiveNodes{};

        /// Take operation and apply it either from the left or (inverted) from the right
        /// \param op operation to apply
        /// \param to DD to apply the operation to
        /// \param dir LEFT or RIGHT
        template<class DDType>
        void applyGate(const std::unique_ptr<qc::Operation>& op, DDType& to, qc::Permutation& permutation, Direction dir = LEFT) {
            auto saved = to;
            if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
                // direction has no effect on state vector DDs
                to = dd->multiply(op->getDD(dd, permutation), to);
            } else {
                if (dir == LEFT) {
                    to = dd->multiply(op->getDD(dd, permutation), to);
                } else {
                    to = dd->multiply(to, op->getInverseDD(dd, permutation));
                }
            }
            dd->incRef(to);
            dd->decRef(saved);
            dd->garbageCollect();
        }
    };

} // namespace ec

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
