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
#include "checker/dd/TaskManager.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/Package_fwd.hpp"

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

  [[nodiscard]] dd::CVec getInitialVector() const {
    return initialState.getVector();
  }
  [[nodiscard]] dd::CVec getInternalVector1() const {
    return taskManager1.getInternalState().getVector();
  }
  [[nodiscard]] dd::CVec getInternalVector2() const {
    return taskManager2.getInternalState().getVector();
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
