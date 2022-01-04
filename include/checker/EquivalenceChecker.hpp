/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
 */

#ifndef QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
#define QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP

#include "CircuitOptimizer.hpp"
#include "Configuration.hpp"
#include "EquivalenceCheckingResults.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "TaskManager.hpp"
#include "applicationscheme/ApplicationScheme.hpp"
#include "applicationscheme/GateCostApplicationScheme.hpp"
#include "applicationscheme/LookaheadApplicationScheme.hpp"
#include "applicationscheme/OneToOneApplicationScheme.hpp"
#include "applicationscheme/ProportionalApplicationScheme.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace ec {
    template<class DDType>
    class EquivalenceChecker {
    public:
        EquivalenceChecker(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, Configuration configuration);

        virtual ~EquivalenceChecker() = default;

        EquivalenceCriterion equals(const DDType& e, const DDType& f);

        virtual EquivalenceCriterion run();

    protected:
        const qc::QuantumComputation& qc1;
        const qc::QuantumComputation& qc2;

        dd::QubitCount nqubits{};

        std::unique_ptr<dd::Package> dd;

        TaskManager<DDType> taskManager1;
        TaskManager<DDType> taskManager2;

        Configuration configuration;

        std::unique_ptr<ApplicationScheme<DDType>> applicationScheme;

        double      runtime{};
        std::size_t maxActiveNodes{};

        virtual void                 initializeTask(TaskManager<DDType>&){};
        virtual void                 initialize();
        virtual void                 execute();
        virtual void                 finish();
        virtual void                 postprocessTask(TaskManager<DDType>& task);
        virtual void                 postprocess();
        virtual EquivalenceCriterion checkEquivalence();
    };

} // namespace ec

#endif //QUANTUMCIRCUITEQUIVALENCECHECKING_EQUIVALENCECHECKER_HPP
