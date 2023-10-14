//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/zx/ZXChecker.hpp"

#include "Definitions.hpp"
#include "QuantumComputation.hpp"
#include "zx/FunctionalityConstruction.hpp"
#include "zx/Simplify.hpp"
#include "zx/ZXDiagram.hpp"

#include <chrono>
#include <optional>

namespace ec {
ZXEquivalenceChecker::ZXEquivalenceChecker(const qc::QuantumComputation& circ1,
                                           const qc::QuantumComputation& circ2,
                                           Configuration config) noexcept
    : EquivalenceChecker(circ1, circ2, std::move(config)),
      miter(zx::FunctionalityConstruction::buildFunctionality(&qc1)),
      tolerance(configuration.functionality.traceThreshold) {
  zx::ZXDiagram dPrime =
      zx::FunctionalityConstruction::buildFunctionality(&qc2);

  if ((qc1.getNancillae() != 0U) || (qc2.getNancillae() != 0U)) {
    ancilla = true;
  }

  const auto& p1 = invertPermutations(qc1);
  const auto& p2 = invertPermutations(qc2);

  // fix ancillaries to |0>
  const auto nQubitsWithoutAncillae =
      static_cast<zx::Qubit>(qc1.getNqubitsWithoutAncillae());
  for (auto anc = static_cast<zx::Qubit>(qc1.getNqubits() - 1U);
       anc >= nQubitsWithoutAncillae; --anc) {
    miter.makeAncilla(
        anc, static_cast<zx::Qubit>(p1.at(static_cast<qc::Qubit>(anc))));
    dPrime.makeAncilla(
        anc, static_cast<zx::Qubit>(p2.at(static_cast<qc::Qubit>(anc))));
  }
  miter.invert();
  miter.concat(dPrime);
}

EquivalenceCriterion ZXEquivalenceChecker::run() {
  const auto start = std::chrono::steady_clock::now();

  fullReduceApproximate();

  bool equivalent = true;

  if (miter.getNEdges() == miter.getNQubits()) {
    const auto& p1 = invert(invertPermutations(qc1));
    const auto& p2 = invert(invertPermutations(qc2));

    const auto nQubits = miter.getNQubits();
    for (std::size_t i = 0U; i < nQubits; ++i) {
      const auto& in   = miter.getInput(i);
      const auto& edge = miter.incidentEdge(in, 0U);

      if (edge.type == zx::EdgeType::Hadamard) {
        equivalent = false;
        break;
      }
      const auto& out = edge.to;
      const auto& q1  = miter.getVData(in);
      const auto& q2  = miter.getVData(out);
      assert(q1.has_value());
      assert(q2.has_value());
      if (p1.at(static_cast<qc::Qubit>(q1->qubit)) !=
          p2.at(static_cast<qc::Qubit>(q2->qubit))) {
        equivalent = false;
        break;
      }
    }
  } else {
    equivalent = false;
  }

  const auto end = std::chrono::steady_clock::now();
  runtime += std::chrono::duration<double>(end - start).count();

  // non-equivalence might be due to incorrect assumption about the state of
  // ancillaries or the check was aborted prematurely, so no information can be
  // given
  if ((!equivalent && ancilla) || isDone()) {
    equivalence = EquivalenceCriterion::NoInformation;
  } else {
    if (equivalent) {
      if (miter.globalPhaseIsZero()) {
        equivalence = EquivalenceCriterion::Equivalent;
      } else {
        equivalence = EquivalenceCriterion::EquivalentUpToGlobalPhase;
      }
    } else {
      equivalence = EquivalenceCriterion::ProbablyNotEquivalent;
    }
  }
  return equivalence;
}

qc::Permutation invert(const qc::Permutation& p) {
  qc::Permutation pInv{};
  for (const auto [v, w] : p) {
    pInv[w] = v;
  }
  return pInv;
}
qc::Permutation concat(const qc::Permutation& p1,
                       const qc::Permutation& p2) { // p2 after p1
  qc::Permutation pComb{};

  for (const auto [v, w] : p1) {
    if (p2.find(w) != p2.end()) {
      pComb[v] = p2.at(w);
    }
  }
  return pComb;
}

qc::Permutation complete(const qc::Permutation& p, const std::size_t n) {
  qc::Permutation pComp = p;

  std::unordered_map<std::size_t, bool> mappedTo;
  std::unordered_map<std::size_t, bool> mappedFrom;
  for (const auto [k, v] : p) {
    mappedFrom[k] = true;
    mappedTo[v]   = true;
  }

  // Map qubits greedily
  for (std::size_t i = 0; i < n; ++i) {
    if (mappedFrom[i]) {
      continue;
    }

    for (std::size_t j = 0; j < n; ++j) {
      if (!mappedTo[j]) {
        pComp[static_cast<qc::Qubit>(i)] = static_cast<qc::Qubit>(j);
        mappedTo[j]                      = true;
        mappedFrom[i]                    = true;
        break;
      }
    }
  }
  return pComp;
}

qc::Permutation invertPermutations(const qc::QuantumComputation& qc) {
  return concat(invert(complete(qc.outputPermutation, qc.getNqubits())),
                complete(qc.initialLayout, qc.getNqubits()));
}

std::size_t ZXEquivalenceChecker::fullReduceApproximate() {
  auto nSimplifications = fullReduce();
  while (!isDone()) {
    miter.approximateCliffords(tolerance);
    const auto newSimps = fullReduce();
    if (newSimps == 0U) {
      break;
    }
    nSimplifications += newSimps;
  }
  return nSimplifications;
}

std::size_t ZXEquivalenceChecker::fullReduce() {
  if (!isDone()) {
    miter.toGraphlike();
  }
  interiorCliffordSimp();

  bool        newMatches       = true;
  std::size_t nSimplifications = 0U;
  while (!isDone() && newMatches) {
    newMatches = false;
    cliffordSimp();
    const auto nGadget = gadgetSimp();
    interiorCliffordSimp();
    const auto nPivot = pivotGadgetSimp();
    if ((nGadget + nPivot) != 0U) {
      newMatches = true;
      ++nSimplifications;
    }
  }
  if (!isDone()) {
    miter.removeDisconnectedSpiders();
  }

  return nSimplifications;
}

std::size_t ZXEquivalenceChecker::gadgetSimp() {
  std::size_t nSimplifications = 0U;
  bool        newMatches       = true;

  while (!isDone() && newMatches) {
    newMatches = false;
    for (const auto& [v, _] : miter.getVertices()) {
      if (miter.isDeleted(v)) {
        continue;
      }

      if (!isDone() && checkAndFuseGadget(miter, v)) {
        newMatches = true;
        ++nSimplifications;
      }
    }
  }
  return nSimplifications;
}

std::size_t ZXEquivalenceChecker::interiorCliffordSimp() {
  spiderSimp();

  bool        newMatches       = true;
  std::size_t nSimplifications = 0U;
  while (!isDone() && newMatches) {
    newMatches            = false;
    const auto nId        = idSimp();
    const auto nSpider    = spiderSimp();
    const auto nPivot     = pivotPauliSimp();
    const auto nLocalComp = localCompSimp();

    if ((nId + nSpider + nPivot + nLocalComp) != 0U) {
      newMatches = true;
      ++nSimplifications;
    }
  }
  return nSimplifications;
}

std::size_t ZXEquivalenceChecker::cliffordSimp() {
  bool        newMatches       = true;
  std::size_t nSimplifications = 0U;
  while (!isDone() && newMatches) {
    newMatches           = false;
    const auto nClifford = interiorCliffordSimp();
    const auto nPivot    = pivotSimp();
    if ((nClifford + nPivot) != 0U) {
      newMatches = true;
      ++nSimplifications;
    }
  }
  return nSimplifications;
}

} // namespace ec
