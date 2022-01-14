/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#pragma once

#include "CircuitOptimizer.hpp"
#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "ThreadSafeQueue.hpp"
#include "checker/DDAlternatingChecker.hpp"
#include "checker/DDConstructionChecker.hpp"
#include "checker/DDSimulationChecker.hpp"
#include "simulation/StateGenerator.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace ec {

    class EquivalenceCheckingManager {
    public:
        struct Results {
            double preprocessingTime{};
            double checkTime{};

            EquivalenceCriterion equivalence{};

            std::size_t startedSimulations   = 0U;
            std::size_t performedSimulations = 0U;
            dd::CVec    cexInput{};
            dd::CVec    cexOutput1{};
            dd::CVec    cexOutput2{};

            [[nodiscard]] bool consideredEquivalent() const {
                switch (equivalence) {
                    case EquivalenceCriterion::Equivalent:
                    case EquivalenceCriterion::ProbablyEquivalent:
                    case EquivalenceCriterion::EquivalentUpToGlobalPhase:
                    case EquivalenceCriterion::EquivalentUpToPhase:
                        return true;
                    case EquivalenceCriterion::NotEquivalent:
                    case EquivalenceCriterion::NoInformation:
                    default:
                        return false;
                }
            }

            [[nodiscard]] nlohmann::json json() const;

            static void to_json(nlohmann::json& j, const dd::CVec& stateVector) {
                j = nlohmann::json::array();
                for (const auto& amp: stateVector) {
                    j.emplace_back(std::pair{amp.real(), amp.imag()});
                }
            }
            static void from_json(const nlohmann::json& j, dd::CVec& stateVector) {
                for (std::size_t i = 0; i < j.size(); ++i) {
                    const auto& c  = j.at(i).get<std::pair<dd::fp, dd::fp>>();
                    stateVector[i] = std::complex<dd::fp>(c.first, c.second);
                }
            }
        };

        EquivalenceCheckingManager(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration = Configuration{});

        void run();

        [[nodiscard]] nlohmann::json json() const;
        [[nodiscard]] std::string    toString() const { return json().dump(2); }

        [[nodiscard]] EquivalenceCriterion equivalence() const {
            return results.equivalence;
        }
        [[nodiscard]] Configuration getConfiguration() const { return configuration; }
        [[nodiscard]] Results       getResults() const { return results; }

        // convenience functions for changing the configuration after the manager has been constructed:
        // Execution: These settings may be changed to influence what is executed during `run`
        void setTolerance(dd::fp tol) {
            configuration.execution.numericalTolerance = tol;
            dd::ComplexTable<>::setTolerance(tol);
        }
        void setParallel(bool parallel) { configuration.execution.parallel = parallel; }
        void setNThreads(std::size_t nthreads) { configuration.execution.nthreads = nthreads; }
        void setTimeout(std::chrono::seconds timeout) { configuration.execution.timeout = timeout; }
        void runConstructionScheme(bool run) { configuration.execution.runConstructionScheme = run; }
        void runAlternatingScheme(bool run) { configuration.execution.runAlternatingScheme = run; }
        void runSimulationScheme(bool run) { configuration.execution.runSimulationScheme = run; }

        // Optimization: Optimizations are applied during initialization. Already configured and applied optimizations cannot be reverted
        void runFixOutputPermutationMismatch();
        void fuseSingleQubitGates();
        void reconstructSWAPs();
        void removeDiagonalGatesBeforeMeasure();
        void transformDynamicCircuit();
        void reorderOperations();

        // Application: These settings may be changed to influence the sequence in which gates are applied during the equivalence check
        void setApplicationScheme(const ApplicationSchemeType applicationScheme) { configuration.application.scheme = applicationScheme; }
        void setGateCostProfile(const std::string& profileLocation) {
            configuration.application.scheme          = ApplicationSchemeType::GateCost;
            configuration.application.useProfile      = true;
            configuration.application.profileLocation = profileLocation;
        }
        void setGateCostFunction(const CostFunction& costFunction) {
            configuration.application.scheme       = ApplicationSchemeType::GateCost;
            configuration.application.useProfile   = false;
            configuration.application.costFunction = costFunction;
        }

        // Functionality: These settings may be changed to adjust options for checkers considering the whole functionality
        void setTraceThreshold(double traceThreshold) { configuration.functionality.traceThreshold = traceThreshold; }

        // Simulation: These setting may be changed to adjust the kinds of simulations that are performed
        void setFidelityThreshold(double fidelityThreshold) { configuration.simulation.fidelityThreshold = fidelityThreshold; }
        void setMaxSims(std::size_t sims) {
            if (sims == 0)
                configuration.execution.runSimulationScheme = false;
            configuration.simulation.maxSims = sims;
        }
        void setStateType(StateType stateType) { configuration.simulation.stateType = stateType; }
        void setSeed(std::size_t seed) {
            configuration.simulation.seed = seed;
            stateGenerator.seedGenerator(seed);
        }
        void storeCEXinput(bool store) { configuration.simulation.storeCEXinput = store; }
        void storeCEXoutput(bool store) { configuration.simulation.storeCEXoutput = store; }

    protected:
        qc::QuantumComputation qc1{};
        qc::QuantumComputation qc2{};

        Configuration configuration{};

        StateGenerator stateGenerator;
        std::mutex     stateGeneratorMutex{};

        bool                                             done{false};
        std::vector<std::unique_ptr<EquivalenceChecker>> checkers{};

        Results results;

        /// Given that one circuit has more qubits than the other, the difference is assumed to arise from ancillary qubits.
        /// This function changes the additional qubits in the larger circuit to ancillary qubits.
        /// Furthermore it adds corresponding ancillaries in the smaller circuit
        void setupAncillariesAndGarbage();

        /// In some cases both circuits calculate the same function, but on different qubits.
        /// This function tries to correct such mismatches.
        /// Note that this is still highly experimental!
        void fixOutputPermutationMismatch();

        /// Run all configured optimization passes
        void runOptimizationPasses();

        /// Sequential Equivalence Check (TCAD'21)
        /// First, a couple of simulations with various stimuli are conducted.
        /// If any of those stimuli produce output states with a fidelity not close to 1, the non-equivalence has been shown and the check is finished.
        /// Given that a couple of simulations did not show any signs of non-equivalence, the circuits are probably equivalent.
        /// To assure this, the alternating decision diagram checker is invoked to determine the equivalence.
        void checkSequential();

        /// Parallel Equivalence Check
        /// The parallel flow makes use of the available processing power by orchestrating all configured checks in a parallel fashion
        void checkParallel();

        static void addCircuitDescription(const qc::QuantumComputation& qc, nlohmann::json& j) {
            j["name"]     = qc.getName();
            j["n_qubits"] = qc.getNqubits();
            j["n_gates"]  = qc.getNops();
        }
    };
} // namespace ec
