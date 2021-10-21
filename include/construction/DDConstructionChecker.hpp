/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDCONSTRUCTIONCHECKER_HPP
#define QCEC_DDCONSTRUCTIONCHECKER_HPP

#include "EquivalenceChecker.hpp"

namespace ec {
    class DDConstructionChecker: public EquivalenceChecker {
    public:
        // in order to enable better operation caching in the decision diagram package the decision diagrams for both
        // circuits are not built in sequence but rather in parallel. To this end, a cost function is used to specify
        // how many gates of either circuit are to be applied for any gate of the other circuit
        EquivalenceCriterion run() override;

    protected:
        struct ConstructionTask {
            const qc::QuantumComputation* qc;
            qc::Permutation               permutation{};
            decltype(qc->begin())         iterator;
            decltype(qc->end())           end;
            qc::MatrixDD                  functionality{};

            explicit ConstructionTask(const qc::QuantumComputation& qc):
                qc(&qc) {
                permutation = qc.initialLayout;
                iterator    = qc.begin();
                end         = qc.end();
            }

            [[nodiscard]] bool finished() const { return iterator == end; }
        };

        ConstructionTask task1;
        ConstructionTask task2;

        void setupConstructionTask(const qc::QuantumComputation& qc, ConstructionTask& task);

        void advanceConstruction(ConstructionTask& task);

        void applyPotentialSwaps(ConstructionTask& task);

        void postprocess(ConstructionTask& task);
    };
} // namespace ec

#endif //QCEC_DDCONSTRUCTIONCHECKER_HPP
