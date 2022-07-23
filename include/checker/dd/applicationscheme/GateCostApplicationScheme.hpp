//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "ApplicationScheme.hpp"
#include "dd/Definitions.hpp"
#include "operations/OpType.hpp"

#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace std {
template <> struct hash<std::pair<qc::OpType, dd::QubitCount>> {
  std::size_t
  operator()(pair<qc::OpType, dd::QubitCount> const& key) const noexcept {
    const std::size_t h1 = hash<decltype(key.first)>{}(key.first);
    const std::size_t h2 = hash<decltype(key.second)>{}(key.second);
    return h1 ^ (h2 << 1);
  }
};
} // namespace std

namespace ec {
using GateCostLookupTableKeyType = std::pair<qc::OpType, dd::QubitCount>;
using GateCostLookupTable =
    std::unordered_map<GateCostLookupTableKeyType, std::size_t>;
using CostFunction =
    std::function<std::size_t(const GateCostLookupTableKeyType&)>;

template <class DDType, class DDPackage = dd::Package<>>
class GateCostApplicationScheme final
    : public ApplicationScheme<DDType, DDPackage> {
public:
  GateCostApplicationScheme(TaskManager<DDType, DDPackage>& taskManager1,
                            TaskManager<DDType, DDPackage>& taskManager2,
                            const CostFunction&             costFunction)
      : ApplicationScheme<DDType, DDPackage>(taskManager1, taskManager2) {
    populateLookupTable(costFunction, taskManager1.getCircuit());
    populateLookupTable(costFunction, taskManager2.getCircuit());
  }

  GateCostApplicationScheme(TaskManager<DDType, DDPackage>& taskManager1,
                            TaskManager<DDType, DDPackage>& taskManager2,
                            const std::string&              filename)
      : ApplicationScheme<DDType, DDPackage>(taskManager1, taskManager2) {
    populateLookupTable(filename);
  }

  std::pair<size_t, size_t> operator()() override {
    if (gateCostLookupTable.empty()) {
      return {1U, 1U};
    }

    const auto& op = this->taskManager1();
    const auto  key =
        GateCostLookupTableKeyType{op->getType(), op->getNcontrols()};
    std::size_t cost = 1U;
    if (auto it = gateCostLookupTable.find(key);
        it != gateCostLookupTable.end()) {
      cost = it->second;
    }
    return {1U, cost};
  }

private:
  GateCostLookupTable gateCostLookupTable{};

  template <class CostFun>
  void populateLookupTable(CostFun                       costFunction,
                           const qc::QuantumComputation* qc) {
    for (const auto& op : *qc) {
      const auto type      = op->getType();
      const auto nControls = op->getNcontrols();
      const auto key       = GateCostLookupTableKeyType{type, nControls};
      if (const auto it = gateCostLookupTable.find(key);
          it == gateCostLookupTable.end()) {
        const auto cost = costFunction(key);
        gateCostLookupTable.emplace(key, cost);
      }
    }
  }

  // read gate cost LUT from file
  // simple file format:
  // each line consists of
  // <identifier> <controls> <cost>
  void populateLookupTable(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.good()) {
      throw std::invalid_argument("Error opening LUT file: " + filename);
    }
    populateLookupTable(ifs);
  }
  void populateLookupTable(std::istream& is) {
    qc::OpType  opType    = qc::OpType::None;
    std::size_t nControls = 0U;
    std::size_t cost      = 1U;
    while (is.good() && (is >> opType >> nControls >> cost)) {
      gateCostLookupTable.emplace(std::pair{opType, nControls}, cost);
    }
  }
};

[[nodiscard]] std::size_t
legacyCostFunction(const GateCostLookupTableKeyType& key) noexcept;
} // namespace ec
