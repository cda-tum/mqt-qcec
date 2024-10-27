#pragma once

#include "Configuration.hpp"
#include "DDEquivalenceChecker.hpp"
#include "Definitions.hpp"
#include "EquivalenceCriterion.hpp"
#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "dd/ComplexValue.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Package.hpp"
#include "dd/Package_fwd.hpp"
#include "ir/QuantumComputation.hpp"
#include "ir/operations/OpType.hpp"
#include "memory"
#include "nlohmann/json_fwd.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace ec {
/**
 * @brief Approximate Equivalence Checking with the
 * HybridSchrodingerFeynmanChecker This checker divides a circuit horizontally
 * into two halves: a lower part and an upper part. This is achieved by
 * decomposing controlled gates, acting across both halves, according to the
 * Schmidt decomposition. By leveraging key trace equalities - specifically,
 *
 * tr[L ⊗ U] = tr[L] ⋅ tr[U]
 *
 * and
 *
 * tr[A + B] = tr[A] + tr[B],
 *
 * we can treat the lower and upper circuit parts, as well as the summands from
 * the Schmidt decomposition, independently. This enables parallel trace
 * computation, allowing to check the equivalence of larger, yet shallow
 * circuits.
 * @note Only suitable for shallow circuits with a maximum number of 63
 * controlled gates acting on both circuit parts (decisions).
 */
class HybridSchrodingerFeynmanChecker final
    : public DDEquivalenceChecker<qc::MatrixDD, dd::DDPackageConfig> {
public:
  HybridSchrodingerFeynmanChecker(const qc::QuantumComputation& circ1,
                                  const qc::QuantumComputation& circ2,
                                  ec::Configuration config)
      : DDEquivalenceChecker(circ1, circ2, std::move(config)),
        qc1(std::make_unique<qc::QuantumComputation>(circ1)),
        qc2Inverted(std::make_unique<qc::QuantumComputation>()) {
    if (circ1.getNqubits() != circ2.getNqubits()) {
      throw std::invalid_argument(
          "The two circuits have a different number of qubits and cannot be "
          "checked for equivalence.");
    }

    // Flatten the operations of the circuits
    qc::CircuitOptimizer::flattenOperations(*qc1);
    qc::CircuitOptimizer::flattenOperations(*qc2Inverted);

    // Invert the second circuit by iterating through the operations in reverse
    // order and inverting each one - except for Measure and Barrier operations
    for (auto it = circ2.rbegin(); it != circ2.rend(); ++it) {
      if (it->get()->getType() != qc::Measure &&
          it->get()->getType() != qc::Barrier) {
        qc2Inverted->emplace_back(it->get()->getInverted());
      }
    }

    splitQubit = static_cast<qc::Qubit>((&circ1)->getNqubits() / 2);
    initializeApplicationScheme(
        this->configuration.application.alternatingScheme);
  }
  EquivalenceCriterion run() override;

  void json(nlohmann::json& j) const noexcept override;

  /**
   * @brief Get # of decisions for given split_qubit, so that lower slice: q0 <
   * i < qubit; upper slice: qubit <= i < nqubits
   * @details The number of decisions is determined by the number of controlled
   * gates that operate across both halves.
   * @param qc
   * @return std::size_t
   */
  [[nodiscard]] std::size_t getNDecisions(qc::QuantumComputation& qc) const;

private:
  std::unique_ptr<qc::QuantumComputation> qc1;
  std::unique_ptr<qc::QuantumComputation> qc2Inverted;
  qc::Qubit splitQubit;

  using DDPackage = typename dd::Package<dd::DDPackageConfig>;

  /**
   * @brief Computing the Frobenius inner product trace(U * V^-1) and comparing
   * it to the desired threshold.
   * @return EquivalenceCriterion Returns `Equivalent` if the result is below
   * the `traceThreshold`, `NotEquivalent´ otherwise.
   */
  EquivalenceCriterion checkEquivalence() override;

  /**
   * @brief Computes the trace for the i-th summand after applying the Schmidt
   * decomposition for all control decisions.
   *
   * @details The Schmidt decomposition allows decomposing a controlled gate
   * into a sum of circuits, each consisting of only single-qubit gates. By
   * recursively applying this decomposition to all decisions, we generate a
   * total of 2^decisions circuits, which do not contain controlled operations
   * acting on both halves. This enables independent investigation of the
   * lower and upper circuit parts.
   *
   * This function computes the trace for the i-th summand, where the index 'i'
   * determines, for each gate, whether the 0 or 1 projection is considered. See
   * getNextControl() and apply() for more details on how these projections are
   * managed.
   *
   * @param sliceDD1 Decision diagram for the lower circuit part.
   * @param sliceDD2 Decision diagram for the upper circuit part.
   * @param i Index of the summand for which the trace is computed.
   * @return dd::ComplexValue Returns the trace value for the specified summand.
   */
  dd::ComplexValue simulateSlicing(std::unique_ptr<DDPackage>& sliceDD1,
                                   std::unique_ptr<DDPackage>& sliceDD2,
                                   std::size_t i);

  class Slice;

  /**
   * @brief Applies a single operation to the lower and upper circuit parts
   * according to the Schmidt decomposition, for the summand specified by
   * `controlIdx` from the class Slice.
   *
   * @param sliceDD1 Decision diagram for the lower circuit part.
   * @param sliceDD2 Decision diagram for the upper circuit part.
   * @param op Current operation to be applied
   * @param lower
   * @param upper
   */
  static void applyLowerUpper(std::unique_ptr<DDPackage>& sliceDD1,
                              std::unique_ptr<DDPackage>& sliceDD2,
                              const std::unique_ptr<qc::Operation>& op,
                              Slice& lower, Slice& upper) {
    if (op->isUnitary()) {
      [[maybe_unused]] auto l = lower.apply(sliceDD1, op);
      [[maybe_unused]] auto u = upper.apply(sliceDD2, op);
      assert(l == u);
    }
    sliceDD1->garbageCollect();
    sliceDD2->garbageCollect();
  }

  class Slice {
  protected:
    std::uint64_t nextControlIdx = 0;

    /**
     * @brief Determines how the current operation is decomposed for the summand
     * at index `controlIdx`.
     * @details nextControlIdx tracks the number of operations processed so far.
     * By comparing the shifted value to the bits of `controlIdx`, we can
     * determine how the current operation should be decomposed for the summand
     * at index `controlIdx`.
     * @return std::size_t
     */
    std::size_t getNextControl() {
      const std::size_t idx = 1UL << nextControlIdx;
      nextControlIdx++;
      return controlIdx & idx;
    }

  public:
    qc::Qubit start;
    qc::Qubit end;
    std::size_t controlIdx;
    qc::Qubit nqubits;
    std::size_t nDecisionsExecuted = 0;
    qc::MatrixDD matrix{};

    explicit Slice(std::unique_ptr<DDPackage>& dd, const qc::Qubit startQ,
                   const qc::Qubit endQ, const std::size_t controlQ)
        : start(startQ), end(endQ), controlIdx(controlQ),
          nqubits(end - start + 1), matrix(dd->makeIdent()) {
      dd->incRef(matrix);
    }

    /**
     * @brief Applies the decomposition of the current operation, based on the
     * summand index `controlIdx`, to the decision diagram of the specified
     * circuit slice.
     *
     * @param sliceDD Decision diagram for the lower or upper circuit part.
     * @param op
     * @return bool Returns true if the operation is a split operation, false
     * otherwise.
     */
    bool apply(std::unique_ptr<DDPackage>& sliceDD,
               const std::unique_ptr<qc::Operation>& op);
  };
};

} // namespace ec
