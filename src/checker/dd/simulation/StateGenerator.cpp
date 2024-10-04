//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/dd/simulation/StateGenerator.hpp"

#include "algorithms/RandomCliffordCircuit.hpp"
#include "checker/dd/DDPackageConfigs.hpp"
#include "checker/dd/simulation/StateType.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/Package.hpp"
#include "dd/Simulation.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
#include <tuple>
#include <vector>

namespace ec {

qc::VectorDD StateGenerator::generateRandomState(
    dd::Package<SimulationDDPackageConfig>& dd, const std::size_t totalQubits,
    const std::size_t ancillaryQubits, const StateType type) {
  switch (type) {
  case ec::StateType::Random1QBasis:
    return generateRandom1QBasisState(dd, totalQubits, ancillaryQubits);
  case ec::StateType::Stabilizer:
    return generateRandomStabilizerState(dd, totalQubits, ancillaryQubits);
  default:
    return generateRandomComputationalBasisState(dd, totalQubits,
                                                 ancillaryQubits);
  }
}

qc::VectorDD StateGenerator::generateRandomComputationalBasisState(
    dd::Package<SimulationDDPackageConfig>& dd, const std::size_t totalQubits,
    const std::size_t ancillaryQubits) {
  // determine how many qubits truly are random
  const std::size_t randomQubits = totalQubits - ancillaryQubits;
  std::vector<bool> stimulusBits(totalQubits, false);

  // check if there still is a unique computational basis state
  if (constexpr auto bitwidth = std::numeric_limits<std::uint64_t>::digits;
      randomQubits <= (bitwidth - 1U)) {
    const auto maxStates = static_cast<std::uint64_t>(1U) << randomQubits;
    assert(generatedComputationalBasisStates.size() != maxStates);
    // generate a unique computational basis state
    std::uniform_int_distribution<std::uint64_t> distribution(0U,
                                                              maxStates - 1U);
    auto [randomState, success] =
        generatedComputationalBasisStates.insert(distribution(mt));
    while (!success) {
      std::tie(randomState, success) =
          generatedComputationalBasisStates.insert(distribution(mt));
    }

    // generate the bitvector corresponding to the random state
    for (std::size_t i = 0U; i < randomQubits; ++i) {
      if ((*randomState & (static_cast<std::uint64_t>(1U) << i)) != 0U) {
        stimulusBits[i] = true;
      }
    }
  } else {
    // check how many numbers are needed for each random state
    const auto nr = static_cast<std::size_t>(
        std::ceil(static_cast<double>(randomQubits) / bitwidth));
    // generate enough random numbers
    std::vector<std::mt19937_64::result_type> randomNumbers(nr, 0U);
    for (auto i = 0U; i < nr; ++i) {
      randomNumbers[i] = mt();
    }
    // generate the corresponding bitvector
    for (std::size_t i = 0U; i < randomQubits; ++i) {
      if ((randomNumbers[i / bitwidth] &
           (static_cast<std::uint_least64_t>(1U) << (i % bitwidth))) != 0U) {
        stimulusBits[i] = true;
      }
    }
  }

  // return the appropriate decision diagram
  return dd.makeBasisState(totalQubits, stimulusBits);
}

qc::VectorDD StateGenerator::generateRandom1QBasisState(
    dd::Package<SimulationDDPackageConfig>& dd, const std::size_t totalQubits,
    const std::size_t ancillaryQubits) {
  // determine how many qubits truly are random
  const std::size_t randomQubits = totalQubits - ancillaryQubits;

  // choose a random basis state for each qubit
  auto randomBasisState =
      std::vector<dd::BasisStates>(totalQubits, dd::BasisStates::zero);
  for (std::size_t i = 0U; i < randomQubits; ++i) {
    switch (random1QBasisDistribution(mt)) {
    case static_cast<std::size_t>(dd::BasisStates::zero):
      randomBasisState[i] = dd::BasisStates::zero;
      break;
    case static_cast<std::size_t>(dd::BasisStates::one):
      randomBasisState[i] = dd::BasisStates::one;
      break;
    case static_cast<std::size_t>(dd::BasisStates::plus):
      randomBasisState[i] = dd::BasisStates::plus;
      break;
    case static_cast<std::size_t>(dd::BasisStates::minus):
      randomBasisState[i] = dd::BasisStates::minus;
      break;
    case static_cast<std::size_t>(dd::BasisStates::right):
      randomBasisState[i] = dd::BasisStates::right;
      break;
    case static_cast<std::size_t>(dd::BasisStates::left):
      randomBasisState[i] = dd::BasisStates::left;
      break;
    default:
      break;
    }
  }

  // return the appropriate decision diagram
  return dd.makeBasisState(totalQubits, randomBasisState);
}

qc::VectorDD StateGenerator::generateRandomStabilizerState(
    dd::Package<SimulationDDPackageConfig>& dd, const std::size_t totalQubits,
    const std::size_t ancillaryQubits) {
  // determine how many qubits truly are random
  const std::size_t randomQubits = totalQubits - ancillaryQubits;

  // generate a random Clifford circuit with appropriate depth
  auto rcs = qc::RandomCliffordCircuit(
      randomQubits,
      static_cast<std::size_t>(std::round(std::log2(randomQubits))), mt());

  // generate the associated stabilizer state by simulating the Clifford
  // circuit
  auto stabilizer = simulate(&rcs, dd.makeZeroState(randomQubits), dd);

  // decrease the ref count right after so that it stays correct later on
  dd.decRef(stabilizer);

  // add |0> edges for all the ancillary qubits
  auto initial = stabilizer;
  for (std::size_t p = randomQubits; p < totalQubits; ++p) {
    initial = dd.makeDDNode(static_cast<dd::Qubit>(p),
                            std::array{initial, qc::VectorDD::zero()});
  }

  // return the resulting decision diagram
  return initial;
}

void StateGenerator::seedGenerator(const std::size_t s) {
  seed = s;
  if (seed == 0U) {
    std::array<std::mt19937_64::result_type, std::mt19937_64::state_size>
        randomData{};
    std::random_device rd;
    std::generate(std::begin(randomData), std::end(randomData), std::ref(rd));
    std::seed_seq seeds(std::begin(randomData), std::end(randomData));
    mt.seed(seeds);
  } else {
    mt.seed(seed);
  }
}

} // namespace ec
