//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/DDConstructionChecker.hpp"

#include "checker/dd/DDEquivalenceChecker.hpp"

#include <nlohmann/json.hpp>

// this function is mainly placed here in order to have an out-of-line
// virtual method definition which avoids emitting the classes' vtable in
// every translation unit.
void ec::DDConstructionChecker::json(nlohmann::basic_json<>& j) const noexcept {
  DDEquivalenceChecker::json(j);
  j["checker"] = "decision_diagram_construction";
}
