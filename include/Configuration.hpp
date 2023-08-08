//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"
#include "checker/dd/simulation/StateGenerator.hpp"
#include "nlohmann/json.hpp"

#include <functional>
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
    std::string  profile{};
    CostFunction costFunction = &legacyCostFunction;
  };

  struct Functionality {
    double traceThreshold = 1e-8;
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

  [[nodiscard]] bool anythingToExecute() const noexcept {
    return (execution.runSimulationChecker && simulation.maxSims > 0U) ||
           execution.runAlternatingChecker ||
           execution.runConstructionChecker || execution.runZXChecker;
  }

  [[nodiscard]] bool onlySingleTask() const noexcept {
    // only a single simulation shall be performed
    if (execution.runSimulationChecker && (simulation.maxSims == 1U) &&
        !execution.runAlternatingChecker && !execution.runConstructionChecker) {
      return true;
    }

    // no simulations and only one of the other checks shall be performed
    if (!execution.runSimulationChecker &&
        (execution.runAlternatingChecker != execution.runConstructionChecker)) {
      return true;
    }

    return false;
  }

  [[nodiscard]] bool onlyZXCheckerConfigured() const noexcept {
    return !execution.runConstructionChecker &&
           !execution.runSimulationChecker &&
           !execution.runAlternatingChecker && execution.runZXChecker;
  }

  [[nodiscard]] bool onlySimulationCheckerConfigured() const noexcept {
    return !execution.runConstructionChecker &&
           execution.runSimulationChecker && !execution.runAlternatingChecker &&
           !execution.runZXChecker;
  }

  [[nodiscard]] nlohmann::json json() const {
    nlohmann::json config{};
    auto&          exe = config["execution"];
    exe["tolerance"]   = execution.numericalTolerance;
    exe["parallel"]    = execution.parallel;
    if (execution.parallel) {
      exe["nthreads"] = execution.nthreads;
    } else {
      exe["nthreads"] = 1U;
    }
    exe["run_construction_checker"] = execution.runConstructionChecker;
    exe["run_simulation_checker"]   = execution.runSimulationChecker;
    exe["run_alternating_checker"]  = execution.runAlternatingChecker;
    exe["run_zx_checker"]           = execution.runZXChecker;
    if (execution.timeout > 0.) {
      exe["timeout"] = execution.timeout;
    }
    auto& opt = config["optimizations"];
    opt["fix_output_permutation_mismatch"] =
        optimizations.fixOutputPermutationMismatch;
    opt["fuse_consecutive_single_qubit_gates"] =
        optimizations.fuseSingleQubitGates;
    opt["reconstruct_swaps"] = optimizations.reconstructSWAPs;
    opt["remove_diagonal_gates_before_measure"] =
        optimizations.removeDiagonalGatesBeforeMeasure;
    opt["transform_dynamic_circuit"] = optimizations.transformDynamicCircuit;
    opt["reorder_operations"]        = optimizations.reorderOperations;

    auto& app = config["application"];
    if (execution.runConstructionChecker) {
      app["construction"] = ec::toString(application.constructionScheme);
    }
    if (execution.runSimulationChecker) {
      app["simulation"] = ec::toString(application.simulationScheme);
    }
    if (execution.runAlternatingChecker) {
      app["alternating"] = ec::toString(application.alternatingScheme);
    }
    if ((application.constructionScheme == ApplicationSchemeType::GateCost) ||
        (application.simulationScheme == ApplicationSchemeType::GateCost) ||
        (application.alternatingScheme == ApplicationSchemeType::GateCost)) {
      if (!application.profile.empty()) {
        app["profile"] = application.profile;
      } else {
        app["profile"] = "cost_function";
      }
    }

    auto& par                        = config["parameterized"];
    par["tolerance"]                 = parameterized.parameterizedTol;
    par["additional_instantiations"] = parameterized.nAdditionalInstantiations;

    if (execution.runConstructionChecker || execution.runAlternatingChecker) {
      auto& fun              = config["functionality"];
      fun["trace_threshold"] = functionality.traceThreshold;
    }

    if (execution.runSimulationChecker) {
      auto& sim                          = config["simulation"];
      sim["fidelity_threshold"]          = simulation.fidelityThreshold;
      sim["max_sims"]                    = simulation.maxSims;
      sim["state_type"]                  = ec::toString(simulation.stateType);
      sim["seed"]                        = simulation.seed;
      sim["store_counterexample_input"]  = simulation.storeCEXinput;
      sim["store_counterexample_output"] = simulation.storeCEXoutput;
    }

    return config;
  }

  [[nodiscard]] std::string toString() const {
    constexpr auto indent = 2;
    return json().dump(indent);
  }
};
} // namespace ec
