//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "ApplicationScheme.hpp"

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
    return {1U, gateRatio};
  }

private:
  [[nodiscard]] std::size_t computeGateRatio() const noexcept {
    const std::size_t size1 = this->taskManager1.getCircuit()->size();
    const std::size_t size2 = this->taskManager2.getCircuit()->size();
    if (size1 == 0U) {
      return size2;
    }
    return std::max((size2 + (size1 / 2U)) / size1,
                    static_cast<std::size_t>(1U));
  }

  std::size_t gateRatio;
};
} // namespace ec
