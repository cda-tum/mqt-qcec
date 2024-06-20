//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/EquivalenceChecker.hpp"

#include "EquivalenceCriterion.hpp"

#include <nlohmann/json.hpp>

// this function is mainly placed here in order to have an out-of-line
// virtual method definition which avoids emitting the classe's vtable in
// every translation unit.
void ec::EquivalenceChecker::json(nlohmann::basic_json<>& j) const noexcept {
  j["equivalence"] = toString(equivalence);
  j["runtime"] = getRuntime();
}
