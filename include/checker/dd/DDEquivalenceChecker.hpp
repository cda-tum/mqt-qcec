//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "TaskManager.hpp"
#include "applicationscheme/ApplicationScheme.hpp"
#include "checker/EquivalenceChecker.hpp"
#include "dd/DDpackageConfig.hpp"
#include "dd/Package.hpp"
#include "ir/QuantumComputation.hpp"

#include <cstddef>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <utility>

namespace ec {
template <class DDType> class DDEquivalenceChecker : public EquivalenceChecker {
public:
  DDEquivalenceChecker(
      const qc::QuantumComputation& circ1, const qc::QuantumComputation& circ2,
      Configuration config,
      const dd::DDPackageConfig& packageConfig = dd::DDPackageConfig{})
      : EquivalenceChecker(circ1, circ2, std::move(config)),
        dd(std::make_unique<dd::Package>(nqubits, packageConfig)),
        taskManager1(TaskManager<DDType>(circ1, *dd)),
        taskManager2(TaskManager<DDType>(circ2, *dd)) {}

  EquivalenceCriterion run() override;

  void json(nlohmann::json& j) const noexcept override;

protected:
  std::unique_ptr<dd::Package> dd;

  TaskManager<DDType> taskManager1;
  TaskManager<DDType> taskManager2;

  std::unique_ptr<ApplicationScheme<DDType>> applicationScheme;

  std::size_t maxActiveNodes{};

  void initializeApplicationScheme(ApplicationSchemeType scheme);

  // at some point this routine should probably make its way into the DD package
  // in some form
  EquivalenceCriterion equals(const DDType& e, const DDType& f);

  virtual void initializeTask(TaskManager<DDType>& taskManager);
  virtual void initialize();
  virtual void execute();
  virtual void finish();
  virtual void postprocessTask(TaskManager<DDType>& task);
  virtual void postprocess();
  virtual EquivalenceCriterion checkEquivalence();
};

} // namespace ec
