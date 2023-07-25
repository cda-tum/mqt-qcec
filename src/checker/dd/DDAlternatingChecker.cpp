//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/DDAlternatingChecker.hpp"

namespace ec {
void DDAlternatingChecker::initialize() {
  DDEquivalenceChecker::initialize();
  // create the full identity matrix
  functionality = dd->makeIdent(nqubits);
  dd->incRef(functionality);

  // Only count ancillaries that are present in but not acted upon in both of
  // the circuits. Otherwise, the alternating checker must not be used.
  // Counter-example: H |0><0| H^-1 = [[0.5, 0.5], [0.5, 0.5]] != |0><0|
  if (!canHandle(qc1, qc2)) {
    throw std::invalid_argument(
        "Alternating checker must not be used for "
        "circuits that both have non-idle ancillary "
        "qubits. Use the construction checker instead.");
  }

  std::vector<bool> ancillary(nqubits);
  for (qc::Qubit q = 0U; q < nqubits; ++q) {
    ancillary[static_cast<std::size_t>(q)] =
        qc1.logicalQubitIsAncillary(q) && qc2.logicalQubitIsAncillary(q);
  }

  // reduce the ancillary qubit contributions
  // [1 0] if the qubit is no ancillary, or it is acted upon by both circuits
  // [0 1]
  //
  // [1 0] (= |0><0|) for an ancillary only acted on in one circuit
  // [0 0]
  functionality = dd->reduceAncillae(functionality, ancillary);
}

void DDAlternatingChecker::execute() {
  while (!taskManager1.finished() && !taskManager2.finished() && !isDone()) {
    // skip over any SWAP operations
    taskManager1.applySwapOperations(functionality);
    taskManager2.applySwapOperations(functionality);

    if (!taskManager1.finished() && !taskManager2.finished()) {
      if (isDone()) {
        return;
      }

      // whenever the current functionality resembles the identity, identical
      // gates on both sides cancel
      if (functionality.p->isIdentity() &&
          (configuration.application.alternatingScheme !=
           ApplicationSchemeType::Lookahead) &&
          gatesAreIdentical()) {
        taskManager1.advanceIterator();
        taskManager2.advanceIterator();
        continue;
      }
      // query application scheme on how to proceed
      const auto [apply1, apply2] = (*applicationScheme)();

      // advance both tasks correspondingly
      if (isDone()) {
        return;
      }
      taskManager1.advance(functionality, apply1);
      if (isDone()) {
        return;
      }
      taskManager2.advance(functionality, apply2);
    }
  }
}

void DDAlternatingChecker::finish() {
  taskManager1.finish(functionality);
  if (isDone()) {
    return;
  }
  taskManager2.finish(functionality);
}

void DDAlternatingChecker::postprocess() {
  // ensure that the permutations that were tracked throughout the circuit match
  // the expected output permutations
  taskManager1.changePermutation(functionality);
  if (isDone()) {
    return;
  }
  taskManager2.changePermutation(functionality);
  if (isDone()) {
    return;
  }

  // sum up the contributions of garbage qubits
  taskManager1.reduceGarbage(functionality);
  if (isDone()) {
    return;
  }
  taskManager2.reduceGarbage(functionality);
  if (isDone()) {
    return;
  }
}

EquivalenceCriterion DDAlternatingChecker::checkEquivalence() {
  // create the full identity matrix
  auto goalMatrix = dd->makeIdent(nqubits);
  dd->incRef(goalMatrix);

  // account for any garbage
  taskManager1.reduceGarbage(goalMatrix);
  taskManager2.reduceGarbage(goalMatrix);

  taskManager1.reduceAncillae(goalMatrix);
  taskManager2.reduceAncillae(goalMatrix);

  // the resulting goal matrix is
  // [1 0] if the qubit is no ancillary
  // [0 1]
  //
  // [1 0] (= |0><0>|) for an ancillary that is present in either circuit
  // [0 0]

  // compare the obtained functionality to the goal matrix
  return equals(functionality, goalMatrix);
}

[[nodiscard]] bool DDAlternatingChecker::gatesAreIdentical() const {
  if (taskManager1.finished() || taskManager2.finished()) {
    return false;
  }

  const auto& op1 = *taskManager1();
  const auto& op2 = *taskManager2();

  return op1.equals(op2);
}

bool DDAlternatingChecker::canHandle(const qc::QuantumComputation& qc1,
                                     const qc::QuantumComputation& qc2) {
  assert(qc1.getNqubits() == qc2.getNqubits());
  const auto nqubits = qc1.getNqubits();

  for (auto qubit = static_cast<std::make_signed_t<qc::Qubit>>(nqubits - 1U);
       qubit >= 0; --qubit) {
    const auto q = static_cast<qc::Qubit>(qubit);
    if (qc1.logicalQubitIsAncillary(q) && qc2.logicalQubitIsAncillary(q)) {
      const auto [found1, physical1] = qc1.containsLogicalQubit(q);
      const auto [found2, physical2] = qc2.containsLogicalQubit(q);

      // just continue, if a qubit is not found in both circuits
      if (found1 != found2) {
        continue;
      }

      const auto isIdle1 = found1 && qc1.isIdleQubit(*physical1);
      const auto isIdle2 = found2 && qc2.isIdleQubit(*physical2);

      // if an ancillary qubit is acted on in both circuits,
      // the alternating checker cannot be used.
      if (!isIdle1 && !isIdle2) {
        return false;
      }
    }
  }
  return true;
}

} // namespace ec
