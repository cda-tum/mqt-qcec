//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "DDEquivalenceChecker.hpp"
#include "DDPackageConfigs.hpp"
#include "EquivalenceCriterion.hpp"
#include "checker/dd/TaskManager.hpp"
#include "dd/Package_fwd.hpp"
#include "ir/QuantumComputation.hpp"

#include <nlohmann/json_fwd.hpp>

namespace ec {
class StateGenerator;

class DDSimulationChecker final
    : public DDEquivalenceChecker<qc::VectorDD, SimulationDDPackageConfig> {
public:
  DDSimulationChecker(const qc::QuantumComputation& circ1,
                      const qc::QuantumComputation& circ2,
                      Configuration configuration);

  void setRandomInitialState(StateGenerator& generator);

  /// Returns the initial state used for simulation
  [[nodiscard]] auto getInitialState() const -> const auto& {
    return initialState;
  }
  /// Returns the internal state of the first task manager
  [[nodiscard]] auto getInternalState1() const -> const auto& {
    return taskManager1.getInternalState();
  }

  /// Returns the internal state of the second task manager
  [[nodiscard]] auto getInternalState2() const -> const auto& {
    return taskManager2.getInternalState();
  }

  void json(nlohmann::basic_json<>& j) const noexcept override;

private:
  // the initial state used for simulation. defaults to the all-zero state
  // |0...0>
  qc::VectorDD initialState{};

  void initializeTask(TaskManager<qc::VectorDD, SimulationDDPackageConfig>&
                          taskManager) override;
  EquivalenceCriterion checkEquivalence() override;
};
} // namespace ec
