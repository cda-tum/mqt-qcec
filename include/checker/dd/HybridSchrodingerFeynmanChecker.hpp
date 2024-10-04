#pragma once

#include "CircuitOptimizer.hpp"
#include "Definitions.hpp"
#include "QuantumComputation.hpp"
#include "dd/ComplexValue.hpp"
#include "dd/Package.hpp"
#include "dd/Package_fwd.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <string>

namespace ec {
template <class Config> class HybridSchrodingerFeynmanChecker final {
public:
  HybridSchrodingerFeynmanChecker(qc::QuantumComputation& circ1,
                                  qc::QuantumComputation& circ2,
                                  const std::size_t nThreads)
      : qc1(&circ1), qc2(&circ2), nthreads(nThreads) {
    // remove final measurements
    if (qc1->getNqubits() != qc2->getNqubits()) {
      throw std::invalid_argument(
          "The two circuits have a different number of qubits.");
    }
    qc::CircuitOptimizer::removeFinalMeasurements(*qc1);
    qc::CircuitOptimizer::removeFinalMeasurements(*qc2);
    qc2->invert();
  }
  std::map<std::string, std::size_t> check();

  //  Get # of decisions for given split_qubit, so that lower slice: q0 < i <
  //  qubit; upper slice: qubit <= i < nqubits
  std::size_t getNDecisions(qc::Qubit splitQubit, qc::QuantumComputation& qc);

protected:
  using DDPackage = typename dd::Package<Config>;
  qc::QuantumComputation* qc1;
  qc::QuantumComputation* qc2;

private:
  std::size_t nthreads = 2;

  void approximateVerification(qc::Qubit splitQubit);

  dd::ComplexValue simulateSlicing(std::unique_ptr<DDPackage>& sliceDD1,
                                   std::unique_ptr<DDPackage>& sliceDD2,
                                   qc::Qubit splitQubit, std::size_t controls);

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
