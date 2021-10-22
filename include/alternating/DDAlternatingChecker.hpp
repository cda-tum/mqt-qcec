/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDALTERNATINGCHECKER_HPP
#define QCEC_DDALTERNATINGCHECKER_HPP

#include "AlternatingScheme.hpp"
#include "EquivalenceChecker.hpp"

namespace ec {
    class DDAlternatingChecker: public EquivalenceChecker<qc::MatrixDD> {
    public:
        DDAlternatingChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, const AlternatingScheme& scheme = AlternatingScheme::CostFunction):
            EquivalenceChecker(qc1, qc2, configuration), scheme(scheme) {
            // gates from the second circuit shall be applied "from the right"
            taskManager2.flipDirection();
        }

    protected:
        qc::MatrixDD functionality{};

        AlternatingScheme scheme = AlternatingScheme::CostFunction;

        void                 initialize() override;
        void                 execute() override;
        void                 finish() override;
        void                 postprocess() override;
        EquivalenceCriterion checkEquivalence() override;

    private:
        void executeCostFunction();
        void executeLookahead();
    };
} // namespace ec

#endif //QCEC_DDALTERNATINGCHECKER_HPP
