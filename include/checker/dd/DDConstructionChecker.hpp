/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "DDEquivalenceChecker.hpp"

namespace ec {
    class DDConstructionChecker: public DDEquivalenceChecker<qc::MatrixDD, ConstructionDDPackage> {
    public:
        DDConstructionChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration):
            DDEquivalenceChecker(qc1, qc2, configuration) {
            if (this->configuration.application.constructionScheme == ApplicationSchemeType::Lookahead) {
                throw std::invalid_argument("Lookahead application scheme must not be used with DD construction checker.");
            }
            initializeApplicationScheme(this->configuration.application.constructionScheme);
        }

        void json(nlohmann::json& j) const noexcept override {
            DDEquivalenceChecker::json(j);
            j["checker"] = "decision_diagram_construction";
        }

    protected:
        void initializeTask(TaskManager<qc::MatrixDD, ConstructionDDPackage>& task) override {
            const auto initial = dd->makeIdent(nqubits);
            task.setInternalState(initial);
            task.incRef();
            task.reduceAncillae();
        }
    };
} // namespace ec
