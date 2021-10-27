/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDSIMULATIONCHECKER_HPP
#define QCEC_DDSIMULATIONCHECKER_HPP

#include "checker/EquivalenceChecker.hpp"

namespace ec {
    class DDSimulationChecker: public EquivalenceChecker<qc::VectorDD> {
    public:
        DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration);

        DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, const qc::VectorDD& initialState);

    protected:
        // the initial state used for simulation. defaults to the all-zero state |0...0>
        qc::VectorDD initialState{};

        void initializeTask(TaskManager<qc::VectorDD>& task) override;
    };
} // namespace ec

#endif //QCEC_DDSIMULATIONCHECKER_HPP
