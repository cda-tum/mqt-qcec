/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_IMPROVEDDDEQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_IMPROVEDDDEQUIVALENCECHECKER_HPP

#include "EquivalenceChecker.hpp"

#include <array>
#include <chrono>
#include <memory>
#include <unordered_set>

namespace ec {

    class ImprovedDDEquivalenceChecker: public EquivalenceChecker {
        /// Alternate between LEFT and RIGHT applications
        void checkNaive(qc::MatrixDD& result, qc::Permutation& perm1, qc::Permutation& perm2);
        /// Alternate according to the gate count ratio between LEFT and RIGHT applications
        void checkProportional(qc::MatrixDD& result, qc::Permutation& perm1, qc::Permutation& perm2);
        /// Look-ahead LEFT and RIGHT and choose the more promising option
        void checkLookahead(qc::MatrixDD& result, qc::Permutation& perm1, qc::Permutation& perm2);

    protected:
        /// Create the initial matrix used for the G->I<-G' scheme.
        /// [1 0] if the qubit is no ancillary or it is acted upon by both circuits
        /// [0 1]
        ///
        /// [1 0] for an ancillary that is present in one circuit and not acted upon in the other
        /// [0 0]
        /// \return initial matrix
        qc::MatrixDD createInitialMatrix();

        bool gatesAreIdentical(const qc::Permutation& perm1, const qc::Permutation& perm2) {
            // exit prematurely whenever one of the circuits is already finished
            if (it1 == end1 || it2 == end2)
                return false;

            const auto& op1 = **it1;
            const auto& op2 = **it2;

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
                    controls1.emplace(dd::Control{perm1.at(control.qubit), control.type});
                }
                dd::Controls controls2{};
                for (const auto& control: op2.getControls()) {
                    controls2.emplace(dd::Control{perm2.at(control.qubit), control.type});
                }
                if (controls1 != controls2) {
                    return false;
                }
            }

            // check targets
            std::set<dd::Qubit> targets1{};
            for (const auto& target: op1.getTargets()) {
                targets1.emplace(perm1.at(target));
            }
            std::set<dd::Qubit> targets2{};
            for (const auto& target: op2.getTargets()) {
                targets2.emplace(perm2.at(target));
            }
            if (targets1 != targets2) {
                return false;
            }

            // operations are identical
            return true;
        }

    public:
        ImprovedDDEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2):
            EquivalenceChecker(qc1, qc2) {
            method = Method::G_I_Gp;
        }

        /// Use dedicated method to check the equivalence of both provided circuits
        EquivalenceCheckingResults check(const Configuration& config) override;
        EquivalenceCheckingResults check() override { return check(Configuration{}); }
    };

} // namespace ec

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_IMPROVEDDDEQUIVALENCECHECKER_HPP
