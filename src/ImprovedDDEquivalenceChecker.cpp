/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include <ImprovedDDEquivalenceChecker.hpp>

namespace ec {
    qc::MatrixDD ImprovedDDEquivalenceChecker::createInitialMatrix() {
        auto e = dd->makeIdent(nqubits);
        dd->incRef(e);

        std::vector<bool> ancillary(nqubits);
        for (auto q = static_cast<dd::Qubit>(nqubits - 1); q >= 0; --q) {
            if (qc1.logicalQubitIsAncillary(q) && qc2.logicalQubitIsAncillary(q)) {
                bool found1  = false;
                bool isidle1 = false;
                for (const auto& in1: initial1) {
                    if (in1.second == q) {
                        found1  = true;
                        isidle1 = qc1.isIdleQubit(in1.first);
                        break;
                    }
                }
                bool found2  = false;
                bool isidle2 = false;
                for (const auto& in2: initial2) {
                    if (in2.second == q) {
                        found2  = true;
                        isidle2 = qc2.isIdleQubit(in2.first);
                        break;
                    }
                }

                // qubit only really exists or is acted on in one of the circuits
                if ((found1 ^ found2) || (isidle1 ^ isidle2)) {
                    ancillary[q] = true;
                }
            }
        }
        e = dd->reduceAncillae(e, ancillary);
        return e;
    }

    qc::MatrixDD ImprovedDDEquivalenceChecker::createGoalMatrix() {
        auto goalMatrix = dd->makeIdent(nqubits);
        dd->incRef(goalMatrix);
        goalMatrix = dd->reduceAncillae(goalMatrix, ancillary2, RIGHT);
        goalMatrix = dd->reduceGarbage(goalMatrix, garbage2, RIGHT);
        goalMatrix = dd->reduceAncillae(goalMatrix, ancillary1, LEFT);
        goalMatrix = dd->reduceGarbage(goalMatrix, garbage1, LEFT);
        return goalMatrix;
    }

    /// Use dedicated method to check the equivalence of both provided circuits
    EquivalenceCheckingResults ImprovedDDEquivalenceChecker::check(const Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);
        results.strategy = config.strategy;

        auto start = std::chrono::steady_clock::now();
        runPreCheckPasses(config);
        auto endPreprocessing = std::chrono::steady_clock::now();

        auto perm1     = initial1;
        auto perm2     = initial2;
        results.result = createInitialMatrix();

        switch (config.strategy) {
            case ec::Strategy::Naive:
                checkNaive(results.result, perm1, perm2);
                break;
            case ec::Strategy::Proportional:
                checkProportional(results.result, perm1, perm2);
                break;
            case ec::Strategy::Lookahead:
                checkLookahead(results.result, perm1, perm2);
                break;
            default:
                throw std::invalid_argument("Strategy " + toString(config.strategy) + " not supported by ImprovedDDEquivalenceChecker");
        }

        // finish first circuit
        while (it1 != end1) {
            applyGate(qc1, it1, results.result, perm1, LEFT);
            ++it1;
        }

        //finish second circuit
        while (it2 != end2) {
            applyGate(qc2, it2, results.result, perm2, RIGHT);
            ++it2;
        }

        qc::QuantumComputation::changePermutation(results.result, perm1, output1, dd, LEFT);
        qc::QuantumComputation::changePermutation(results.result, perm2, output2, dd, RIGHT);
        results.result = dd->reduceGarbage(results.result, garbage1, LEFT);
        results.result = dd->reduceGarbage(results.result, garbage2, RIGHT);
        results.result = dd->reduceAncillae(results.result, ancillary1, LEFT);
        results.result = dd->reduceAncillae(results.result, ancillary2, RIGHT);

        results.equivalence = equals(results.result, createGoalMatrix());
        results.maxActive   = std::max(results.maxActive, dd->mUniqueTable.getMaxActiveNodes());

        auto                          endVerification   = std::chrono::steady_clock::now();
        std::chrono::duration<double> preprocessingTime = endPreprocessing - start;
        std::chrono::duration<double> verificationTime  = endVerification - endPreprocessing;
        results.preprocessingTime                       = preprocessingTime.count();
        results.verificationTime                        = verificationTime.count();

        return results;
    }

    /// Alternate between LEFT and RIGHT applications
    void ImprovedDDEquivalenceChecker::checkNaive(qc::MatrixDD& result, qc::Permutation& perm1, qc::Permutation& perm2) {
        while (it1 != end1 && it2 != end2) {
            applyGate(qc1, it1, result, perm1, LEFT);
            ++it1;
            applyGate(qc2, it2, result, perm2, RIGHT);
            ++it2;
        }
    }

    /// Alternate according to the gate count ratio between LEFT and RIGHT applications
    void ImprovedDDEquivalenceChecker::checkProportional(qc::MatrixDD& result, qc::Permutation& perm1, qc::Permutation& perm2) {
        auto ratio  = static_cast<unsigned int>(std::round(
                static_cast<double>(std::max(qc1.getNops(), qc2.getNops())) /
                static_cast<double>(std::min(qc1.getNops(), qc2.getNops()))));
        auto ratio1 = (qc1.getNops() > qc2.getNops()) ? ratio : 1;
        auto ratio2 = (qc1.getNops() > qc2.getNops()) ? 1 : ratio;

        while (it1 != end1 && it2 != end2) {
            for (unsigned int i = 0; i < ratio1 && it1 != end1; ++i) {
                applyGate(qc1, it1, result, perm1, LEFT);
                ++it1;
            }
            for (unsigned int i = 0; i < ratio2 && it2 != end2; ++i) {
                applyGate(qc2, it2, result, perm2, RIGHT);
                ++it2;
            }
        }
    }

    /// Look-ahead LEFT and RIGHT and choose the more promising option
    void ImprovedDDEquivalenceChecker::checkLookahead(qc::MatrixDD& result, qc::Permutation& perm1, qc::Permutation& perm2) {
        qc::MatrixDD left{}, right{}, saved{};
        bool         cachedLeft = false, cachedRight = false;

        while (it1 != end1 && it2 != end2) {
            if (!cachedLeft) {
                // stop if measurement is encountered
                if ((*it1)->getType() == qc::Measure)
                    break;

                auto nq = (*it1)->getNqubits();
                (*it1)->setNqubits(nqubits);
                left = (*it1)->getDD(dd, perm1);
                dd->incRef(left);
                (*it1)->setNqubits(nq);
                ++it1;
                cachedLeft = true;
            }

            if (!cachedRight) {
                // stop if measurement is encountered
                if ((*it2)->getType() == qc::Measure)
                    break;

                auto nq = (*it2)->getNqubits();
                (*it2)->setNqubits(nqubits);
                right = (*it2)->getInverseDD(dd, perm2);
                dd->incRef(right);
                (*it2)->setNqubits(nq);
                ++it2;
                cachedRight = true;
            }

            saved          = result;
            auto lookLeft  = dd->multiply(left, saved);
            auto lookRight = dd->multiply(saved, right);

            auto nc1 = dd->size(lookLeft);
            auto nc2 = dd->size(lookRight);

            if (nc1 <= nc2) {
                result = lookLeft;
                dd->decRef(left);
                cachedLeft = false;
            } else {
                result = lookRight;
                dd->decRef(right);
                cachedRight = false;
            }
            dd->incRef(result);
            dd->decRef(saved);
            dd->garbageCollect();
        }

        if (cachedLeft) {
            saved  = result;
            result = dd->multiply(left, saved);
            dd->incRef(result);
            dd->decRef(saved);
            dd->decRef(left);
            dd->garbageCollect();
        }

        if (cachedRight) {
            saved  = result;
            result = dd->multiply(saved, right);
            dd->incRef(result);
            dd->decRef(saved);
            dd->decRef(right);
            dd->garbageCollect();
        }
    }
} // namespace ec
