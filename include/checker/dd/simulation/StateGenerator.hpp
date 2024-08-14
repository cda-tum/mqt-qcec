//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "StateType.hpp"
#include "checker/dd/DDPackageConfigs.hpp"
#include "dd/Package_fwd.hpp"

#include <cassert>
#include <cstddef>
#include <random>
#include <unordered_set>

namespace ec {
class StateGenerator {
public:
  explicit StateGenerator(const std::size_t s) : seed(s) {
    seedGenerator(seed);
  }
  StateGenerator() : StateGenerator(0U) {}

  qc::VectorDD
  generateRandomState(dd::Package<SimulationDDPackageConfig>& dd,
                      std::size_t totalQubits, std::size_t ancillaryQubits = 0U,
                      StateType type = StateType::ComputationalBasis);

  qc::VectorDD generateRandomComputationalBasisState(
      dd::Package<SimulationDDPackageConfig>& dd, std::size_t totalQubits,
      std::size_t ancillaryQubits = 0U);

  qc::VectorDD
  generateRandom1QBasisState(dd::Package<SimulationDDPackageConfig>& dd,
                             std::size_t totalQubits,
                             std::size_t ancillaryQubits = 0U);

  qc::VectorDD
  generateRandomStabilizerState(dd::Package<SimulationDDPackageConfig>& dd,
                                std::size_t totalQubits,
                                std::size_t ancillaryQubits = 0U);

  void seedGenerator(std::size_t s);

  void clear() { generatedComputationalBasisStates.clear(); }

private:
  std::size_t seed = 0U;
  std::mt19937_64 mt;

  std::unordered_set<std::size_t> generatedComputationalBasisStates;
  constexpr static std::size_t ONE_QUBIT_BASE_ELEMENTS = 6U;
  // this generator produces random bases from the set { |0>, |1>, |+>, |->,
  // |L>, |R> }
  std::uniform_int_distribution<std::size_t> random1QBasisDistribution =
      std::uniform_int_distribution<std::size_t>(0U,
                                                 ONE_QUBIT_BASE_ELEMENTS - 1U);
};
} // namespace ec
