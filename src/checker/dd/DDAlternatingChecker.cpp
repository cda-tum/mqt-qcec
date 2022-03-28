/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#include "checker/dd/DDAlternatingChecker.hpp"

namespace ec {
    void DDAlternatingChecker::initialize() {
        DDEquivalenceChecker::initialize();
        // create the full identity matrix
        functionality = dd->makeIdent(nqubits);
        dd->incRef(functionality);

        // only count ancillaries that are present in but not acted upon in both of the circuits
        // at the moment this is just to be on the safe side. It might be fine to also start with the
        // reduced matrix for every ancillary without any restriction
        // TODO: check whether the way ancillaries are handled here is theoretically sound
        std::vector<bool> ancillary(nqubits);
        for (auto q = static_cast<dd::Qubit>(nqubits - 1U); q >= 0; --q) {
            if (qc1.logicalQubitIsAncillary(q) && qc2.logicalQubitIsAncillary(q)) {
                bool found1  = false;
                bool isIdle1 = false;
                for (const auto& in1: qc1.initialLayout) {
                    if (in1.second == q) {
                        found1  = true;
                        isIdle1 = qc1.isIdleQubit(in1.first);
                        break;
                    }
                }
                bool found2  = false;
                bool isIdle2 = false;
                for (const auto& in2: qc2.initialLayout) {
                    if (in2.second == q) {
                        found2  = true;
                        isIdle2 = qc2.isIdleQubit(in2.first);
                        break;
                    }
                }

                // qubit only really exists or is acted on in one of the circuits
                if ((found1 != found2) || (isIdle1 != isIdle2)) {
                    ancillary[q] = true;
                }
            }
        }

        // reduce the ancillary qubit contributions
        // [1 0] if the qubit is no ancillary, or it is acted upon by both circuits
        // [0 1]
        //
        // [1 0] for an ancillary that is present in one circuit and not acted upon in the other
        // [0 0]
        functionality = dd->reduceAncillae(functionality, ancillary);
    }

    void DDAlternatingChecker::execute() {
        while (!taskManager1.finished() && !taskManager2.finished() && !isDone()) {
            // skip over any SWAP operations
            taskManager1.applySwapOperations(functionality);
            taskManager2.applySwapOperations(functionality);

            if (!taskManager1.finished() && !taskManager2.finished()) {
                if (isDone()) { return; }

                // whenever the current functionality resembles the identity, identical gates on both sides cancel
                if (functionality.p->ident && configuration.application.alternatingScheme != ApplicationSchemeType::Lookahead && gatesAreIdentical()) {
                    taskManager1.advanceIterator();
                    taskManager2.advanceIterator();
                    continue;
                }

                // query application scheme on how to proceed
                const auto [apply1, apply2] = (*applicationScheme)();

                // advance both tasks correspondingly
                if (isDone()) { return; }
                taskManager1.advance(functionality, apply1);
                if (isDone()) { return; }
                taskManager2.advance(functionality, apply2);
            }
        }
    }

    void DDAlternatingChecker::finish() {
        taskManager1.finish(functionality);
        if (isDone()) { return; }
        taskManager2.finish(functionality);
    }

    void DDAlternatingChecker::postprocess() {
        // ensure that the permutations that were tracked throughout the circuit match the expected output permutations
        taskManager1.changePermutation(functionality);
        if (isDone()) { return; }
        taskManager2.changePermutation(functionality);
        if (done) { return; }

        // sum up the contributions of garbage qubits
        taskManager1.reduceGarbage(functionality);
        if (isDone()) { return; }
        taskManager2.reduceGarbage(functionality);
        if (isDone()) { return; }

        // TODO: check whether reducing ancillaries here is theoretically sound
        taskManager1.reduceAncillae(functionality);
        if (isDone()) { return; }
        taskManager2.reduceAncillae(functionality);
        if (isDone()) { return; }
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

    bool DDAlternatingChecker::gatesAreIdentical() {
        if (taskManager1.finished() || taskManager2.finished()) {
            return false;
        }

        const auto& op1 = *taskManager1();
        const auto& op2 = *taskManager2();

        return op1.equals(op2);
    }

} // namespace ec
