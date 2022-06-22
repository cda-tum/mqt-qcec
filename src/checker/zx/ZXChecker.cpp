#include "checker/zx/ZXChecker.hpp"

#include "Simplify.hpp"
#include "ZXDiagram.hpp"
#include "zx/FunctionalityConstruction.hpp"

#include <chrono>

namespace ec {
    ZXEquivalenceChecker::ZXEquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration) noexcept:
        EquivalenceChecker(qc1, qc2, std::move(configuration)), miter(zx::FunctionalityConstruction::buildFunctionality(&qc1)) {
        zx::ZXDiagram dPrime = zx::FunctionalityConstruction::buildFunctionality(&qc2);
        miter.invert();
        miter.concat(dPrime);
    }

    EquivalenceCriterion ZXEquivalenceChecker::run() {
        const auto start = std::chrono::steady_clock::now();
        zx::fullReduce(miter);
        bool equivalent = true;

        if (miter.getNEdges() == miter.getNQubits()) {
            // check if permutation of miter matches output permutations of input circuits
            const qc::Permutation& p1 = qc1.outputPermutation;
            const qc::Permutation& p2 = qc2.outputPermutation;
            qc::Permutation        p;
            for (auto& [from_0, to_0]: p1) {
                auto to   = to_0; //the standard is weird here regarding capturing structure bound variables
                auto k_v  = std::find_if(p2.begin(), p2.end(), [&](auto& k_v) { return k_v.second == to; });
                p[from_0] = k_v->first;
            }

            for (auto& [in, out]: p) {
                if (!miter.connected(miter.getInputs()[in], miter.getOutputs()[out])) {
                    equivalent = false;
                }
            }
        } else {
            equivalent = false;
        }

        const auto end = std::chrono::steady_clock::now();
        runtime += std::chrono::duration<double>(end - start).count();

        equivalence = equivalent ? EquivalenceCriterion::EquivalentUpToGlobalPhase : EquivalenceCriterion::ProbablyNotEquivalent;
        return equivalence;
    }
} // namespace ec
