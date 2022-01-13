/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDSIMULATIONCHECKER_HPP
#define QCEC_DDSIMULATIONCHECKER_HPP

#include "checker/DDEquivalenceChecker.hpp"

namespace ec {
    class DDSimulationChecker: public DDEquivalenceChecker<qc::VectorDD> {
    public:
        DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, bool& done);

        DDSimulationChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, bool& done, const qc::VectorDD& initialState);

        [[nodiscard]] const qc::VectorDD& getInitialState() const { return initialState; }

        void setInitialState(const qc::VectorDD& state) { initialState = state; }

        void setRandomInitialState(StateGenerator& generator);

        [[nodiscard]] dd::CVec getInitialVector() const { return dd->getVector(initialState); }
        [[nodiscard]] dd::CVec getInternalVector1() const { return dd->getVector(taskManager1.getInternalState()); }
        [[nodiscard]] dd::CVec getInternalVector2() const { return dd->getVector(taskManager2.getInternalState()); }

        void json(nlohmann::json& j) const override {
            DDEquivalenceChecker::json(j);
            j["checker"] = "decision_diagram_simulation";
        }

    protected:
        // the initial state used for simulation. defaults to the all-zero state |0...0>
        qc::VectorDD initialState{};

        void                 initializeTask(TaskManager<qc::VectorDD>& task) override;
        EquivalenceCriterion checkEquivalence() override;
    };
} // namespace ec

#endif //QCEC_DDSIMULATIONCHECKER_HPP
