/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_DDCONSTRUCTIONCHECKER_HPP
#define QCEC_DDCONSTRUCTIONCHECKER_HPP

#include "checker/DDEquivalenceChecker.hpp"

namespace ec {
    class DDConstructionChecker: public DDEquivalenceChecker<qc::MatrixDD> {
    public:
        DDConstructionChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const ec::Configuration& configuration, bool& done):
            DDEquivalenceChecker(qc1, qc2, configuration, done) {}

        void json(nlohmann::json& j) const override {
            DDEquivalenceChecker::json(j);
            j["checker"] = "decision_diagram_construction";
        }

    protected:
        void initializeTask(TaskManager<qc::MatrixDD>& task) override;
    };
} // namespace ec

#endif //QCEC_DDCONSTRUCTIONCHECKER_HPP
