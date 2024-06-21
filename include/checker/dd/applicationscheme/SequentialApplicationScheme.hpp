//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "ApplicationScheme.hpp"
#include "checker/dd/TaskManager.hpp"

#include <cstddef>
#include <utility>

namespace ec {
template <class DDType, class Config>
class SequentialApplicationScheme final
    : public ApplicationScheme<DDType, Config> {
public:
  SequentialApplicationScheme(TaskManager<DDType, Config>& tm1,
                              TaskManager<DDType, Config>& tm2) noexcept
      : ApplicationScheme<DDType, Config>(tm1, tm2),
        gates1(tm1.getCircuit()->getNops()),
        gates2(tm2.getCircuit()->getNops()) {}

  std::pair<size_t, size_t> operator()() noexcept override {
    return {gates1, gates2};
  }

private:
  std::size_t gates1;
  std::size_t gates2;
};
} // namespace ec
