//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/DDConstructionChecker.hpp"

#include "Configuration.hpp"
#include "checker/dd/DDEquivalenceChecker.hpp"
#include "checker/dd/DDPackageConfigs.hpp"
#include "checker/dd/TaskManager.hpp"
#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "dd/Node.hpp"
#include "dd/Package.hpp"
#include "ir/QuantumComputation.hpp"

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <utility>

ec::DDConstructionChecker::DDConstructionChecker(
    const qc::QuantumComputation& circ1, const qc::QuantumComputation& circ2,
    Configuration config)
    : DDEquivalenceChecker(circ1, circ2, std::move(config),
                           ConstructionDDPackageConfig{}) {
  if (configuration.application.constructionScheme ==
      ApplicationSchemeType::Lookahead) {
    throw std::invalid_argument("Lookahead application scheme must not be "
                                "used with DD construction checker.");
  }
  initializeApplicationScheme(configuration.application.constructionScheme);
}

// this function is mainly placed here in order to have an out-of-line
// virtual method definition which avoids emitting the classes' vtable in
// every translation unit.
void ec::DDConstructionChecker::json(nlohmann::basic_json<>& j) const noexcept {
  DDEquivalenceChecker::json(j);
  j["checker"] = "decision_diagram_construction";
}

void ec::DDConstructionChecker::initializeTask(
    TaskManager<dd::MatrixDD>& taskManager) {
  DDEquivalenceChecker::initializeTask(taskManager);
  const auto initial = dd::Package::makeIdent();
  taskManager.setInternalState(initial);
  taskManager.incRef();
  taskManager.reduceAncillae();
}
