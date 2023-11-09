//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"

namespace ec {
[[nodiscard]] std::size_t
legacyCostFunction(const GateCostLookupTableKeyType& key) noexcept {
  const auto [gate, nc] = key;

  if (nc == 0U) {
    switch (gate) {
    case qc::I:
    case qc::H:
    case qc::X:
    case qc::Y:
    case qc::Z:
    case qc::S:
    case qc::Sdg:
    case qc::T:
    case qc::Tdg:
    case qc::P:
    case qc::U2:
    case qc::U:
    case qc::SX:
    case qc::SXdg:
    case qc::V:
    case qc::Vdg:
    case qc::RX:
    case qc::RY:
    case qc::RZ:
    // the following are merely placeholders so that the check can advance
    case qc::Compound:
    case qc::Measure:
    case qc::Reset:
    case qc::Barrier:
    case qc::ClassicControlled:
      return 1U;
    default:
      break;
    }
  }

  // special treatment for CNOT
  if ((gate == qc::X) && (nc == 1U)) {
    return 1U;
  }

  switch (gate) {
  case qc::X:
    return 2UL * (nc - 2UL) *
               ((2UL * legacyCostFunction({qc::P, 0})) +
                (2UL * legacyCostFunction({qc::U2, 0})) +
                (3UL * legacyCostFunction({qc::X, 1}))) +
           (6UL * legacyCostFunction({qc::X, 1})) +
           (8UL * legacyCostFunction({qc::U, 0}));
  case qc::U:
  case qc::U2:
  case qc::V:
  case qc::Vdg:
  case qc::RX:
  case qc::RY:
  case qc::H:
  case qc::SX:
  case qc::SXdg:
    // heuristic
    return (2U * legacyCostFunction({qc::X, nc})) +
           (4U * legacyCostFunction({qc::U, 0}));
  case qc::P:
  case qc::S:
  case qc::Sdg:
  case qc::T:
  case qc::Tdg:
  case qc::RZ:
    // heuristic
    return (2U * legacyCostFunction({qc::X, nc})) +
           (3U * legacyCostFunction({qc::P, 0}));
  case qc::Y:
  case qc::Z:
    return legacyCostFunction({qc::X, nc}) +
           (2U * legacyCostFunction({qc::U, 0}));
  case qc::SWAP:
    return legacyCostFunction({qc::X, nc}) +
           (2U * legacyCostFunction({qc::X, 1}));
  case qc::iSWAP:
    return (2U * legacyCostFunction({qc::X, nc + 1U})) +
           (2U * legacyCostFunction({qc::S, nc})) +
           (2U * legacyCostFunction({qc::H, nc}));
  case qc::Peres:
  case qc::Peresdg:
    return legacyCostFunction({qc::X, nc + 1U}) +
           legacyCostFunction({qc::X, nc});
  default:
    return 1U;
  }
}
} // namespace ec
