/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"

#include <utility>

namespace ec {
    class EquivalenceChecker {
    public:
        EquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration, bool& done):
            qc1(qc1), qc2(qc2),
            nqubits(std::max(qc1.getNqubits(), qc2.getNqubits())),
            configuration(std::move(configuration)),
            done(done){};

        virtual ~EquivalenceChecker() = default;

        virtual EquivalenceCriterion run() = 0;

        [[nodiscard]] const Configuration& getConfiguration() const {
            return configuration;
        }
        [[nodiscard]] EquivalenceCriterion getEquivalence() const {
            return equivalence;
        }
        [[nodiscard]] double getRuntime() const {
            return runtime;
        }

        virtual void json(nlohmann::json& j) const {
            j["equivalence"] = toString(equivalence);
            j["runtime"]     = runtime;
        }

    protected:
        const qc::QuantumComputation& qc1;
        const qc::QuantumComputation& qc2;

        dd::QubitCount nqubits{};

        Configuration configuration;

        bool& done;

        EquivalenceCriterion equivalence = EquivalenceCriterion::NoInformation;
        double               runtime{};
    };

} // namespace ec
