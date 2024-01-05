//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "StateType.hpp"
#include "algorithms/RandomCliffordCircuit.hpp"
#include "checker/dd/TaskManager.hpp"
#include "dd/Package.hpp"
#include "dd/Simulation.hpp"

#include <functional>
#include <random>

namespace ec {
class StateGenerator {
public:
  explicit StateGenerator(const std::size_t s) : seed(s) {
    seedGenerator(seed);
  }
  StateGenerator() : StateGenerator(0U) {}

  template <class Config = dd::DDPackageConfig>
  qc::VectorDD
  generateRandomState(dd::Package<Config>& dd, const std::size_t totalQubits,
                      const std::size_t ancillaryQubits = 0U,
                      const StateType   type = StateType::ComputationalBasis) {
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

  template <class Config = dd::DDPackageConfig>
  qc::VectorDD generateRandomComputationalBasisState(
      dd::Package<Config>& dd, const std::size_t totalQubits,
      const std::size_t ancillaryQubits = 0U) {
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

  template <class Config = dd::DDPackageConfig>
  qc::VectorDD
  generateRandom1QBasisState(dd::Package<Config>& dd,
                             const std::size_t    totalQubits,
                             const std::size_t    ancillaryQubits = 0U) {
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

  template <class Config = dd::DDPackageConfig>
  qc::VectorDD
  generateRandomStabilizerState(dd::Package<Config>& dd,
                                const std::size_t    totalQubits,
                                const std::size_t    ancillaryQubits = 0U) {
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

  void seedGenerator(std::size_t s);

  void clear() { generatedComputationalBasisStates.clear(); }

private:
  std::size_t     seed = 0U;
  std::mt19937_64 mt;

  std::unordered_set<std::size_t> generatedComputationalBasisStates{};
  constexpr static std::size_t    ONE_QUBIT_BASE_ELEMENTS = 6U;
  // this generator produces random bases from the set { |0>, |1>, |+>, |->,
  // |L>, |R> }
  std::uniform_int_distribution<std::size_t> random1QBasisDistribution =
      std::uniform_int_distribution<std::size_t>(0U,
                                                 ONE_QUBIT_BASE_ELEMENTS - 1U);
};
} // namespace ec
