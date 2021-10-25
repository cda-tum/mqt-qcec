/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "alternating/DDAlternatingChecker.hpp"

namespace ec {
    void DDAlternatingChecker::initialize() {
        // create the full identity matrix
        functionality = dd->makeIdent(nqubits);
        dd->incRef(functionality);

        // only count ancillaries that are present in but not acted upon in both of the circuits
        // at the moment this is just to be on the safe side. It might be fine to also start with the
        // reduced matrix for every ancillary without any restriction
        // TODO: check whether the way ancillaries are handled here is theoretically sound
        std::vector<bool> ancillary(nqubits);
        for (auto q = static_cast<dd::Qubit>(nqubits - 1); q >= 0; --q) {
            if (qc1.logicalQubitIsAncillary(q) && qc2.logicalQubitIsAncillary(q)) {
                bool found1  = false;
                bool isidle1 = false;
                for (const auto& in1: qc1.initialLayout) {
                    if (in1.second == q) {
                        found1  = true;
                        isidle1 = qc1.isIdleQubit(in1.first);
                        break;
                    }
                }
                bool found2  = false;
                bool isidle2 = false;
                for (const auto& in2: qc2.initialLayout) {
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

        // reduce the ancillary qubit contributions
        // [1 0] if the qubit is no ancillary or it is acted upon by both circuits
        // [0 1]
        //
        // [1 0] for an ancillary that is present in one circuit and not acted upon in the other
        // [0 0]
        functionality = dd->reduceAncillae(functionality, ancillary);
    }

    void DDAlternatingChecker::execute() {
        if (scheme == AlternatingScheme::CostFunction) {
            executeCostFunction();
        } else if (scheme == AlternatingScheme::Lookahead) {
            executeLookahead();
        }
    }

    void DDAlternatingChecker::executeCostFunction() {
        while (!taskManager1.finished() && !taskManager2.finished()) {
            // skip over any SWAP operations
            taskManager1.applySwapOperations(functionality);
            taskManager2.applySwapOperations(functionality);

            if (!taskManager1.finished() && !taskManager2.finished()) {
                // query application scheme on how to proceed
                const auto [apply1, apply2] = (*applicationScheme)();

                // advance both tasks correspondingly
                taskManager1.advance(functionality, apply1);
                taskManager2.advance(functionality, apply2);
            }
        }
    }

    void DDAlternatingChecker::executeLookahead() {
        qc::MatrixDD left{}, right{}, saved{};
        bool         cachedLeft = false, cachedRight = false;

        while (!taskManager1.finished() && !taskManager2.finished()) {
            if (!cachedLeft) {
                // skip over any SWAP operations
                taskManager1.applySwapOperations(functionality);

                // break if the job is finished
                if (taskManager1.finished())
                    break;

                // cache the current left DD
                left = taskManager1.getDD();
                dd->incRef(left);
                taskManager1.advanceIterator();
                cachedLeft = true;
            }

            if (!cachedRight) {
                // skip over any SWAP operations
                taskManager2.applySwapOperations(functionality);

                // break if the job is finished
                if (taskManager2.finished())
                    break;

                // cache the current right DD
                right = taskManager2.getInverseDD();
                dd->incRef(right);
                taskManager2.advanceIterator();
                cachedRight = true;
            }

            saved          = functionality;
            auto lookLeft  = dd->multiply(left, saved);
            auto lookRight = dd->multiply(saved, right);

            auto nc1 = dd->size(lookLeft);
            auto nc2 = dd->size(lookRight);

            if (nc1 <= nc2) {
                functionality = lookLeft;
                dd->decRef(left);
                cachedLeft = false;
            } else {
                functionality = lookRight;
                dd->decRef(right);
                cachedRight = false;
            }
            dd->incRef(functionality);
            dd->decRef(saved);
            dd->garbageCollect();
        }

        if (cachedLeft) {
            saved         = functionality;
            functionality = dd->multiply(left, saved);
            dd->incRef(functionality);
            dd->decRef(saved);
            dd->decRef(left);
            dd->garbageCollect();
        }

        if (cachedRight) {
            saved         = functionality;
            functionality = dd->multiply(saved, right);
            dd->incRef(functionality);
            dd->decRef(saved);
            dd->decRef(right);
            dd->garbageCollect();
        }
    }

    void DDAlternatingChecker::finish() {
        taskManager1.finish(functionality);
        taskManager2.finish(functionality);
    }

    void DDAlternatingChecker::postprocess() {
        // ensure that the permutations that were tracked throughout the circuit match the expected output permutations
        taskManager1.changePermutation(functionality);
        taskManager2.changePermutation(functionality);

        // sum up the contributions of garbage qubits
        taskManager1.reduceGarbage(functionality);
        taskManager2.reduceGarbage(functionality);

        // TODO: check whether reducing ancillaries here is theoretically sound
        taskManager1.reduceAncillae(functionality);
        taskManager2.reduceAncillae(functionality);
    }

    EquivalenceCriterion DDAlternatingChecker::checkEquivalence() {
        // create the full identity matrix
        auto goalMatrix = dd->makeIdent(nqubits);
        dd->incRef(goalMatrix);

        // account for any garbage
        taskManager1.reduceGarbage(goalMatrix);
        taskManager2.reduceGarbage(goalMatrix);

        // TODO: check whether reducing ancillaries here is theoretically sound
        taskManager1.reduceAncillae(goalMatrix);
        taskManager2.reduceAncillae(goalMatrix);

        // the resulting goal matrix is
        // [1 0] if the qubit is no ancillary
        // [0 1]
        //
        // [1 0] for an ancillary that is present in either circuit
        // [0 0]

        // compare the obtained functionality to the goal matrix
        return equals(functionality, goalMatrix);
    }

} // namespace ec
