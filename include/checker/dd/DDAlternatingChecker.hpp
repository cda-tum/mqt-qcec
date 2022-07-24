//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "DDEquivalenceChecker.hpp"
#include "DDPackageConfigs.hpp"
#include "applicationscheme/LookaheadApplicationScheme.hpp"

namespace ec {
class DDAlternatingChecker final
    : public DDEquivalenceChecker<qc::MatrixDD, AlternatingDDPackage> {
public:
  DDAlternatingChecker(const qc::QuantumComputation& qc1,
                       const qc::QuantumComputation& qc2,
                       ec::Configuration             configuration)
      : DDEquivalenceChecker(qc1, qc2, std::move(configuration)) {
    // gates from the second circuit shall be applied "from the right"
    taskManager2.flipDirection();

    initializeApplicationScheme(
        this->configuration.application.alternatingScheme);

    // special treatment for the lookahead application scheme
    if (auto* lookahead =
            dynamic_cast<LookaheadApplicationScheme<AlternatingDDPackage>*>(
                applicationScheme.get())) {
      // initialize links for the internal state and the package of the
      // lookahead scheme
      lookahead->setInternalState(functionality);
      lookahead->setPackage(dd.get());
    }
  }

  void json(nlohmann::json& j) const noexcept override {
    DDEquivalenceChecker::json(j);
    j["checker"] = "decision_diagram_alternating";
  }

private:
  qc::MatrixDD functionality{};

  void initializeTask(
      [[maybe_unused]] TaskManager<qc::MatrixDD, AlternatingDDPackage>&
          taskManager) override{
      // task initialization is conducted separately for this checker
  };
  void                 initialize() override;
  void                 execute() override;
  void                 finish() override;
  void                 postprocess() override;
  EquivalenceCriterion checkEquivalence() override;

  // at some point this routine should probably make its way into the QFR
  // library
  [[nodiscard]] bool gatesAreIdentical() const;
};
} // namespace ec
