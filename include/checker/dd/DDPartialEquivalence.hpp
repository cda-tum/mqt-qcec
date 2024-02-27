#pragma once

#include "dd/FunctionalityConstruction.hpp"
#include "dd/Package.hpp"

namespace dd {

template <class Config>
mEdge partialEquivalenceCheckDDSubroutine(mEdge u, const Qubit m, const Qubit k,
                                          const Qubit      extra,
                                          Package<Config>& dd) {
  // add extra ancillary qubits
  if (extra > 0) {
    if (u.p->v + 1U + extra > dd.qubits()) {
      dd.resize(u.p->v + 1U + extra);
    }
    u = dd.kronecker(dd.makeIdent(extra), u);
  }
  if (u.isTerminal()) {
    return u;
  }
  const Qubit n = u.p->v + 1;
  const Qubit d = n - k;
  u             = dd.setColumnsToZero(u, d);
  const auto u2 = dd.shiftAllRows(u, m, d);
  return dd.multiply(dd.conjugateTranspose(u), u2);
}

// get next garbage qubit after n
inline Qubit getNextGarbage(Qubit n, const std::vector<bool>& garbage) {
  while (n < static_cast<Qubit>(garbage.size()) && !garbage.at(n)) {
    n++;
  }
  return n;
}
/**
    Checks for partial equivalence between the two circuits c1 and c2.
    Assumption: the data qubits are all at the beginning of the input qubits and
    the input and output permutations are the same.

    @param circuit1 First circuit
    @param circuit2 Second circuit
    @return true if the two circuits c1 and c2 are partially equivalent.
    **/
template <class Config>
bool partialEquivalenceCheck(qc::QuantumComputation c1,
                             qc::QuantumComputation c2, Package<Config>& dd) {
  const auto d1 = c1.getNqubitsWithoutAncillae();
  const auto d2 = c2.getNqubitsWithoutAncillae();
  const auto m1 = c1.getNmeasuredQubits();
  const auto m2 = c2.getNmeasuredQubits();
  if (m1 != m2 || d1 != d2) {
    return false;
  }
  const auto n1 = static_cast<Qubit>(c1.getNqubits());
  const auto n2 = static_cast<Qubit>(c2.getNqubits());

  // add swaps in order to put the measured (= not garbage) qubits in the end
  const auto garbage1 = c1.getGarbage();

  auto nextGarbage = getNextGarbage(0, garbage1);
  // find the first garbage qubit at the end
  for (std::int64_t i = std::min(n1, n2) - 1;
       i >= static_cast<std::int64_t>(m1); i--) {
    if (!garbage1.at(static_cast<Qubit>(i))) {
      // swap it to the beginning
      c1.swap(static_cast<Qubit>(i), nextGarbage);
      c2.swap(static_cast<Qubit>(i), nextGarbage);
      ++nextGarbage;
      nextGarbage = getNextGarbage(nextGarbage, garbage1);
    }
  }

  // partialEquivalenceCheck with dd

  const auto u1 = buildFunctionality(&c1, dd, false, false);
  const auto u2 = buildFunctionality(&c2, dd, false, false);

  return true;
}

/**
  Generate random benchmarks for partial equivalence checking.
  @param n number of qubits of the resulting circuits
  @param d number of data qubits in the resulting circuits
  @param m number of measured qubits in the resulting circuit
  @return two circuits that are partially equivalent
**/
std::pair<qc::QuantumComputation, qc::QuantumComputation>
generateRandomBenchmark(size_t n, qc::Qubit d, qc::Qubit m);
} // namespace dd
