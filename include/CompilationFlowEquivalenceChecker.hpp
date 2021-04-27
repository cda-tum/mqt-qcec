/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP
#define QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP

#include "ImprovedDDEquivalenceChecker.hpp"

#include <functional>
#include <unordered_map>
#include <utility>

namespace ec {
    using CostFunction = std::function<unsigned long long(const qc::OpType&, unsigned short)>;

    unsigned long long IBMCostFunction(const qc::OpType& gate, unsigned short nc);

    class CompilationFlowEquivalenceChecker: public ImprovedDDEquivalenceChecker {
        CostFunction costFunction;

    public:
        CompilationFlowEquivalenceChecker(qc::QuantumComputation& qc1, qc::QuantumComputation& qc2, CostFunction costFunction = IBMCostFunction):
            ImprovedDDEquivalenceChecker(qc1, qc2), costFunction(std::move(costFunction)) {}

        EquivalenceCheckingResults check(const Configuration& config) override;
        EquivalenceCheckingResults check() override { return check(Configuration{}); }
    };
} // namespace ec

#endif //QCEC_COMPILATIONFLOWEQUIVALENCECHECKER_HPP
