//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "QuantumComputation.hpp"
#include "dd/Operations.hpp"

namespace ec {
enum class Direction : bool { Left = true, Right = false };

template <class DDType, class DDPackage = dd::Package<>> class TaskManager {
public:
  TaskManager(const qc::QuantumComputation& qc,
              std::unique_ptr<DDPackage>&   package,
              const ec::Direction&          direction) noexcept
      : qc(&qc), package(package), direction(direction),
        permutation(qc.initialLayout), iterator(qc.begin()), end(qc.end()) {}
  TaskManager(const qc::QuantumComputation& qc,
              std::unique_ptr<DDPackage>&   package) noexcept
      : TaskManager(qc, package, Direction::Left) {}

  [[nodiscard]] bool finished() const noexcept { return iterator == end; }

  const std::unique_ptr<qc::Operation>& operator()() const { return *iterator; }

  [[nodiscard]] const DDType& getInternalState() const noexcept {
    return internalState;
  }
  void setInternalState(const DDType& state) noexcept { internalState = state; }
  void flipDirection() noexcept {
    if (direction == Direction::Left) {
      direction = Direction::Right;
    } else {
      direction = Direction::Left;
    }
  }

  [[nodiscard]] qc::MatrixDD getDD() {
    return dd::getDD((*iterator).get(), package, permutation);
  }
  [[nodiscard]] qc::MatrixDD getInverseDD() {
    return dd::getInverseDD((*iterator).get(), package, permutation);
  }

  [[nodiscard]] const qc::QuantumComputation* getCircuit() const noexcept {
    return qc;
  }

  void advanceIterator() { ++iterator; }

  void applyGate(DDType& to) {
    auto saved = to;
    if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
      // direction has no effect on state vector DDs
      to = package->multiply(getDD(), to);
    } else {
      if (direction == Direction::Left) {
        to = package->multiply(getDD(), to);
      } else {
        to = package->multiply(to, getInverseDD());
      }
    }
    package->incRef(to);
    package->decRef(saved);
    package->garbageCollect();
    ++iterator;
  }

  void applySwapOperations(DDType& state) {
    while (!finished() && (*iterator)->getType() == qc::SWAP) {
      applyGate(state);
    }
  }
  void applySwapOperations() { applySwapOperations(internalState); }

  void advance(DDType& state, const std::size_t steps) {
    for (std::size_t i = 0U; i < steps && !finished(); ++i) {
      applyGate(state);
      applySwapOperations(state);
    }
  }
  void advance(DDType& state) { advance(state, 1U); }
  void advance(std::size_t steps) { advance(internalState, steps); }
  void advance() { advance(1U); }

  void finish(DDType& state) {
    while (!finished()) {
      advance(state);
    }
  }
  void finish() { finish(internalState); }

  void changePermutation(DDType& state) {
    dd::changePermutation(state, permutation, qc->outputPermutation, package,
                          static_cast<bool>(direction));
  }
  void changePermutation() { changePermutation(internalState); }

  void reduceAncillae(DDType& state) {
    if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
      state = package->reduceAncillae(state, qc->ancillary,
                                      static_cast<bool>(direction));
    }
  }
  void reduceAncillae() { reduceAncillae(internalState); }

  void reduceGarbage(DDType& state) {
    if constexpr (std::is_same_v<DDType, qc::VectorDD>) {
      state = package->reduceGarbage(state, qc->garbage);
    } else if constexpr (std::is_same_v<DDType, qc::MatrixDD>) {
      state = package->reduceGarbage(state, qc->garbage,
                                     static_cast<bool>(direction));
    }
  }
  void reduceGarbage() { reduceGarbage(internalState); }

  void incRef(DDType& state) { package->incRef(state); }
  void incRef() { incRef(internalState); }

  void decRef(DDType& state) { package->decRef(state); }
  void decRef() { decRef(internalState); }

private:
  const qc::QuantumComputation* qc{};
  std::unique_ptr<DDPackage>&   package;
  ec::Direction                 direction = Direction::Left;
  qc::Permutation               permutation{};
  decltype(qc->begin())         iterator;
  decltype(qc->end())           end;
  DDType                        internalState{};
};
} // namespace ec
