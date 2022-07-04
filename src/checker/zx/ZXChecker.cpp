#include "checker/zx/ZXChecker.hpp"

#include "Definitions.hpp"
#include "QuantumComputation.hpp"
#include "Simplify.hpp"
#include "ZXDiagram.hpp"
#include "dd/Definitions.hpp"
#include "zx/FunctionalityConstruction.hpp"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <optional>
#include <unordered_set>

namespace ec {
    ZXEquivalenceChecker::ZXEquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration) noexcept:
        EquivalenceChecker(qc1, qc2, std::move(configuration)), miter(zx::FunctionalityConstruction::buildFunctionality(&qc1)), tolerance(configuration.functionality.traceThreshold) {
        zx::ZXDiagram dPrime = zx::FunctionalityConstruction::buildFunctionality(&qc2);

        if (qc1.getNancillae() != 0 || qc2.getNancillae() != 0)
            ancilla = true;

        const auto& p1 = invertPermutations(qc1);
        const auto& p2 = invertPermutations(qc2);

        // fix ancillaries to |0>
        for (auto anc = static_cast<dd::Qubit>(qc1.getNqubits() - 1); anc >= qc1.getNqubitsWithoutAncillae(); --anc) {
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

            for (std::size_t i = 0; i < miter.getNQubits(); ++i) {
                const auto& in  = miter.getInput(i);
                const auto& out = miter.incidentEdge(in, 0).to;

                if (p1.at(static_cast<dd::Qubit>(miter.getVData(in).value().qubit)) != p2.at(static_cast<dd::Qubit>(miter.getVData(out).value().qubit))) {
                    equivalent = false;
                    break;
                }
            }
        } else {
            equivalent = false;
        }

        const auto end = std::chrono::steady_clock::now();
        runtime += std::chrono::duration<double>(end - start).count();

        // non-equivalence might be due to incorrect assumption about the state of ancillaries or the check was aborted prematurely, so no information can be given
        if ((!equivalent && ancilla) || isDone())
            equivalence = EquivalenceCriterion::NoInformation;
        else
            equivalence = equivalent ? EquivalenceCriterion::EquivalentUpToGlobalPhase : EquivalenceCriterion::ProbablyNotEquivalent;

        return equivalence;
    }

    qc::Permutation invert(const qc::Permutation& p) {
        qc::Permutation pInv{};
        for (auto [v, w]: p) {
            pInv[w] = v;
        }
        return pInv;
    }
    qc::Permutation concat(const qc::Permutation& p1, const qc::Permutation& p2) { //p2 after p1
        qc::Permutation pComb{};

        for (auto [v, w]: p1) {
            if (p2.find(w) != p2.end())
                pComb[v] = p2.at(w);
        }
        return pComb;
    }

    qc::Permutation complete(const qc::Permutation& p, dd::Qubit n) {
        qc::Permutation pComp = p;

        std::vector<bool> mappedTo(n, false);
        std::vector<bool> mappedFrom(n, false);
        for (auto [k, v]: p) {
            mappedFrom[k] = true;
            mappedTo[v]   = true;
        }

        //Try to map identity
        for (dd::Qubit i = 0; i < n; ++i) {
            if (mappedFrom[i] || mappedTo[i]) continue;

            pComp[i]      = i;
            mappedFrom[i] = true;
            mappedTo[i]   = true;
        }

        // Try to map inverse
        for (dd::Qubit i = 0; i < n; ++i) {
            if (mappedFrom[i]) continue;
            std::optional<dd::Qubit> j;
            for (auto [k, v]: p) {
                if (v == i) {
                    j = k;
                    break;
                }
            }

            if (mappedTo[j.value()]) continue;

            pComp[i]            = j.value();
            mappedFrom[i]       = true;
            mappedTo[j.value()] = true;
        }

        //Map rest greedily

        for (dd::Qubit i = 0; i < n; ++i) {
            if (mappedFrom[i]) continue;

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
        return concat(
                invert(complete(qc.outputPermutation, static_cast<dd::Qubit>(qc.getNqubits()))),
                complete(qc.initialLayout, static_cast<dd::Qubit>(qc.getNqubits())));
    }

    std::size_t ZXEquivalenceChecker::fullReduceApproximate() {
        auto        nSimplifications = fullReduce();
        std::size_t newSimps;
        do {
            miter.approximateCliffords(tolerance);
            newSimps = fullReduce();
            nSimplifications += newSimps;
        } while (!isDone() && (newSimps > 0));
        return nSimplifications;
    }

    std::size_t ZXEquivalenceChecker::fullReduce() {
        if (!isDone()) {
            miter.toGraphlike();
        }
        interiorCliffordSimp();

        std::size_t nSimplifications = 0;
        while (!isDone()) {
            cliffordSimp();
            const auto nGadget = gadgetSimp();
            interiorCliffordSimp();
            const auto nPivot = pivotGadgetSimp();
            if (nGadget + nPivot == 0)
                break;
            nSimplifications += nGadget + nPivot;
        }
        if (!isDone()) {
            miter.removeDisconnectedSpiders();
        }

        return nSimplifications;
    }

    std::size_t ZXEquivalenceChecker::gadgetSimp() {
        std::size_t nSimplifications = 0;
        bool        new_matches      = true;

        while (!isDone() && new_matches) {
            new_matches = false;
            for (const auto& [v, _]: miter.getVertices()) {
                if (miter.isDeleted(v))
                    continue;

                if (!isDone() && checkAndFuseGadget(miter, v)) {
                    new_matches = true;
                    nSimplifications++;
                }
            }
        }
        return nSimplifications;
    }

    std::size_t ZXEquivalenceChecker::interiorCliffordSimp() {
        spiderSimp();

        bool        newMatches       = true;
        std::size_t nSimplifications = 0;
        while (!isDone() && newMatches) {
            newMatches            = false;
            const auto nId        = idSimp();
            const auto nSpider    = spiderSimp();
            const auto nPivot     = pivotPauliSimp();
            const auto nLocalComp = localCompSimp();

            if (nId + nSpider + nPivot + nLocalComp != 0) {
                newMatches = true;
                nSimplifications++;
            }
        }
        return nSimplifications;
    }

    std::size_t ZXEquivalenceChecker::cliffordSimp() {
        bool        newMatches       = true;
        std::size_t nSimplifications = 0;
        while (!isDone() && newMatches) {
            newMatches           = false;
            const auto nClifford = interiorCliffordSimp();
            const auto nPivot    = pivotSimp();
            if (nClifford + nPivot != 0) {
                newMatches = true;
                nSimplifications++;
            }
        }
        return nSimplifications;
    }

} // namespace ec
