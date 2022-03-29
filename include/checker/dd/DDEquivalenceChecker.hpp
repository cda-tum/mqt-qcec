/*
 * This file is part of MQT QCEC library which is released under the MIT license.
 * See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
 */

#pragma once

#include "CircuitOptimizer.hpp"
#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "TaskManager.hpp"
#include "applicationscheme/ApplicationScheme.hpp"
#include "applicationscheme/GateCostApplicationScheme.hpp"
#include "applicationscheme/LookaheadApplicationScheme.hpp"
#include "applicationscheme/OneToOneApplicationScheme.hpp"
#include "applicationscheme/ProportionalApplicationScheme.hpp"
#include "applicationscheme/SequentialApplicationScheme.hpp"
#include "checker/EquivalenceChecker.hpp"
#include "checker/dd/DDPackageConfigs.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace ec {
    template<class DDType, class DDPackage>
    class DDEquivalenceChecker: public EquivalenceChecker {
    public:
        DDEquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration) noexcept;

        EquivalenceCriterion run() override;

        void json(nlohmann::json& j) const noexcept override {
            EquivalenceChecker::json(j);
            j["max_nodes"] = maxActiveNodes;
        }

    protected:
        std::unique_ptr<DDPackage> dd;

        TaskManager<DDType, DDPackage> taskManager1;
        TaskManager<DDType, DDPackage> taskManager2;

        std::unique_ptr<ApplicationScheme<DDType, DDPackage>> applicationScheme;

        std::size_t maxActiveNodes{};

        void initializeApplicationScheme(ApplicationSchemeType scheme);

        // at some point this routine should probably make its way into the DD package in some form
        EquivalenceCriterion equals(const DDType& e, const DDType& f);

        virtual void                 initializeTask(TaskManager<DDType, DDPackage>&) = 0;
        virtual void                 initialize();
        virtual void                 execute();
        virtual void                 finish();
        virtual void                 postprocessTask(TaskManager<DDType, DDPackage>& task);
        virtual void                 postprocess();
        virtual EquivalenceCriterion checkEquivalence();
    };

} // namespace ec
