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
template <class DDType>
class OneToOneApplicationScheme final : public ApplicationScheme<DDType> {
public:
  OneToOneApplicationScheme(TaskManager<DDType>& tm1,
                            TaskManager<DDType>& tm2) noexcept
      : ApplicationScheme<DDType>(tm1, tm2) {}

  std::pair<size_t, size_t> operator()() noexcept override { return {1U, 1U}; }
};
} // namespace ec
