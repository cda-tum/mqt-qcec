/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDSIMULATIONCHECKER_HPP
#define QCEC_DDSIMULATIONCHECKER_HPP

#include "EquivalenceChecker.hpp"

namespace ec {
    class DDSimulationChecker: public EquivalenceChecker {
    public:
        template<class... Args>
        DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, Args... args):
            EquivalenceChecker(qc1, qc2, configuration, args...) {
            initialState = dd->makeZeroState(this->qc1.getNqubits());
        }

        template<class... Args>
        DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const qc::VectorDD& initialState, const ec::Configuration& configuration, Args... args):
            EquivalenceChecker(qc1, qc2, configuration, args...), initialState(initialState) {}

        EquivalenceCriterion run() override;

    protected:
        // the initial state used for simulation. defaults to the all-zero state |0...0>
        qc::VectorDD initialState{};

        struct SimulationTask {
            const qc::QuantumComputation* qc;
            qc::Permutation               permutation{};
            decltype(qc->begin())         iterator;
            decltype(qc->end())           end;
            qc::VectorDD                  state{};

            SimulationTask(const qc::QuantumComputation& qc, const qc::VectorDD& initialState):
                qc(&qc) {
                state       = initialState;
                permutation = qc.initialLayout;
                iterator    = qc.begin();
                end         = qc.end();
            }

            [[nodiscard]] bool finished() const { return iterator == end; }
        };

        SimulationTask task1;
        SimulationTask task2;

        void setupSimulationTask(const qc::QuantumComputation& qc, SimulationTask& task);

        void advanceSimulation(SimulationTask& task);

        void postprocess(SimulationTask& task);
    };
} // namespace ec

#endif //QCEC_DDSIMULATIONCHECKER_HPP
