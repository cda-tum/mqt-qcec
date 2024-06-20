//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/mqt-qcec for more
// information.
//

#include "Configuration.hpp"

#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "checker/dd/simulation/StateType.hpp"

#include <nlohmann/json.hpp>

namespace ec {
bool Configuration::anythingToExecute() const noexcept {
  return (execution.runSimulationChecker && simulation.maxSims > 0U) ||
         execution.runAlternatingChecker || execution.runConstructionChecker ||
         execution.runZXChecker;
}

bool Configuration::onlySingleTask() const noexcept {
  // only a single simulation shall be performed
  if (execution.runSimulationChecker && (simulation.maxSims == 1U) &&
      !execution.runAlternatingChecker && !execution.runConstructionChecker &&
      !execution.runZXChecker) {
    return true;
  }

  // no simulations and only one of the other checks shall be performed
  if (!execution.runSimulationChecker &&
      ((execution.runAlternatingChecker && !execution.runConstructionChecker &&
        !execution.runZXChecker) ||
       (!execution.runAlternatingChecker && execution.runConstructionChecker &&
        !execution.runZXChecker) ||
       (!execution.runAlternatingChecker && !execution.runConstructionChecker &&
        execution.runZXChecker))) {
    return true;
  }

  return false;
}

bool Configuration::onlyZXCheckerConfigured() const noexcept {
  return !execution.runConstructionChecker && !execution.runSimulationChecker &&
         !execution.runAlternatingChecker && execution.runZXChecker;
}

bool Configuration::onlySimulationCheckerConfigured() const noexcept {
  return !execution.runConstructionChecker && execution.runSimulationChecker &&
         !execution.runAlternatingChecker && !execution.runZXChecker;
}

nlohmann::basic_json<> Configuration::json() const {
  nlohmann::basic_json<> config{};
  auto& exe = config["execution"];
  exe["tolerance"] = execution.numericalTolerance;
  exe["parallel"] = execution.parallel;
  if (execution.parallel) {
    exe["nthreads"] = execution.nthreads;
  } else {
    exe["nthreads"] = 1U;
  }
  exe["run_construction_checker"] = execution.runConstructionChecker;
  exe["run_simulation_checker"] = execution.runSimulationChecker;
  exe["run_alternating_checker"] = execution.runAlternatingChecker;
  exe["run_zx_checker"] = execution.runZXChecker;
  if (execution.timeout > 0.) {
    exe["timeout"] = execution.timeout;
  }
  auto& opt = config["optimizations"];
  opt["fuse_consecutive_single_qubit_gates"] =
      optimizations.fuseSingleQubitGates;
  opt["reconstruct_swaps"] = optimizations.reconstructSWAPs;
  opt["remove_diagonal_gates_before_measure"] =
      optimizations.removeDiagonalGatesBeforeMeasure;
  opt["transform_dynamic_circuit"] = optimizations.transformDynamicCircuit;
  opt["reorder_operations"] = optimizations.reorderOperations;
  opt["backpropagate_output_permutation"] =
      optimizations.backpropagateOutputPermutation;
  opt["elide_permutations"] = optimizations.elidePermutations;

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

  auto& par = config["parameterized"];
  par["tolerance"] = parameterized.parameterizedTol;
  par["additional_instantiations"] = parameterized.nAdditionalInstantiations;

  if (execution.runConstructionChecker || execution.runAlternatingChecker) {
    auto& fun = config["functionality"];
    fun["trace_threshold"] = functionality.traceThreshold;
    fun["check_partial_equivalence"] = functionality.checkPartialEquivalence;
  }

  if (execution.runSimulationChecker) {
    auto& sim = config["simulation"];
    sim["fidelity_threshold"] = simulation.fidelityThreshold;
    sim["max_sims"] = simulation.maxSims;
    sim["state_type"] = ec::toString(simulation.stateType);
    sim["seed"] = simulation.seed;
    sim["store_counterexample_input"] = simulation.storeCEXinput;
    sim["store_counterexample_output"] = simulation.storeCEXoutput;
  }

  return config;
}

std::string Configuration::toString() const {
  constexpr auto indent = 2;
  return json().dump(indent);
}
} // namespace ec
