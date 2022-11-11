//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "checker/zx/ZXChecker.hpp"

#include "Definitions.hpp"
#include "QuantumComputation.hpp"
#include "Simplify.hpp"
#include "ZXDiagram.hpp"
#include "dd/Definitions.hpp"
#include "zx/FunctionalityConstruction.hpp"

#include <chrono>
#include <optional>

namespace ec {
ZXEquivalenceChecker::ZXEquivalenceChecker(const qc::QuantumComputation& qc1,
                                           const qc::QuantumComputation& qc2,
                                           Configuration configuration) noexcept
    : EquivalenceChecker(qc1, qc2, std::move(configuration)),
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
  const auto nQubitsWithoutAncillae = qc1.getNqubitsWithoutAncillae();
  for (auto anc = static_cast<dd::Qubit>(qc1.getNqubits() - 1U);
       anc >= nQubitsWithoutAncillae; --anc) {
    miter.makeAncilla(anc, p1.at(anc));
    dPrime.makeAncilla(anc, p2.at(anc));
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

      if (p1.at(static_cast<dd::Qubit>(miter.getVData(in).value().qubit)) !=
          p2.at(static_cast<dd::Qubit>(miter.getVData(out).value().qubit))) {
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

qc::Permutation complete(const qc::Permutation& p, const dd::QubitCount n) {
  qc::Permutation pComp = p;

  std::vector<bool> mappedTo(n, false);
  std::vector<bool> mappedFrom(n, false);
  for (const auto [k, v] : p) {
    mappedFrom[k] = true;
    mappedTo[v]   = true;
  }

  // Try to map identity
  for (dd::Qubit i = 0; i < n; ++i) {
    if (mappedFrom[i] || mappedTo[i]) {
      continue;
    }

    pComp[i]      = i;
    mappedFrom[i] = true;
    mappedTo[i]   = true;
  }

  // Try to map inverse
  for (dd::Qubit i = 0; i < n; ++i) {
    if (mappedFrom[i]) {
      continue;
    }
    std::optional<dd::Qubit> j;
    for (const auto [k, v] : p) {
      if (v == i) {
        j = k;
        break;
      }
    }

    if (mappedTo[j.value()]) {
      continue;
    }

    pComp[i]            = j.value();
    mappedFrom[i]       = true;
    mappedTo[j.value()] = true;
  }

  // Map rest greedily

  for (dd::Qubit i = 0; i < n; ++i) {
    if (mappedFrom[i]) {
      continue;
    }

    for (dd::Qubit j = 0; j < n; ++j) {
      if (!mappedTo[j]) {
        pComp[i]    = j;
        mappedTo[j] = true;
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
  auto        nSimplifications = fullReduce();
  std::size_t newSimps{};
  do {
    miter.approximateCliffords(tolerance);
    newSimps = fullReduce();
    nSimplifications += newSimps;
  } while (!isDone() && (newSimps > 0U));
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
