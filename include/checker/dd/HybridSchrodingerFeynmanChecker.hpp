#pragma once

#include "Definitions.hpp"
#include "EquivalenceCriterion.hpp"
#include "circuit_optimizer/CircuitOptimizer.hpp"
#include "dd/ComplexValue.hpp"
#include "dd/Package.hpp"
#include "dd/Package_fwd.hpp"
#include "ir/QuantumComputation.hpp"
#include "memory"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace ec {
/**
 * @brief Approximate Equivalence Checking with the
 * HybridSchrodingerFeynmanChecker This checker divides a circuit horizontally
 * into two halves: a lower part and an upper part. This is achieved by cutting
 * controlled gates that cross the middle line according to the Schmidt
 * decomposition. By leveraging key trace equalities - specifically,
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
 * controlled gates crossing the middle line (decisions).
 */
template <class Config> class HybridSchrodingerFeynmanChecker final {
public:
  HybridSchrodingerFeynmanChecker(const qc::QuantumComputation& circ1,
                                  const qc::QuantumComputation& circ2,
                                  const double threshold,
                                  const std::size_t nThreads)
      : qc1(std::make_unique<qc::QuantumComputation>(circ1)),
        qc2Inverted(std::make_unique<qc::QuantumComputation>()),
        traceThreshold(threshold), nthreads(nThreads) {
    if (circ1.getNqubits() != circ2.getNqubits()) {
      throw std::invalid_argument(
          "The two circuits have a different number of qubits and cannot be "
          "checked for equivalence.");
    }

    // Invert the second circuit by iterating through the operations in reverse
    // order and inverting each one
    for (auto it = circ2.rbegin(); it != circ2.rend(); ++it) {
      qc2Inverted->emplace_back(it->get()->getInverted());
    }

    // Flatten the operations of the circuits
    qc::CircuitOptimizer::flattenOperations(*qc1);
    qc::CircuitOptimizer::flattenOperations(*qc2Inverted);

    splitQubit = static_cast<qc::Qubit>((&circ1)->getNqubits() / 2);
  }

  EquivalenceCriterion run();

private:
  std::unique_ptr<qc::QuantumComputation> qc1;
  std::unique_ptr<qc::QuantumComputation> qc2Inverted;
  double traceThreshold = 1e-8;
  std::size_t nthreads = 2;
  qc::Qubit splitQubit;
  double runtime{};

  using DDPackage = typename dd::Package<Config>;

  /**
   * @brief Computing the Frobenius inner product trace(U * V^-1) and comparing
   * it to the desired threshold.
   * @return EquivalenceCriterion Returns `Equivalent` if the result is below
   * the `traceThreshold`, `NotEquivalent´ otherwise.
   */
  EquivalenceCriterion checkEquivalence();

  /**
   * @brief Get # of decisions for given split_qubit, so that lower slice: q0 <
   * i < qubit; upper slice: qubit <= i < nqubits
   * @details The number of decisions is determined by the number of controlled
   * gates that cross the middle line.
   * @param qc
   * @return std::size_t
   */
  [[nodiscard]] std::size_t getNDecisions(qc::QuantumComputation& qc) const;

  /**
   * @brief Computes the trace for the i-th summand after applying the Schmidt
   * decomposition for all control decisions.
   *
   * @details The Schmidt decomposition allows decomposing a controlled gate
   * into a sum of circuits, each consisting of only single-qubit gates. By
   * recursively applying this decomposition to all decisions, we generate a
   * total of 2^decisions circuits, which do not contain controlled operations
   * crossing the middle line. This enables independent investigation of the
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
      std::size_t idx = 1UL << nextControlIdx;
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
