/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"
#include "checker/dd/simulation/StateGenerator.hpp"
#include "dd/Package.hpp"
#include "nlohmann/json.hpp"

#include <chrono>
#include <functional>
#include <thread>

using namespace std::chrono_literals;

namespace ec {
    struct Configuration {
        // configuration options for execution
        struct Execution {
            dd::fp numericalTolerance = dd::ComplexTable<>::tolerance();

            bool                 parallel = true;
            std::size_t          nthreads = std::max(2U, std::thread::hardware_concurrency());
            std::chrono::seconds timeout  = 0s;

            bool runConstructionChecker = false;
            bool runSimulationChecker   = true;
            bool runAlternatingChecker  = true;
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
            ApplicationSchemeType constructionScheme = ApplicationSchemeType::Proportional;
            ApplicationSchemeType simulationScheme   = ApplicationSchemeType::Proportional;
            ApplicationSchemeType alternatingScheme  = ApplicationSchemeType::Proportional;

            // options for the gate cost application scheme
            std::string  profile{};
            CostFunction costFunction = LegacyIBMCostFunction;
        };

        struct Functionality {
            double traceThreshold = 1e-8;
        };

        // configuration options for the simulation scheme
        struct Simulation {
            double      fidelityThreshold = 1e-8;
            std::size_t maxSims           = std::max(16U, std::thread::hardware_concurrency() - 2U);
            StateType   stateType         = StateType::ComputationalBasis;
            std::size_t seed              = 0U;
            bool        storeCEXinput     = false;
            bool        storeCEXoutput    = false;
        };

        Execution     execution{};
        Optimizations optimizations{};
        Application   application{};
        Functionality functionality{};
        Simulation    simulation{};

        [[nodiscard]] bool anythingToExecute() const noexcept {
            return (execution.runSimulationChecker && simulation.maxSims > 0U) || execution.runAlternatingChecker || execution.runConstructionChecker;
        }

        [[nodiscard]] bool onlySingleTask() const noexcept {
            // only a single simulation shall be performed
            if (execution.runSimulationChecker && simulation.maxSims == 1U && !execution.runAlternatingChecker && !execution.runConstructionChecker) {
                return true;
            }

            // no simulations and only one of the other checks shall be performed
            if (!execution.runSimulationChecker && (execution.runAlternatingChecker != execution.runConstructionChecker)) {
                return true;
            }

            return false;
        }

        [[nodiscard]] nlohmann::json json() const noexcept {
            nlohmann::json config{};
            auto&          exe              = config["execution"];
            exe["tolerance"]                = execution.numericalTolerance;
            exe["parallel"]                 = execution.parallel;
            exe["nthreads"]                 = execution.parallel ? execution.nthreads : 1U;
            exe["run_construction_checker"] = execution.runConstructionChecker;
            exe["run_simulation_checker"]   = execution.runSimulationChecker;
            exe["run_alternating_checker"]  = execution.runAlternatingChecker;
            if (execution.timeout > 0s) {
                exe["timeout"] = execution.timeout.count();
            }
            auto& opt                                   = config["optimizations"];
            opt["fix_output_permutation_mismatch"]      = optimizations.fixOutputPermutationMismatch;
            opt["fuse_consecutive_single_qubit_gates"]  = optimizations.fuseSingleQubitGates;
            opt["reconstruct_swaps"]                    = optimizations.reconstructSWAPs;
            opt["remove_diagonal_gates_before_measure"] = optimizations.removeDiagonalGatesBeforeMeasure;
            opt["transform_dynamic_circuit"]            = optimizations.transformDynamicCircuit;
            opt["reorder_operations"]                   = optimizations.reorderOperations;

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
            if (application.constructionScheme == ApplicationSchemeType::GateCost ||
                application.simulationScheme == ApplicationSchemeType::GateCost ||
                application.alternatingScheme == ApplicationSchemeType::GateCost) {
                if (!application.profile.empty()) {
                    app["profile"] = application.profile;
                } else {
                    app["profile"] = "cost_function";
                }
            }

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

        [[nodiscard]] std::string toString() const noexcept {
            return json().dump(2);
        }
    };
} // namespace ec
