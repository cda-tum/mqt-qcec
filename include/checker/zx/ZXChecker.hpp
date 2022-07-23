//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "Definitions.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "Simplify.hpp"
#include "ZXDiagram.hpp"
#include "checker/EquivalenceChecker.hpp"
#include "nlohmann/json.hpp"

namespace ec {
class ZXEquivalenceChecker : public EquivalenceChecker {
public:
  ZXEquivalenceChecker(const qc::QuantumComputation& qc1,
                       const qc::QuantumComputation& qc2,
                       Configuration                 configuration) noexcept;

  EquivalenceCriterion run() override;

  void json(nlohmann::json& j) const noexcept override {
    EquivalenceChecker::json(j);
    j["checker"] = "zx";
  }

private:
  zx::ZXDiagram miter;
  zx::fp        tolerance;
  bool          ancilla = false;

  // the following methods are adaptations of the core ZX simplification
  // routines that additionally check a criterion for early termination of the
  // simplification.
  std::size_t fullReduceApproximate();
  std::size_t fullReduce();

  std::size_t gadgetSimp();
  std::size_t interiorCliffordSimp();
  std::size_t cliffordSimp();

  std::size_t idSimp() {
    return simplifyVertices(zx::checkIdSimp, zx::removeId);
  }

  std::size_t spiderSimp() {
    return simplifyEdges(zx::checkSpiderFusion, zx::fuseSpiders);
  }

  std::size_t localCompSimp() {
    return simplifyVertices(zx::checkLocalComp, zx::localComp);
  }

  std::size_t pivotPauliSimp() {
    return simplifyEdges(zx::checkPivotPauli, zx::pivotPauli);
  }

  std::size_t pivotSimp() { return simplifyEdges(zx::checkPivot, zx::pivot); }

  std::size_t pivotGadgetSimp() {
    return simplifyEdges(zx::checkPivotGadget, zx::pivotGadget);
  }

  template <class CheckFun = zx::VertexCheckFun,
            class RuleFun  = zx::VertexRuleFun>
  std::size_t simplifyVertices(CheckFun check, RuleFun rule) {
    std::size_t nSimplifications = 0U;
    bool        newMatches       = true;

    while (!isDone() && newMatches) {
      newMatches = false;
      for (const auto& [v, _] : miter.getVertices()) {
        if (isDone() || !check(miter, v)) {
          continue;
        }
        rule(miter, v);
        newMatches = true;
        ++nSimplifications;
      }
    }
    return nSimplifications;
  }

  template <class CheckFun = zx::EdgeCheckFun, class RuleFun = zx::EdgeRuleFun>
  std::size_t simplifyEdges(CheckFun check, RuleFun rule) {
    std::size_t nSimplifications = 0U;
    bool        newMatches       = true;

    while (!isDone() && newMatches) {
      newMatches = false;
      for (const auto& [v0, v1] : miter.getEdges()) {
        if (isDone() || miter.isDeleted(v0) || miter.isDeleted(v1) ||
            !check(miter, v0, v1)) {
          continue;
        }
        rule(miter, v0, v1);
        newMatches = true;
        ++nSimplifications;
      }
    }
    return nSimplifications;
  }
};

qc::Permutation complete(const qc::Permutation& p, dd::QubitCount n);
qc::Permutation concat(const qc::Permutation& p1, const qc::Permutation& p2);
qc::Permutation invert(const qc::Permutation& p);
qc::Permutation invertPermutations(const qc::QuantumComputation& qc);
} // namespace ec
