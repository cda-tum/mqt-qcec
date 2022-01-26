/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#pragma once

#include "CircuitOptimizer.hpp"
#include "Configuration.hpp"
#include "EquivalenceChecker.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "TaskManager.hpp"
#include "applicationscheme/ApplicationScheme.hpp"
#include "applicationscheme/GateCostApplicationScheme.hpp"
#include "applicationscheme/LookaheadApplicationScheme.hpp"
#include "applicationscheme/OneToOneApplicationScheme.hpp"
#include "applicationscheme/ProportionalApplicationScheme.hpp"
#include "applicationscheme/SequentialApplicationScheme.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace ec {
    template<class DDType>
    class DDEquivalenceChecker: public EquivalenceChecker {
    public:
        DDEquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration, bool& done) noexcept;

        EquivalenceCriterion run() override;

        void json(nlohmann::json& j) const noexcept override {
            EquivalenceChecker::json(j);
            j["max_nodes"] = maxActiveNodes;
        }

    protected:
        std::unique_ptr<dd::Package> dd;

        TaskManager<DDType> taskManager1;
        TaskManager<DDType> taskManager2;

        std::unique_ptr<ApplicationScheme<DDType>> applicationScheme;

        std::size_t maxActiveNodes{};

        void initializeApplicationScheme(ApplicationSchemeType scheme);

        EquivalenceCriterion equals(const DDType& e, const DDType& f);

        virtual void                 initializeTask(TaskManager<DDType>&){};
        virtual void                 initialize();
        virtual void                 execute();
        virtual void                 finish();
        virtual void                 postprocessTask(TaskManager<DDType>& task);
        virtual void                 postprocess();
        virtual EquivalenceCriterion checkEquivalence();
    };

} // namespace ec
