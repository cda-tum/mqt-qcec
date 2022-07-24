//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"

#include <atomic>
#include <utility>

namespace ec {
class EquivalenceChecker {
public:
  EquivalenceChecker(const qc::QuantumComputation& qc1,
                     const qc::QuantumComputation& qc2,
                     Configuration                 configuration) noexcept
      : qc1(qc1), qc2(qc2),
        nqubits(std::max(qc1.getNqubits(), qc2.getNqubits())),
        configuration(std::move(configuration)){};

  virtual ~EquivalenceChecker() = default;

  virtual EquivalenceCriterion run() = 0;

  [[nodiscard]] EquivalenceCriterion getEquivalence() const noexcept {
    return equivalence;
  }
  [[nodiscard]] double getRuntime() const noexcept { return runtime; }

  virtual void json(nlohmann::json& j) const noexcept;

  void signalDone() { done.store(true, std::memory_order_relaxed); }
  [[nodiscard]] auto isDone() const {
    return done.load(std::memory_order_relaxed);
  }

protected:
  const qc::QuantumComputation& qc1;
  const qc::QuantumComputation& qc2;

  dd::QubitCount nqubits{};

  Configuration configuration;

  EquivalenceCriterion equivalence = EquivalenceCriterion::NoInformation;
  double               runtime{};

private:
  std::atomic<bool> done{false};
};

} // namespace ec
