//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "TaskManager.hpp"
#include "applicationscheme/ApplicationScheme.hpp"
#include "checker/EquivalenceChecker.hpp"

#include <memory>
#include <utility>

namespace ec {
template <class DDType, class DDPackage>
class DDEquivalenceChecker : public EquivalenceChecker {
public:
  DDEquivalenceChecker(const qc::QuantumComputation& qc1,
                       const qc::QuantumComputation& qc2,
                       Configuration                 configuration) noexcept;

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

  // at some point this routine should probably make its way into the DD package
  // in some form
  EquivalenceCriterion equals(const DDType& e, const DDType& f);

  virtual void initializeTask(TaskManager<DDType, DDPackage>& taskManager) = 0;
  virtual void initialize();
  virtual void execute();
  virtual void finish();
  virtual void postprocessTask(TaskManager<DDType, DDPackage>& task);
  virtual void postprocess();
  virtual EquivalenceCriterion checkEquivalence();
};

} // namespace ec
