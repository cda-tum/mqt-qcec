//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "DDEquivalenceChecker.hpp"
#include "checker/dd/TaskManager.hpp"
#include "dd/Node.hpp"

#include <nlohmann/json_fwd.hpp>

namespace qc {
class QuantumComputation;
}

namespace ec {
class Configuration;

class DDConstructionChecker final : public DDEquivalenceChecker<dd::MatrixDD> {
public:
  DDConstructionChecker(const qc::QuantumComputation& circ1,
                        const qc::QuantumComputation& circ2,
                        Configuration config);

  void json(nlohmann::json& j) const noexcept override;

private:
  void initializeTask(TaskManager<dd::MatrixDD>& taskManager) override;
};
} // namespace ec
