/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_CONFIGURATION_HPP
#define QCEC_CONFIGURATION_HPP

#include "applicationscheme/ApplicationScheme.hpp"
#include "applicationscheme/GateCostApplicationScheme.hpp"
#include "dd/Package.hpp"
#include "nlohmann/json.hpp"
#include "simulation/StateGenerator.hpp"

#include <functional>
#include <thread>

namespace ec {
    struct Configuration {
        // configuration options for execution
        struct Execution {
            dd::fp numericalTolerance = dd::ComplexTable<>::tolerance();

            bool        parallel = true;
            std::size_t nthreads = std::max(1U, std::thread::hardware_concurrency());

            bool runConstructionScheme = false;
            bool runSimulationScheme   = true;
            bool runAlternatingScheme  = true;
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
            ApplicationSchemeType scheme = ApplicationSchemeType::Proportional;

            // options for the gate cost application scheme
            bool         useProfile = false;
            std::string  profileLocation{};
            CostFunction costFunction = [](const GateCostLUTKeyType&) { return 1U; };
        };

        struct Functionality {
            double traceThreshold = 1e-8;
        };

        // configuration options for the simulation scheme
        struct Simulation {
            double      fidelityThreshold = 1e-8;
            std::size_t maxSims           = 16;
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

        [[nodiscard]] nlohmann::json json() const {
            nlohmann::json config{};
            auto&          exe             = config["execution"];
            exe["tolerance"]               = execution.numericalTolerance;
            exe["parallel"]                = execution.parallel;
            exe["nthreads"]                = execution.parallel ? execution.nthreads : 1U;
            exe["run_construction_scheme"] = execution.runConstructionScheme;
            exe["run_simulation_scheme"]   = execution.runSimulationScheme;
            exe["run_alternating_scheme"]  = execution.runAlternatingScheme;

            auto& opt                                   = config["optimizations"];
            opt["fix_output_permutation_mismatch"]      = optimizations.fixOutputPermutationMismatch;
            opt["fuse_consecutive_single_qubit_gates"]  = optimizations.fuseSingleQubitGates;
            opt["reconstruct_swaps"]                    = optimizations.reconstructSWAPs;
            opt["remove_diagonal_gates_before_measure"] = optimizations.removeDiagonalGatesBeforeMeasure;
            opt["transform_dynamic_circuit"]            = optimizations.transformDynamicCircuit;
            opt["reorder_operations"]                   = optimizations.reorderOperations;

            auto& app   = config["application"];
            app["type"] = ec::toString(application.scheme);
            if (application.scheme == ApplicationSchemeType::GateCost) {
                if (application.useProfile) {
                    app["profile"] = application.profileLocation;
                } else {
                    app["profile"] = "cost_function";
                }
            }

            if (execution.runConstructionScheme || execution.runAlternatingScheme) {
                auto& fun              = config["functionality"];
                fun["trace_threshold"] = functionality.traceThreshold;
            }

            if (execution.runSimulationScheme) {
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
            return json().dump(2);
        }
    };
} // namespace ec
#endif //QCEC_CONFIGURATION_HPP
