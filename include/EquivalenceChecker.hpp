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
#include "TaskManager.hpp"
#include "costfunction/CostFunction.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <utility>

namespace ec {
    template<class DDType>
    class EquivalenceChecker {
    public:
        EquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration):
            qc1(qc1), qc2(qc2),
            nqubits(std::max(qc1.getNqubits(), qc2.getNqubits())),
            dd(std::make_unique<dd::Package>(nqubits)),
            taskManager1(TaskManager<DDType>(qc1, dd)),
            taskManager2(TaskManager<DDType>(qc1, dd)),
            configuration(configuration),
            costFunction(qc1, qc2, configuration.execution.costFunctionType){};

        virtual ~EquivalenceChecker() = default;

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

        virtual EquivalenceCriterion run() {
            const auto start = std::chrono::steady_clock::now();

            // initialize the internal representation (initial state, initial matrix, etc.)
            initialize();

            // execute the equivalence checking scheme
            execute();

            // finish off both circuits
            finish();

            // postprocess the result
            postprocess();

            // check the equivalence
            const auto equivalence = checkEquivalence();

            // determine maximum number of nodes used
            if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
                maxActiveNodes = dd->mUniqueTable.getMaxActiveNodes();
            } else if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
                maxActiveNodes = dd->vUniqueTable.getMaxActiveNodes();
            }

            const auto end = std::chrono::steady_clock::now();
            runtime        = std::chrono::duration<double>(end - start).count();

            return equivalence;
        }

    protected:
        const qc::QuantumComputation& qc1;
        const qc::QuantumComputation& qc2;

        dd::QubitCount nqubits{};

        std::unique_ptr<dd::Package> dd;

        TaskManager<DDType> taskManager1;
        TaskManager<DDType> taskManager2;

        Configuration configuration;
        CostFunction  costFunction;

        double      runtime{};
        std::size_t maxActiveNodes{};

        virtual void initializeTask(TaskManager<DDType>&){};
        virtual void initialize() {
            initializeTask(taskManager1);
            initializeTask(taskManager2);
        }
        virtual void execute() {
            while (!taskManager1.finished() && !taskManager2.finished()) {
                // skip over any SWAP operations
                taskManager1.applySwapOperations();
                taskManager2.applySwapOperations();

                if (!taskManager1.finished() && !taskManager2.finished()) {
                    // determine cost of either gate
                    const auto cost1 = costFunction(taskManager1(), LEFT);
                    const auto cost2 = costFunction(taskManager2(), RIGHT);

                    // advance both tasks correspondingly
                    taskManager1.advance(cost2);
                    taskManager2.advance(cost1);
                }
            }
        }
        virtual void finish() {
            taskManager1.finish();
            taskManager2.finish();
        }
        virtual void postprocessTask(TaskManager<DDType>& task) {
            // ensure that the permutation that was tracked throughout the circuit matches the expected output permutation
            task.changePermutation();

            // eliminate the superfluous contributions of ancillary qubits (this only has an effect on matrices)
            task.reduceAncillae();

            // sum up the contributions of garbage qubits
            task.reduceGarbage();
        }
        virtual void postprocess() {
            postprocessTask(taskManager1);
            postprocessTask(taskManager2);
        }
        virtual EquivalenceCriterion checkEquivalence() {
            return equals(taskManager1.getInternalState(), taskManager2.getInternalState());
        }
    };

} // namespace ec

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
