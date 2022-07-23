//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "DDEquivalenceChecker.hpp"
#include "DDPackageConfigs.hpp"

namespace ec {
class DDSimulationChecker final
    : public DDEquivalenceChecker<qc::VectorDD, SimulationDDPackage> {
public:
  DDSimulationChecker(const qc::QuantumComputation& qc1,
                      const qc::QuantumComputation& qc2,
                      Configuration                 configuration) noexcept;

  void setRandomInitialState(StateGenerator& generator);

  [[nodiscard]] dd::CVec getInitialVector() const {
    return dd->getVector(initialState);
  }
  [[nodiscard]] dd::CVec getInternalVector1() const {
    return dd->getVector(taskManager1.getInternalState());
  }
  [[nodiscard]] dd::CVec getInternalVector2() const {
    return dd->getVector(taskManager2.getInternalState());
  }

  void json(nlohmann::json& j) const noexcept override {
    DDEquivalenceChecker::json(j);
    j["checker"] = "decision_diagram_simulation";
  }

private:
  // the initial state used for simulation. defaults to the all-zero state
  // |0...0>
  qc::VectorDD initialState{};

  void initializeTask(
      TaskManager<qc::VectorDD, SimulationDDPackage>& taskManager) override;
  EquivalenceCriterion checkEquivalence() override;
};
} // namespace ec
