/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "ApplicationScheme.hpp"
#include "dd/Definitions.hpp"
#include "operations/OpType.hpp"

#include <functional>
#include <unordered_map>
#include <utility>

namespace std {
    template<>
    struct hash<std::pair<qc::OpType, dd::QubitCount>> {
        size_t operator()(pair<qc::OpType, dd::QubitCount> const& key) const noexcept {
            size_t h1 = hash<decltype(key.first)>{}(key.first);
            size_t h2 = hash<decltype(key.second)>{}(key.second);
            return h1 ^ (h2 << 1);
        }
    };
} // namespace std

namespace ec {
    using GateCostLUTKeyType = std::pair<qc::OpType, dd::QubitCount>;
    using GateCostLUT        = std::unordered_map<GateCostLUTKeyType, std::size_t>;
    using CostFunction       = std::function<std::size_t(const GateCostLUTKeyType&)>;

    template<class DDType>
    class GateCostApplicationScheme: public ApplicationScheme<DDType> {
    public:
        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const CostFunction& costFunction);

        GateCostApplicationScheme(TaskManager<DDType>& taskManager1, TaskManager<DDType>& taskManager2, const std::string& filename);

        std::pair<size_t, size_t> operator()() override;

    protected:
        GateCostLUT gateCostLUT{};

        void populateLUT(const CostFunction& costFunction, const qc::QuantumComputation* qc);

        // read gate cost LUT from file
        // very simple file format:
        // each line consists of
        // <identifier> <controls> <cost>
        void populateLUT(const std::string& filename);
        void populateLUT(std::istream& is);
    };

    std::size_t LegacyIBMCostFunction(const GateCostLUTKeyType& key) noexcept;
} // namespace ec
