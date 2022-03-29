/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "DDEquivalenceChecker.hpp"
#include "applicationscheme/LookaheadApplicationScheme.hpp"

namespace ec {
    class DDAlternatingChecker: public DDEquivalenceChecker<qc::MatrixDD, AlternatingDDPackage> {
    public:
        DDAlternatingChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration):
            DDEquivalenceChecker(qc1, qc2, configuration) {
            // gates from the second circuit shall be applied "from the right"
            taskManager2.flipDirection();

            initializeApplicationScheme(this->configuration.application.alternatingScheme);

            // special treatment for the lookahead application scheme
            if (auto lookahead = dynamic_cast<LookaheadApplicationScheme<AlternatingDDPackage>*>(applicationScheme.get())) {
                // initialize links for the internal state and the package of the lookahead scheme
                lookahead->setInternalState(functionality);
                lookahead->setPackage(dd.get());
            }
        }

        void json(nlohmann::json& j) const noexcept override {
            DDEquivalenceChecker::json(j);
            j["checker"] = "decision_diagram_alternating";
        }

    protected:
        qc::MatrixDD functionality{};

        void                 initializeTask(TaskManager<qc::MatrixDD, AlternatingDDPackage>&) override{};
        void                 initialize() override;
        void                 execute() override;
        void                 finish() override;
        void                 postprocess() override;
        EquivalenceCriterion checkEquivalence() override;

        // at some point this routine should probably make its way into the QFR library
        bool gatesAreIdentical();
    };
} // namespace ec
