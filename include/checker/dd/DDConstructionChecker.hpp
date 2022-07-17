//
// This file is part of MQT QCEC library which is released under the MIT license.
// See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
//

#pragma once

#include "DDEquivalenceChecker.hpp"
#include "DDPackageConfigs.hpp"

namespace ec {
    class DDConstructionChecker final: public DDEquivalenceChecker<qc::MatrixDD, ConstructionDDPackage> {
    public:
        DDConstructionChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, ec::Configuration configuration):
            DDEquivalenceChecker(qc1, qc2, std::move(configuration)) {
            if (this->configuration.application.constructionScheme == ApplicationSchemeType::Lookahead) {
                throw std::invalid_argument("Lookahead application scheme must not be used with DD construction checker.");
            }
            initializeApplicationScheme(this->configuration.application.constructionScheme);
        }

        void json(nlohmann::json& j) const noexcept override {
            DDEquivalenceChecker::json(j);
            j["checker"] = "decision_diagram_construction";
        }

    private:
        void initializeTask(TaskManager<qc::MatrixDD, ConstructionDDPackage>& taskManager) override {
            const auto initial = dd->makeIdent(nqubits);
            taskManager.setInternalState(initial);
            taskManager.incRef();
            taskManager.reduceAncillae();
        }
    };
} // namespace ec
