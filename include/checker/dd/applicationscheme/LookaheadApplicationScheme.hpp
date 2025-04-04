//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "ApplicationScheme.hpp"
#include "checker/dd/TaskManager.hpp"
#include "dd/Node.hpp"
#include "dd/Package.hpp"

#include <cstddef>
#include <utility>

namespace ec {
class LookaheadApplicationScheme final
    : public ApplicationScheme<dd::MatrixDD> {
public:
  LookaheadApplicationScheme(TaskManager<dd::MatrixDD>& tm1,
                             TaskManager<dd::MatrixDD>& tm2) noexcept;

  void setInternalState(dd::MatrixDD& state) noexcept;
  void setPackage(dd::Package* dd) noexcept;

  // in general, the lookup application scheme will apply a single operation of
  // either circuit for every invocation. manipulation of the state is handled
  // directly by the application scheme. Thus, the return value is always {0,0}.
  std::pair<size_t, size_t> operator()() override;

private:
  dd::MatrixDD op1{};
  bool cached1 = false;

  dd::MatrixDD op2{};
  bool cached2 = false;

  // the lookahead application scheme maintains links to an internal state to
  // manipulate and a package to use
  dd::MatrixDD* internalState{};
  dd::Package* package{};
};
} // namespace ec
