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
            dd::fp tolerance      = dd::ComplexTable<>::tolerance();
            double traceThreshold = 1e-8;

            bool        parallel = false;
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
        };

        // configuration options for application schemes
        struct Application {
            ApplicationSchemeType scheme = ApplicationSchemeType::Proportional;

            // options for the gate cost application scheme
            bool                                           useProfile = false;
            std::string                                    profileLocation{};
            std::function<std::size_t(GateCostLUTKeyType)> costFunction = [](GateCostLUTKeyType) { return 1U; };
        };

        // configuration options for the simulation scheme
        struct Simulation {
            double      fidelityLimit  = 0.999;
            std::size_t maxSims        = 16;
            StateType   stateType      = StateType::ComputationalBasis;
            std::size_t seed           = 0U;
            bool        storeCEXinput  = false;
            bool        storeCEXoutput = false;
        };

        Execution     execution{};
        Optimizations optimizations{};
        Application   application{};
        Simulation    simulation{};

        [[nodiscard]] nlohmann::json json() const {
            nlohmann::json config{};
            config["execution"]            = {};
            auto& exe                      = config["execution"];
            exe["tolerance"]               = execution.tolerance;
            exe["trace_threshold"]         = execution.traceThreshold;
            exe["parallel"]                = execution.parallel;
            exe["nthreads"]                = execution.parallel ? execution.nthreads : 1U;
            exe["run_construction_scheme"] = execution.runConstructionScheme;
            exe["run_simulation_scheme"]   = execution.runSimulationScheme;
            exe["run_alternating_scheme"]  = execution.runAlternatingScheme;

            config["optimizations"]                     = {};
            auto& opt                                   = config["optimizations"];
            opt["fix_output_permutation_mismatch"]      = optimizations.fixOutputPermutationMismatch;
            opt["fuse_consecutive_single_qubit_gates"]  = optimizations.fuseSingleQubitGates;
            opt["reconstruct_swaps"]                    = optimizations.reconstructSWAPs;
            opt["remove_diagonal_gates_before_measure"] = optimizations.removeDiagonalGatesBeforeMeasure;
            opt["transform_dynamic_circuit"]            = optimizations.transformDynamicCircuit;

            config["application"] = {};
            auto& app             = config["application"];
            app["type"]           = ec::toString(application.scheme);
            if (application.scheme == ApplicationSchemeType::GateCost) {
                if (application.useProfile) {
                    app["profile"] = application.profileLocation;
                } else {
                    app["profile"] = "cost_function";
                }
            }

            if (execution.runSimulationScheme) {
                config["simulation_scheme"]        = {};
                auto& sim                          = config["simulation_scheme"];
                sim["fidelity_limit"]              = simulation.fidelityLimit;
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
