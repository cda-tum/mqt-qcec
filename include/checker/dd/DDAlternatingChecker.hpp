//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "DDEquivalenceChecker.hpp"
#include "EquivalenceCriterion.hpp"
#include "dd/Node.hpp"
#include "ir/QuantumComputation.hpp"

#include <nlohmann/json_fwd.hpp>

namespace qc {
class QuantumComputation;
}

namespace ec {
class Configuration;

class DDAlternatingChecker final : public DDEquivalenceChecker<dd::MatrixDD> {
public:
  DDAlternatingChecker(const qc::QuantumComputation& circ1,
                       const qc::QuantumComputation& circ2,
                       Configuration config);

  void json(nlohmann::json& j) const noexcept override;

  /// a function to determine whether the alternating checker can handle
  /// checking both circuits. In particular, it checks whether both circuits
  /// contain non-idle ancillaries.
  static bool canHandle(const qc::QuantumComputation& qc1,
                        const qc::QuantumComputation& qc2);

private:
  dd::MatrixDD functionality{};

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
