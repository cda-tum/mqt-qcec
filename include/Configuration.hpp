//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"
#include "checker/dd/simulation/StateType.hpp"
#include "dd/DDDefinitions.hpp"
#include "dd/RealNumber.hpp"

#include <algorithm>
#include <cstddef>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <thread>

namespace ec {

class Configuration {
public:
  // configuration options for execution
  struct Execution {
    dd::fp numericalTolerance = dd::RealNumber::eps;

    bool        parallel = true;
    std::size_t nthreads = std::max(2U, std::thread::hardware_concurrency());
    double      timeout  = 0.; // in seconds

    bool runConstructionChecker = false;
    bool runSimulationChecker   = true;
    bool runAlternatingChecker  = true;
    bool runZXChecker           = true;
  };

  // configuration options for pre-check optimizations
  struct Optimizations {
    bool fixOutputPermutationMismatch     = false;
    bool fuseSingleQubitGates             = true;
    bool reconstructSWAPs                 = true;
    bool removeDiagonalGatesBeforeMeasure = false;
    bool transformDynamicCircuit          = false;
    bool reorderOperations                = true;
    bool backpropagateOutputPermutation   = false;
    bool elidePermutations                = true;
  };

  // configuration options for application schemes
  struct Application {
    ApplicationSchemeType constructionScheme =
        ApplicationSchemeType::Proportional;
    ApplicationSchemeType simulationScheme =
        ApplicationSchemeType::Proportional;
    ApplicationSchemeType alternatingScheme =
        ApplicationSchemeType::Proportional;

    // options for the gate cost application scheme
    std::string  profile;
    CostFunction costFunction = &legacyCostFunction;
  };

  struct Functionality {
    double traceThreshold          = 1e-8;
    bool   checkPartialEquivalence = false;
  };

  // configuration options for the simulation scheme
  struct Simulation {
    double      fidelityThreshold = 1e-8;
    std::size_t maxSims           = computeMaxSims();
    StateType   stateType         = StateType::ComputationalBasis;
    std::size_t seed              = 0U;
    bool        storeCEXinput     = false;
    bool        storeCEXoutput    = false;

    // this function makes sure that the maximum number of simulations is
    // configured properly.
    static std::size_t computeMaxSims() {
      constexpr std::size_t defaultMaxSims                 = 16U;
      constexpr std::size_t defaultConfiguredOtherCheckers = 2U;
      const auto            systemThreads = std::thread::hardware_concurrency();
      // catch the case where hardware_concurrency() returns 0 or the other
      // pre-configured checkers already use up all the available threads
      if (systemThreads < defaultConfiguredOtherCheckers) {
        return defaultMaxSims;
      }
      return std::max(defaultMaxSims,
                      systemThreads - defaultConfiguredOtherCheckers);
    }
  };

  struct Parameterized {
    double      parameterizedTol          = 1e-12;
    std::size_t nAdditionalInstantiations = 0;
  };

  Execution     execution{};
  Optimizations optimizations{};
  Application   application{};
  Functionality functionality{};
  Simulation    simulation{};
  Parameterized parameterized{};

  [[nodiscard]] bool anythingToExecute() const noexcept;

  [[nodiscard]] bool onlySingleTask() const noexcept;

  [[nodiscard]] bool onlyZXCheckerConfigured() const noexcept;

  [[nodiscard]] bool onlySimulationCheckerConfigured() const noexcept;

  [[nodiscard]] nlohmann::json json() const;

  [[nodiscard]] std::string toString() const;
};
} // namespace ec
