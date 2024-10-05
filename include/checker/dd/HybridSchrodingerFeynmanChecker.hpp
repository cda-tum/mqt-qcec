#pragma once

#include "Definitions.hpp"
#include "dd/ComplexValue.hpp"
#include "dd/Package.hpp"
#include "dd/Package_fwd.hpp"
#include "ir/QuantumComputation.hpp"
#include "memory"

#include <cassert>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>

namespace ec {
template <class Config> class HybridSchrodingerFeynmanChecker final {
public:
  HybridSchrodingerFeynmanChecker(const qc::QuantumComputation& circ1,
                                  const qc::QuantumComputation& circ2,
                                  const std::size_t nThreads)
      : qc1(&circ1), qc2(&circ2), nthreads(nThreads) {
    // remove final measurements
    if (qc1->getNqubits() != qc2->getNqubits()) {
      throw std::invalid_argument(
          "The two circuits have a different number of qubits.");
    }
  }
  std::map<std::string, std::size_t> check();

  //  Get # of decisions for given split_qubit, so that lower slice: q0 < i <
  //  qubit; upper slice: qubit <= i < nqubits
  std::size_t getNDecisions(qc::Qubit splitQubit,
                            const qc::QuantumComputation& qc);

protected:
  using DDPackage = typename dd::Package<Config>;
  qc::QuantumComputation const* qc1;
  qc::QuantumComputation const* qc2;

private:
  std::size_t nthreads = 2;

  void approximateVerification(qc::Qubit splitQubit);

  dd::ComplexValue simulateSlicing(std::unique_ptr<DDPackage>& sliceDD1,
                                   std::unique_ptr<DDPackage>& sliceDD2,
                                   qc::Qubit splitQubit, std::size_t controls);

  class Slice;

  void applyLowerUpper(std::unique_ptr<DDPackage>& sliceDD1,
                       std::unique_ptr<DDPackage>& sliceDD2,
                       const std::unique_ptr<qc::Operation>& op, Slice& lower,
                       Slice& upper) {
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
    qc::Qubit nextControlIdx = 0;

    std::size_t getNextControl() {
      std::size_t idx = 1UL << nextControlIdx;
      nextControlIdx++;
      return controls & idx;
    }

  public:
    qc::Qubit start;
    qc::Qubit end;
    std::size_t controls;
    qc::Qubit nqubits;
    std::size_t nDecisionsExecuted = 0;
    qc::MatrixDD matrix{};

    explicit Slice(std::unique_ptr<DDPackage>& dd, const qc::Qubit startQ,
                   const qc::Qubit endQ, const std::size_t controlQ)
        : start(startQ), end(endQ), controls(controlQ),
          nqubits(end - start + 1), matrix(dd->makeIdent()) {
      dd->incRef(matrix);
    }

    bool apply(std::unique_ptr<DDPackage>& sliceDD,
               const std::unique_ptr<qc::Operation>& op);
  };
};

} // namespace ec
