//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/DDConstructionChecker.hpp"

namespace ec {

// this function is mainly placed here in order to have an out-of-line
// virtual method definition which avoids emitting the classe's vtable in
// every translation unit.
void DDConstructionChecker::json(nlohmann::json& j) const noexcept {
  DDEquivalenceChecker::json(j);
  j["checker"] = "decision_diagram_construction";
}

void DDConstructionChecker::postprocessTask(
    TaskManager<qc::MatrixDD, ConstructionDDPackageConfig>& task) {
  // ensure that the permutation that was tracked throughout the circuit matches
  // the expected output permutation
  task.changePermutation();
  if (isDone()) {
    return;
  }
  if (configuration.functionality.checkPartialEquivalence) {
    // reduce ancillaries and garbage for partial equivalence check
    task.reduceForPartialEquivalence();

  } else { // reduce ancillaries for total equivalence check
    // eliminate the superfluous contributions of ancillary qubits (this only
    // has an effect on matrices)
    task.reduceAncillae();
    if (isDone()) {
      return;
    }
    task.reduceGarbage();
  }
}

} // namespace ec
