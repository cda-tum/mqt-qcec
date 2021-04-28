/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#include "CompilationFlowEquivalenceChecker.hpp"

namespace ec {

    EquivalenceCheckingResults CompilationFlowEquivalenceChecker::check(const ec::Configuration& config) {
        EquivalenceCheckingResults results{};
        setupResults(results);
        results.strategy = Strategy::CompilationFlow;

        auto start = std::chrono::steady_clock::now();
        runPreCheckPasses(config);
        auto endPreprocessing = std::chrono::steady_clock::now();

        auto perm1     = initial1;
        auto perm2     = initial2;
        results.result = createInitialMatrix();

        while (it1 != end1 && it2 != end2) {
            // apply possible swaps
            while (it1 != end1 && (*it1)->getType() == qc::SWAP) {
                applyGate(*it1, results.result, perm1, LEFT);
                ++it1;
            }

            while (it2 != end2 && (*it2)->getType() == qc::SWAP) {
                applyGate(*it2, results.result, perm2, RIGHT);
                ++it2;
            }

            if (it1 != end1 && it2 != end2) {
                auto cost1 = costFunction((*it1)->getType(), (*it1)->getControls().size());
                auto cost2 = costFunction((*it2)->getType(), (*it2)->getControls().size());

                for (unsigned long long i = 0; i < cost2 && it1 != end1; ++i) {
                    applyGate(qc1, it1, results.result, perm1, LEFT);
                    ++it1;

                    // apply possible swaps
                    while (it1 != end1 && (*it1)->getType() == qc::SWAP) {
                        applyGate(*it1, results.result, perm1, LEFT);
                        ++it1;
                    }
                }

                for (unsigned long long i = 0; i < cost1 && it2 != end2; ++i) {
                    applyGate(qc2, it2, results.result, perm2, RIGHT);
                    ++it2;

                    // apply possible swaps
                    while (it2 != end2 && (*it2)->getType() == qc::SWAP) {
                        applyGate(*it2, results.result, perm2, RIGHT);
                        ++it2;
                    }
                }
            }
        }
        // finish first circuit
        while (it1 != end1) {
            applyGate(qc1, it1, results.result, perm1, LEFT);
            ++it1;
        }

        // finish second circuit
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

        auto goal_matrix    = createGoalMatrix();
        results.equivalence = equals(results.result, goal_matrix);

        auto                          endVerification   = std::chrono::steady_clock::now();
        std::chrono::duration<double> preprocessingTime = endPreprocessing - start;
        std::chrono::duration<double> verificationTime  = endVerification - endPreprocessing;
        results.preprocessingTime                       = preprocessingTime.count();
        results.verificationTime                        = verificationTime.count();
        results.maxActive                               = std::max(results.maxActive, dd->mUniqueTable.getMaxActiveNodes());

        return results;
    }

    unsigned long long IBMCostFunction(const qc::OpType& gate, unsigned short nc) {
        switch (gate) {
            case qc::I:
                return 1;

            case qc::X:
                if (nc <= 1)
                    return 1;
                else {
                    return 2 * (nc - 2) * (2 * IBMCostFunction(qc::Phase, 0) + 2 * IBMCostFunction(qc::U2, 0) + 3 * IBMCostFunction(qc::X, 1)) + 6 * IBMCostFunction(qc::X, 1) + 8 * IBMCostFunction(qc::U3, 0);
                }
                //if (nc == 3) return 2 * (4 * IBMCostFunction(qc::U1, 1) + 3 * IBMCostFunction(qc::X, 1)) + IBMCostFunction(qc::X, 2);
                //7 * IBMCostFunction(qc::U1, 1) + 14 * IBMCostFunction(qc::H, 0) + 6 * IBMCostFunction(qc::X, 1);
                //if (nc == 4) return
                //2 * IBMCostFunction(qc::U1, 1) + 4 * IBMCostFunction(qc::H, 0) + 3 * IBMCostFunction(qc::X, 3);
                //else {
                //	int n = std::ceil(nc / 2.);
                //	return 2 * IBMCostFunction(qc::X, n+1) + 2 * IBMCostFunction(qc::X, nc-n+1);
                //}

            case qc::U3:
            case qc::U2:
            case qc::V:
            case qc::Vdag:
            case qc::RX:
            case qc::RY:
            case qc::H:
            case qc::SX:
            case qc::SXdag:
                if (nc == 0) return 1;
                if (nc == 1)
                    return 2 * IBMCostFunction(qc::X, 1) + 4 * IBMCostFunction(qc::U3, 0);
                else
                    return 2 * IBMCostFunction(qc::X, nc) + 4 * IBMCostFunction(qc::U3, 0); // heuristic

            case qc::Phase:
            case qc::S:
            case qc::Sdag:
            case qc::T:
            case qc::Tdag:
            case qc::RZ:
                if (nc == 0) return 1;
                if (nc == 1)
                    return 2 * IBMCostFunction(qc::X, 1) + 3 * IBMCostFunction(qc::Phase, 0);
                else
                    return 2 * IBMCostFunction(qc::X, nc) + 3 * IBMCostFunction(qc::U3, 0); // heuristic

            case qc::Y:
            case qc::Z:
                if (nc == 0)
                    return 1;
                else
                    return IBMCostFunction(qc::X, nc) + 2 * IBMCostFunction(qc::U3, 0);

            case qc::SWAP:
                return IBMCostFunction(qc::X, nc) + 2 * IBMCostFunction(qc::X, 1);

            case qc::iSWAP:
                return IBMCostFunction(qc::SWAP, nc) + 2 * IBMCostFunction(qc::S, nc - 1) + IBMCostFunction(qc::Z, nc);

            case qc::Peres:
            case qc::Peresdag:
                return IBMCostFunction(qc::X, nc) + IBMCostFunction(qc::X, nc - 1);

            case qc::Compound: // this assumes that compound operations only arise from single qubit fusion
            case qc::Measure:
            case qc::Barrier:
            case qc::ShowProbabilities:
            case qc::Snapshot:
                return 1; // these operations are assumed to incur no cost, but to advance the procedure 1 is used

            default:
                std::cerr << "No cost for gate " << std::to_string(gate) << " -> assuming cost of 1!" << std::endl;
                return 1;
        }
    }
} // namespace ec
