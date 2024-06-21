//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "DDEquivalenceChecker.hpp"
#include "DDPackageConfigs.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "applicationscheme/LookaheadApplicationScheme.hpp"
#include "dd/Package_fwd.hpp"

#include <nlohmann/json_fwd.hpp>
#include <utility>

namespace ec {
class DDAlternatingChecker final
    : public DDEquivalenceChecker<qc::MatrixDD, AlternatingDDPackageConfig> {
public:
  DDAlternatingChecker(const qc::QuantumComputation& circ1,
                       const qc::QuantumComputation& circ2,
                       ec::Configuration config)
      : DDEquivalenceChecker(circ1, circ2, std::move(config)) {
    // gates from the second circuit shall be applied "from the right"
    taskManager2.flipDirection();

    initializeApplicationScheme(
        this->configuration.application.alternatingScheme);

    // special treatment for the lookahead application scheme
    if (auto* lookahead = dynamic_cast<
            LookaheadApplicationScheme<AlternatingDDPackageConfig>*>(
            applicationScheme.get())) {
      // initialize links for the internal state and the package of the
      // lookahead scheme
      lookahead->setInternalState(functionality);
      lookahead->setPackage(dd.get());
    }
  }

  void json(nlohmann::json& j) const noexcept override;

  /// a function to determine whether the alternating checker can handle
  /// checking both circuits. In particular, it checks whether both circuits
  /// contain non-idle ancillaries.
  static bool canHandle(const qc::QuantumComputation& qc1,
                        const qc::QuantumComputation& qc2);

private:
  qc::MatrixDD functionality{};

  void initialize() override;
  void execute() override;
  void finish() override;
  void postprocess() override;
  EquivalenceCriterion checkEquivalence() override;

  // at some point this routine should probably make its way into the QFR
  // library
  [[nodiscard]] bool gatesAreIdentical() const;
};
} // namespace ec
