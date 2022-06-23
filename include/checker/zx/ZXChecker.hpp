#pragma once

#include "Configuration.hpp"
#include "Definitions.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "ZXDiagram.hpp"
#include "checker/EquivalenceChecker.hpp"
#include "nlohmann/json.hpp"

namespace ec {
    class ZXEquivalenceChecker: public EquivalenceChecker {
    public:
        ZXEquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration) noexcept;

        EquivalenceCriterion run() override;

        void json(nlohmann::json& j) const noexcept override {
            EquivalenceChecker::json(j);
        }

    private:
        zx::ZXDiagram miter;
        zx::fp        tolerance = zx::TOLERANCE;
    };
} // namespace ec
