//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "ApplicationScheme.hpp"
#include "checker/dd/TaskManager.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace ec {
template <class DDType, class Config>
class ProportionalApplicationScheme final
    : public ApplicationScheme<DDType, Config> {
public:
  ProportionalApplicationScheme(TaskManager<DDType, Config>& tm1,
                                TaskManager<DDType, Config>& tm2)
      : ApplicationScheme<DDType, Config>(tm1, tm2),
        gateRatio(computeGateRatio()) {}

  std::pair<size_t, size_t> operator()() noexcept override {
    const std::size_t size1 = this->taskManager1->getCircuit()->size();
    const std::size_t size2 = this->taskManager2->getCircuit()->size();
    if (size1 >= size2) {
      return {gateRatio, 1U};
    }
    return {1U, gateRatio};
  }

private:
  [[nodiscard]] std::size_t computeGateRatio() const noexcept {
    const std::size_t size1 = this->taskManager1->getCircuit()->size();
    const std::size_t size2 = this->taskManager2->getCircuit()->size();

    const auto [min, max] = std::minmax(size1, size2);
    if (min == 0U) {
      return max;
    }
    return std::max((max + (min / 2U)) / min, static_cast<std::size_t>(1U));
  }

  std::size_t gateRatio;
};
} // namespace ec
