/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDALTERNATINGCHECKER_HPP
#define QCEC_DDALTERNATINGCHECKER_HPP

#include "AlternatingScheme.hpp"
#include "EquivalenceChecker.hpp"

namespace ec {
    class DDAlternatingChecker: public EquivalenceChecker {
    public:
        DDAlternatingChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, const AlternatingScheme& scheme = AlternatingScheme::CostFunction):
            EquivalenceChecker(qc1, qc2, configuration), scheme(scheme) {}

        EquivalenceCriterion run() override;

    protected:
        struct AlternatingTask {
            const qc::QuantumComputation* qc = nullptr;
            qc::Permutation               permutation{};
            decltype(qc->begin())         iterator;
            decltype(qc->end())           end;
            ec::Direction                 direction = LEFT;

            AlternatingTask() = default;

            explicit AlternatingTask(const qc::QuantumComputation& qc, const ec::Direction& direction):
                qc(&qc), direction(direction) {
                permutation = qc.initialLayout;
                iterator    = qc.begin();
                end         = qc.end();
            }

            [[nodiscard]] bool finished() const { return iterator == end; }
        };

        AlternatingTask task1;
        AlternatingTask task2;

        qc::MatrixDD functionality{};

        AlternatingScheme scheme = AlternatingScheme::CostFunction;

        /// Create the initial matrix used for the G->I<-G' scheme.
        /// [1 0] if the qubit is no ancillary or it is acted upon by both circuits
        /// [0 1]
        ///
        /// [1 0] for an ancillary that is present in one circuit and not acted upon in the other
        /// [0 0]
        /// \return initial matrix
        qc::MatrixDD createInitialMatrix();

        /// Create the goal matrix used for the G->I<-G' scheme.
        /// [1 0] if the qubit is no ancillary
        /// [0 1]
        ///
        /// [1 0] for an ancillary that is present in either circuit
        /// [0 0]
        /// \return goal matrix
        qc::MatrixDD createGoalMatrix();

        void advanceAlternatingScheme(AlternatingTask& task);

        void applyPotentialSwaps(AlternatingTask& task);

        void runCostFunctionCheck();

        void runLookaheadCheck();

        void postprocess();
    };
} // namespace ec

#endif //QCEC_DDALTERNATINGCHECKER_HPP
