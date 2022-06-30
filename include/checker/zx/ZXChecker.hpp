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
            j["checker"] = "zx";
        }

    private:
        zx::ZXDiagram miter;
        zx::fp        tolerance;
        bool          ancilla = false;
    };

    qc::Permutation complete(const qc::Permutation& p, dd::Qubit n);
    qc::Permutation concat(const qc::Permutation& p1, const qc::Permutation& p2);
    qc::Permutation invert(const qc::Permutation& p);
    qc::Permutation invertPermutations(const qc::QuantumComputation& qc);
} // namespace ec
