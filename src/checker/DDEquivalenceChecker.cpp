/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "checker/DDEquivalenceChecker.hpp"

namespace ec {

    template<class DDType>
    DDEquivalenceChecker<DDType>::DDEquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration, bool& done):
        EquivalenceChecker(qc1, qc2, configuration, done),
        dd(std::make_unique<dd::Package>(nqubits)),
        taskManager1(TaskManager<DDType>(qc1, dd)),
        taskManager2(TaskManager<DDType>(qc2, dd)) {
    }

    template<class DDType>
    EquivalenceCriterion DDEquivalenceChecker<DDType>::equals(const DDType& e, const DDType& f) {
        // both node pointers being equivalent is the strongest indication that the two decision diagrams are equivalent
        if (e.p == f.p) {
            // whenever the top edge weights differ, both decision diagrams are only equivalent up to a global phase
            if (!e.w.approximatelyEquals(f.w)) {
                if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
                    return EquivalenceCriterion::EquivalentUpToGlobalPhase;
                } else {
                    return EquivalenceCriterion::EquivalentUpToPhase;
                }
            }
            return EquivalenceCriterion::Equivalent;
        }

        // in general, decision diagrams are canonic. This implies that if their top nodes differ, they are not
        // equivalent. However, numerical instabilities might create a scenario where two nodes differ besides
        // their underlying decision diagrams being extremely close (for some definition of `close`).
        if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
            // for matrices this can be resolved by calculating their Frobenius inner product tr(U V^-1) and comparing it to some threshold.
            // in a similar fashion, we can simply compare U V^-1 with the identity, which results in a much simpler check that is not prone to overflow.
            bool isClose{};
            if (e.p->ident) {
                isClose = dd->isCloseToIdentity(f, configuration.functionality.traceThreshold);
            } else if (f.p->ident) {
                isClose = dd->isCloseToIdentity(e, configuration.functionality.traceThreshold);
            } else {
                auto g  = dd->multiply(e, dd->conjugateTranspose(f));
                isClose = dd->isCloseToIdentity(g, configuration.functionality.traceThreshold);
            }

            if (isClose) {
                // whenever the top edge weights differ, both decision diagrams are only equivalent up to a global phase
                if (!e.w.approximatelyEquals(f.w)) {
                    return EquivalenceCriterion::EquivalentUpToGlobalPhase;
                }
                return EquivalenceCriterion::Equivalent;
            }
        } else {
            // for vectors this is resolved by computing the inner product (or fidelity) between both decision
            // diagrams and comparing it to some threshold
            const auto innerProduct = dd->innerProduct(e, f);

            // whenever <e,f> ≃ 1, both decision diagrams should be considered equivalent
            if (std::abs(innerProduct.r - 1.) < configuration.simulation.fidelityThreshold) {
                return EquivalenceCriterion::Equivalent;
            }

            // whenever |<e,f>|^2 ≃ 1, both decision diagrams should be considered equivalent up to a phase
            const auto fidelity = innerProduct.r * innerProduct.r + innerProduct.i * innerProduct.i;
            if (std::abs(fidelity - 1.0) < configuration.simulation.fidelityThreshold) {
                return EquivalenceCriterion::EquivalentUpToPhase;
            }
        }

        return EquivalenceCriterion::NotEquivalent;
    }

    template<class DDType>
    EquivalenceCriterion DDEquivalenceChecker<DDType>::run() {
        const auto start = std::chrono::steady_clock::now();

        // initialize the internal representation (initial state, initial matrix, etc.)
        initialize();

        if (done)
            return equivalence;

        // execute the equivalence checking scheme
        execute();

        if (done)
            return equivalence;

        // finish off both circuits
        finish();

        if (done)
            return equivalence;

        // postprocess the result
        postprocess();

        if (done)
            return equivalence;

        // check the equivalence
        equivalence = checkEquivalence();

        // determine maximum number of nodes used
        if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
            maxActiveNodes = dd->mUniqueTable.getMaxActiveNodes();
        } else if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
            maxActiveNodes = dd->vUniqueTable.getMaxActiveNodes();
        }

        const auto end = std::chrono::steady_clock::now();
        runtime += std::chrono::duration<double>(end - start).count();

        return equivalence;
    }

    template<class DDType>
    void DDEquivalenceChecker<DDType>::initialize() {
        initializeTask(taskManager1);
        initializeTask(taskManager2);
    }

    template<class DDType>
    void DDEquivalenceChecker<DDType>::execute() {
        while (!taskManager1.finished() && !taskManager2.finished() && !done) {
            // skip over any SWAP operations
            taskManager1.applySwapOperations();
            taskManager2.applySwapOperations();

            if (!taskManager1.finished() && !taskManager2.finished()) {
                // query application scheme on how to proceed
                const auto [apply1, apply2] = (*applicationScheme)();

                // advance both tasks correspondingly
                if (done) return;
                taskManager1.advance(apply1);
                if (done) return;
                taskManager2.advance(apply2);
            }
        }
    }

    template<class DDType>
    void DDEquivalenceChecker<DDType>::finish() {
        taskManager1.finish();
        if (done) return;
        taskManager2.finish();
    }

    template<class DDType>
    void DDEquivalenceChecker<DDType>::postprocessTask(TaskManager<DDType>& task) {
        // ensure that the permutation that was tracked throughout the circuit matches the expected output permutation
        task.changePermutation();
        if (done) return;
        // eliminate the superfluous contributions of ancillary qubits (this only has an effect on matrices)
        task.reduceAncillae();
        if (done) return;
        // sum up the contributions of garbage qubits
        task.reduceGarbage();
    }

    template<class DDType>
    void DDEquivalenceChecker<DDType>::postprocess() {
        postprocessTask(taskManager1);
        if (done) return;
        postprocessTask(taskManager2);
    }

    template<class DDType>
    EquivalenceCriterion DDEquivalenceChecker<DDType>::checkEquivalence() {
        return equals(taskManager1.getInternalState(), taskManager2.getInternalState());
    }

    template<class DDType>
    void DDEquivalenceChecker<DDType>::initializeApplicationScheme(ApplicationSchemeType scheme) {
        switch (scheme) {
            case ApplicationSchemeType::Sequential:
                applicationScheme = std::make_unique<SequentialApplicationScheme<DDType>>(taskManager1, taskManager2);
                break;
            case ApplicationSchemeType::OneToOne:
                applicationScheme = std::make_unique<OneToOneApplicationScheme<DDType>>(taskManager1, taskManager2);
                break;
            case ApplicationSchemeType::Proportional:
                applicationScheme = std::make_unique<ProportionalApplicationScheme<DDType>>(taskManager1, taskManager2);
                break;
            case ApplicationSchemeType::Lookahead:
                if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
                    applicationScheme = std::make_unique<LookaheadApplicationScheme>(taskManager1, taskManager2);
                } else {
                    throw std::runtime_error("Lookahead application scheme can only be used for matrices.");
                }
                break;
            case ApplicationSchemeType::GateCost:
                if (!configuration.application.profile.empty()) {
                    applicationScheme = std::make_unique<GateCostApplicationScheme<DDType>>(taskManager1, taskManager2, configuration.application.profile);
                } else {
                    applicationScheme = std::make_unique<GateCostApplicationScheme<DDType>>(taskManager1, taskManager2, configuration.application.costFunction);
                }
                break;
        }
    }

    template class DDEquivalenceChecker<qc::VectorDD>;
    template class DDEquivalenceChecker<qc::MatrixDD>;
} // namespace ec