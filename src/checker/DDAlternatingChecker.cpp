/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "checker/DDAlternatingChecker.hpp"

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
        while (!taskManager1.finished() && !taskManager2.finished() && !done) {
            // skip over any SWAP operations
            taskManager1.applySwapOperations(functionality);
            taskManager2.applySwapOperations(functionality);

            if (!taskManager1.finished() && !taskManager2.finished()) {
                if (done) return;

                // whenever the current functionality resembles the identity, identical gates on both sides cancel
                if (functionality.p->ident && configuration.application.scheme != ApplicationSchemeType::Lookahead && gatesAreIdentical()) {
                    taskManager1.advanceIterator();
                    taskManager2.advanceIterator();
                    continue;
                }

                // query application scheme on how to proceed
                const auto [apply1, apply2] = (*applicationScheme)();

                // advance both tasks correspondingly
                if (done) return;
                taskManager1.advance(functionality, apply1);
                if (done) return;
                taskManager2.advance(functionality, apply2);
            }
        }
    }

    void DDAlternatingChecker::finish() {
        taskManager1.finish(functionality);
        if (done) return;
        taskManager2.finish(functionality);
    }

    void DDAlternatingChecker::postprocess() {
        // ensure that the permutations that were tracked throughout the circuit match the expected output permutations
        taskManager1.changePermutation(functionality);
        if (done) return;
        taskManager2.changePermutation(functionality);
        if (done) return;

        // sum up the contributions of garbage qubits
        taskManager1.reduceGarbage(functionality);
        if (done) return;
        taskManager2.reduceGarbage(functionality);
        if (done) return;

        // TODO: check whether reducing ancillaries here is theoretically sound
        taskManager1.reduceAncillae(functionality);
        if (done) return;
        taskManager2.reduceAncillae(functionality);
        if (done) return;
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
        if (taskManager1.finished() || taskManager2.finished())
            return false;

        const auto& op1 = *taskManager1();
        const auto& op2 = *taskManager2();

        // check type
        if (op1.getType() != op2.getType()) {
            return false;
        }

        // check number of controls
        const auto nc1 = op1.getNcontrols();
        const auto nc2 = op2.getNcontrols();
        if (nc1 != nc2) {
            return false;
        }

        // SWAPs are not handled by this routine
        if (op1.getType() == qc::SWAP && nc1 == 0) {
            return false;
        }

        // check parameters
        const auto param1 = op1.getParameter();
        const auto param2 = op2.getParameter();
        for (std::size_t p = 0; p < qc::MAX_PARAMETERS; ++p) {
            // it might make sense to use fuzzy comparison here
            if (param1[p] != param2[p]) {
                return false;
            }
        }

        // check controls
        if (nc1 != 0) {
            dd::Controls controls1{};
            for (const auto& control: op1.getControls()) {
                controls1.emplace(dd::Control{taskManager1.getPermutation().at(control.qubit), control.type});
            }
            dd::Controls controls2{};
            for (const auto& control: op2.getControls()) {
                controls2.emplace(dd::Control{taskManager2.getPermutation().at(control.qubit), control.type});
            }
            if (controls1 != controls2) {
                return false;
            }
        }

        // check targets
        std::set<dd::Qubit> targets1{};
        for (const auto& target: op1.getTargets()) {
            targets1.emplace(taskManager1.getPermutation().at(target));
        }
        std::set<dd::Qubit> targets2{};
        for (const auto& target: op2.getTargets()) {
            targets2.emplace(taskManager2.getPermutation().at(target));
        }
        if (targets1 != targets2) {
            return false;
        }

        // operations are identical
        return true;
    }

} // namespace ec
