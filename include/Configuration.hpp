/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#ifndef QCEC_CONFIGURATION_HPP
#define QCEC_CONFIGURATION_HPP

#include "alternating/AlternatingScheme.hpp"
#include "costfunction/CostFunction.hpp"
#include "dd/Package.hpp"
#include "nlohmann/json.hpp"
#include "simulation/StateType.hpp"

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

            bool runAlternatingScheme = true;
            bool runSimulationScheme  = true;

            CostFunction::Type costFunctionType = CostFunction::Type::Proportional;
        };

        // configuration options for pre-check optimizations
        struct Optimizations {
            bool fixOutputPermutationMismatch     = false;
            bool fuseSingleQubitGates             = true;
            bool reconstructSWAPs                 = true;
            bool removeDiagonalGatesBeforeMeasure = false;
            bool transformDynamicCircuit          = false;
        };

        // configuration options for the alternating multiplication scheme
        struct Alternating {
            AlternatingScheme scheme = AlternatingScheme::CostFunction;
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
        Alternating   alternating{};
        Simulation    simulation{};

        [[nodiscard]] nlohmann::json json() const {
            nlohmann::json config{};
            config["execution"]           = {};
            auto& exe                     = config["execution"];
            exe["tolerance"]              = execution.tolerance;
            exe["trace_threshold"]        = execution.traceThreshold;
            exe["parallel"]               = execution.parallel;
            exe["nthreads"]               = execution.parallel ? execution.nthreads : 1U;
            exe["run_alternating_scheme"] = execution.runAlternatingScheme;
            exe["run_simulation_scheme"]  = execution.runSimulationScheme;
            exe["cost_function_type"]     = CostFunction::toString(execution.costFunctionType);

            config["optimizations"]                     = {};
            auto& opt                                   = config["optimizations"];
            opt["fix_output_permutation_mismatch"]      = optimizations.fixOutputPermutationMismatch;
            opt["fuse_consecutive_single_qubit_gates"]  = optimizations.fuseSingleQubitGates;
            opt["reconstruct_swaps"]                    = optimizations.reconstructSWAPs;
            opt["remove_diagonal_gates_before_measure"] = optimizations.removeDiagonalGatesBeforeMeasure;
            opt["transform_dynamic_circuit"]            = optimizations.transformDynamicCircuit;

            config["alternating_scheme_config"] = {};
            auto& alt                           = config["alternating_scheme_config"];
            alt["scheme"]                       = ec::toString(alternating.scheme);

            config["simulation_scheme_config"] = {};
            auto& sim                          = config["simulation config"];
            sim["fidelity_limit"]              = simulation.fidelityLimit;
            sim["max_sims"]                    = simulation.maxSims;
            sim["state_type"]                  = ec::toString(simulation.stateType);
            sim["seed"]                        = simulation.seed;
            sim["store_counterexample_input"]  = simulation.storeCEXinput;
            sim["store_counterexample_output"] = simulation.storeCEXoutput;

            return config;
        }

        [[nodiscard]] std::string toString() const {
            return json().dump(2);
        }
    };
} // namespace ec
#endif //QCEC_CONFIGURATION_HPP
