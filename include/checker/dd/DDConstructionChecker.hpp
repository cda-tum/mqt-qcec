//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "DDEquivalenceChecker.hpp"
#include "DDPackageConfigs.hpp"
#include "QuantumComputation.hpp"
#include "checker/dd/TaskManager.hpp"
#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "dd/Package_fwd.hpp"

#include <nlohmann/json_fwd.hpp>
#include <stdexcept>
#include <utility>

namespace ec {
class DDConstructionChecker final
    : public DDEquivalenceChecker<qc::MatrixDD, ConstructionDDPackageConfig> {
public:
  DDConstructionChecker(const qc::QuantumComputation& circ1,
                        const qc::QuantumComputation& circ2,
                        ec::Configuration config)
      : DDEquivalenceChecker(circ1, circ2, std::move(config)) {
    if (this->configuration.application.constructionScheme ==
        ApplicationSchemeType::Lookahead) {
      throw std::invalid_argument("Lookahead application scheme must not be "
                                  "used with DD construction checker.");
    }
    initializeApplicationScheme(
        this->configuration.application.constructionScheme);
  }

  void json(nlohmann::json& j) const noexcept override;

private:
  void initializeTask(TaskManager<qc::MatrixDD, ConstructionDDPackageConfig>&
                          taskManager) override {
    DDEquivalenceChecker::initializeTask(taskManager);
    const auto initial = dd->makeIdent();
    taskManager.setInternalState(initial);
    taskManager.incRef();
    taskManager.reduceAncillae();
  }
};
} // namespace ec
