//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "ApplicationScheme.hpp"

namespace ec {
template <class DDType, class DDPackage = dd::Package<>>
class SequentialApplicationScheme final
    : public ApplicationScheme<DDType, DDPackage> {
public:
  SequentialApplicationScheme(
      TaskManager<DDType, DDPackage>& taskManager1,
      TaskManager<DDType, DDPackage>& taskManager2) noexcept
      : ApplicationScheme<DDType, DDPackage>(taskManager1, taskManager2),
        gates1(taskManager1.getCircuit()->getNops()),
        gates2(taskManager2.getCircuit()->getNops()) {}

  std::pair<size_t, size_t> operator()() noexcept override {
    return {gates1, gates2};
  }

private:
  const std::size_t gates1;
  const std::size_t gates2;
};
} // namespace ec
