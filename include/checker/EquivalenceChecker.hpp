/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"

#include <atomic>
#include <utility>

namespace ec {
    class EquivalenceChecker {
    public:
        EquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration) noexcept:
            qc1(qc1), qc2(qc2),
            nqubits(std::max(qc1.getNqubits(), qc2.getNqubits())),
            configuration(std::move(configuration)){};

        virtual ~EquivalenceChecker() = default;

        virtual EquivalenceCriterion run() = 0;

        [[nodiscard]] const Configuration& getConfiguration() const noexcept {
            return configuration;
        }
        [[nodiscard]] EquivalenceCriterion getEquivalence() const noexcept {
            return equivalence;
        }
        [[nodiscard]] double getRuntime() const noexcept {
            return runtime;
        }

        virtual void json(nlohmann::json& j) const noexcept {
            j["equivalence"] = toString(equivalence);
            j["runtime"]     = runtime;
        }

        void signalDone() {
            done.store(true, std::memory_order_relaxed);
        }
        inline auto isDone() { return done.load(std::memory_order_relaxed); }

    protected:
        const qc::QuantumComputation& qc1;
        const qc::QuantumComputation& qc2;

        dd::QubitCount nqubits{};

        Configuration configuration;

        std::atomic<bool> done{false};

        EquivalenceCriterion equivalence = EquivalenceCriterion::NoInformation;
        double               runtime{};
    };

} // namespace ec
