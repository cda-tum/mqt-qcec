/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_EQUIVALENCECHECKINGMANAGER_HPP
#define QCEC_EQUIVALENCECHECKINGMANAGER_HPP

#include "CircuitOptimizer.hpp"
#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "construction/DDConstructionChecker.hpp"
#include "simulation/DDSimulationChecker.hpp"
#include "simulation/StateGenerator.hpp"

#include <chrono>
#include <thread>

namespace ec {

    class EquivalenceCheckingManager {
    public:
        EquivalenceCheckingManager(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration = Configuration{});

        static void setTolerance(dd::fp tol) { dd::ComplexTable<>::setTolerance(tol); }

        EquivalenceCriterion check();

    protected:
        qc::QuantumComputation qc1{};
        qc::QuantumComputation qc2{};

        Configuration configuration{};

        std::size_t  gateRatio           = 1U;
        CostFunction defaultCostFunction = [&](const qc::QuantumComputation&, const qc::QuantumComputation&, const std::unique_ptr<qc::Operation>&) { return gateRatio; };

        StateGenerator stimuliGenerator;

        double preprocessingTime{};
        double checkTime{};

        /// Given that one circuit has more qubits than the other, the difference is assumed to arise from ancillary qubits.
        /// This function changes the additional qubits in the larger circuit to ancillary qubits.
        /// Furthermore it adds corresponding ancillaries in the smaller circuit
        void setupAncillariesAndGarbage();

        /// In some cases both circuits calculate the same function, but on different qubits.
        /// This function tries to correct such mismatches.
        /// Note that this is still highly experimental!
        void fixOutputPermutationMismatch();

        /// Run all configured optimization passes
        void runOptimizationPasses();
    };
} // namespace ec

#endif //QCEC_EQUIVALENCECHECKINGMANAGER_HPP
