/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "EquivalenceCheckingManager.hpp"
#include "pybind11/chrono.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11_json/pybind11_json.hpp"
#include "qiskit/QasmQobjExperiment.hpp"
#include "qiskit/QuantumCircuit.hpp"

#include <exception>
#include <memory>

namespace py = pybind11;
namespace nl = nlohmann;
using namespace pybind11::literals;

namespace ec {
    qc::QuantumComputation importCircuit(const py::object& circ) {
        py::object QuantumCircuit       = py::module::import("qiskit").attr("QuantumCircuit");
        py::object pyQasmQobjExperiment = py::module::import("qiskit.qobj").attr("QasmQobjExperiment");

        auto qc = qc::QuantumComputation();

        if (py::isinstance<py::str>(circ)) {
            auto&& file = circ.cast<std::string>();
            qc.import(file);
        } else if (py::isinstance(circ, QuantumCircuit)) {
            qc::qiskit::QuantumCircuit::import(qc, circ);
        } else if (py::isinstance(circ, pyQasmQobjExperiment)) {
            qc::qiskit::QasmQobjExperiment::import(qc, circ);
        } else {
            throw std::runtime_error("PyObject is neither py::str, QuantumCircuit, nor QasmQobjExperiment");
        }

        return qc;
    }

    std::unique_ptr<EquivalenceCheckingManager> createManagerFromConfiguration(const py::object& circ1, const py::object& circ2, const Configuration& configuration) noexcept {
        qc::QuantumComputation qc1;
        try {
            qc1 = importCircuit(circ1);
        } catch (const std::exception& ex) {
            std::cerr << "Could not import first circuit: " << ex.what() << std::endl;
            return {};
        }

        qc::QuantumComputation qc2;
        try {
            qc2 = importCircuit(circ2);
        } catch (const std::exception& ex) {
            std::cerr << "Could not import second circuit: " << ex.what() << std::endl;
            return {};
        }

        try {
            return std::make_unique<EquivalenceCheckingManager>(qc1, qc2, configuration);
        } catch (const std::exception& ex) {
            std::cerr << "Failed to construct equivalence checking manager: " << ex.what() << std::endl;
            return {};
        }
    }

    std::unique_ptr<EquivalenceCheckingManager> createManagerFromOptions(const py::object& circ1, const py::object& circ2,
                                                                         // Execution
                                                                         dd::fp               numericalTolerance     = dd::ComplexTable<>::tolerance(),
                                                                         bool                 parallel               = true,
                                                                         std::size_t          nthreads               = std::max(2U, std::thread::hardware_concurrency()),
                                                                         std::chrono::seconds timeout                = 0s,
                                                                         bool                 runConstructionChecker = false,
                                                                         bool                 runSimulationChecker   = true,
                                                                         bool                 runAlternatingChecker  = true,
                                                                         // Optimization
                                                                         bool fixOutputPermutationMismatch     = false,
                                                                         bool fuseSingleQubitGates             = true,
                                                                         bool reconstructSWAPs                 = true,
                                                                         bool removeDiagonalGatesBeforeMeasure = false,
                                                                         bool transformDynamicCircuit          = false,
                                                                         bool reorderOperations                = true,
                                                                         // Application
                                                                         const py::object&  constructionScheme = py::str("proportional"),
                                                                         const py::object&  simulationScheme   = py::str("proportional"),
                                                                         const py::object&  alternatingScheme  = py::str("proportional"),
                                                                         const std::string& profile            = {},
                                                                         // Functionality
                                                                         double traceThreshold = 1e-8,
                                                                         // Simulation
                                                                         double            fidelityThreshold = 1e-8,
                                                                         std::size_t       maxSims           = std::max(16U, std::thread::hardware_concurrency() - 2),
                                                                         const py::object& stateType         = py::str("computational_basis"),
                                                                         std::size_t       seed              = 0U,
                                                                         bool              storeCEXinput     = false,
                                                                         bool              storeCEXoutput    = false) {
        Configuration configuration{};
        // Execution
        configuration.execution.numericalTolerance     = numericalTolerance;
        configuration.execution.parallel               = parallel;
        configuration.execution.nthreads               = nthreads;
        configuration.execution.timeout                = timeout;
        configuration.execution.runConstructionChecker = runConstructionChecker;
        configuration.execution.runSimulationChecker   = runSimulationChecker;
        configuration.execution.runAlternatingChecker  = runAlternatingChecker;
        // Optimization
        configuration.optimizations.fixOutputPermutationMismatch     = fixOutputPermutationMismatch;
        configuration.optimizations.fuseSingleQubitGates             = fuseSingleQubitGates;
        configuration.optimizations.reconstructSWAPs                 = reconstructSWAPs;
        configuration.optimizations.removeDiagonalGatesBeforeMeasure = removeDiagonalGatesBeforeMeasure;
        configuration.optimizations.transformDynamicCircuit          = transformDynamicCircuit;
        configuration.optimizations.reorderOperations                = reorderOperations;
        // Application
        configuration.application.profile = profile;
        try {
            if (py::isinstance<py::str>(constructionScheme)) {
                const auto str                               = constructionScheme.cast<std::string>();
                const auto applicationScheme                 = applicationSchemeFromString(str);
                configuration.application.constructionScheme = applicationScheme;
            } else {
                const auto applicationScheme                 = constructionScheme.cast<ApplicationSchemeType>();
                configuration.application.constructionScheme = applicationScheme;
            }
            if (configuration.application.constructionScheme == ApplicationSchemeType::Lookahead) {
                throw std::invalid_argument("Lookahead application scheme must not be used with construction checker.");
            }
        } catch (std::exception const& e) {
            std::stringstream ss{};
            ss << "Could not set application scheme for construction checker: " << e.what();
            throw std::invalid_argument(ss.str());
        }
        try {
            if (py::isinstance<py::str>(simulationScheme)) {
                const auto str                             = simulationScheme.cast<std::string>();
                const auto applicationScheme               = applicationSchemeFromString(str);
                configuration.application.simulationScheme = applicationScheme;
            } else {
                const auto applicationScheme               = simulationScheme.cast<ApplicationSchemeType>();
                configuration.application.simulationScheme = applicationScheme;
            }
            if (configuration.application.simulationScheme == ApplicationSchemeType::Lookahead) {
                throw std::invalid_argument("Lookahead application scheme must not be used with simulation checker.");
            }
        } catch (std::exception const& e) {
            std::stringstream ss{};
            ss << "Could not set application scheme for simulation checker: " << e.what();
            throw std::invalid_argument(ss.str());
        }
        try {
            if (py::isinstance<py::str>(alternatingScheme)) {
                const auto str                              = alternatingScheme.cast<std::string>();
                const auto applicationScheme                = applicationSchemeFromString(str);
                configuration.application.alternatingScheme = applicationScheme;
            } else {
                const auto applicationScheme                = alternatingScheme.cast<ApplicationSchemeType>();
                configuration.application.alternatingScheme = applicationScheme;
            }
        } catch (std::exception const& e) {
            std::stringstream ss{};
            ss << "Could not set application scheme for alternating checker: " << e.what();
            throw std::invalid_argument(ss.str());
        }
        // Functionality
        configuration.functionality.traceThreshold = traceThreshold;
        // Simulation
        configuration.simulation.fidelityThreshold = fidelityThreshold;
        configuration.simulation.maxSims           = maxSims;

        try {
            if (py::isinstance<py::str>(stateType)) {
                const auto str                     = stateType.cast<std::string>();
                const auto type                    = stateTypeFromString(str);
                configuration.simulation.stateType = type;
            } else {
                const auto type                    = stateType.cast<StateType>();
                configuration.simulation.stateType = type;
            }
        } catch (std::exception const& e) {
            std::stringstream ss{};
            ss << "Could not set state type: " << e.what();
            throw std::invalid_argument(ss.str());
        }

        configuration.simulation.seed           = seed;
        configuration.simulation.storeCEXinput  = storeCEXinput;
        configuration.simulation.storeCEXoutput = storeCEXoutput;

        return createManagerFromConfiguration(circ1, circ2, configuration);
    }

    PYBIND11_MODULE(pyqcec, m) {
        m.doc() = "Python interface for the JKQ QCEC quantum circuit equivalence checking tool";

        py::enum_<ApplicationSchemeType>(m, "ApplicationScheme")
                .value("sequential", ApplicationSchemeType::Sequential,
                       "Applies all gates from the first circuit, before proceeding with the second circuit.")
                .value("one_to_one", ApplicationSchemeType::OneToOne,
                       "Alternates between applications from the first and the second circuit.")
                .value("proportional", ApplicationSchemeType::Proportional,
                       "For every gate of the first circuit, proportionally many are applied from the second circuit according to the difference in the number of gates.")
                .value("lookahead", ApplicationSchemeType::Lookahead,
                       "Looks whether an application from the first circuit or the second circuit yields the smaller decision diagram. Only works for the :attr:`alternating checker <.Configuration.Execution.run_alternating_checker>`.")
                .value("gate_cost", ApplicationSchemeType::GateCost,
                       "Each gate of the first circuit is associated with a corresponding cost according to some cost function *f(...)*. Whenever a gate *g* from the first circuit is applied *f(g)* gates are applied from the second circuit.")
                .def(py::init([](const std::string& str) -> ApplicationSchemeType { return applicationSchemeFromString(str); }));

        py::enum_<StateType>(m, "StateType")
                .value("computational_basis", StateType::ComputationalBasis,
                       "Randomly choose computational basis states.")
                .value("random_1Q_basis", StateType::Random1QBasis,
                       "Randomly choose a single-qubit basis state for each qubit from the six-tuple *(|0>, |1>, |+>, |->, |L>, |R>)*.")
                .value("stabilizer", StateType::Stabilizer,
                       "Randomly choose a stabilizer state by creating a random Clifford circuit.")
                .def(py::init([](const std::string& str) -> StateType { return stateTypeFromString(str); }));

        py::enum_<EquivalenceCriterion>(m, "EquivalenceCriterion")
                .value("no_information", EquivalenceCriterion::NoInformation,
                       "No information on the equivalence is available. This can be due to the fact that the check has not been run or that a timeout happened.")
                .value("not_equivalent", EquivalenceCriterion::NotEquivalent,
                       "Circuits are shown to be non-equivalent.")
                .value("equivalent", EquivalenceCriterion::Equivalent,
                       "Circuits are shown to be equivalent.")
                .value("equivalent_up_to_phase", EquivalenceCriterion::EquivalentUpToPhase,
                       "Circuits are equivalent up to a certain (global or relative) phase.")
                .value("equivalent_up_to_global_phase", EquivalenceCriterion::EquivalentUpToGlobalPhase,
                       "Circuits are equivalent up to a global phase factor.")
                .value("probably_equivalent", EquivalenceCriterion::ProbablyEquivalent,
                       "Circuits are probably equivalent. A result that is obtained whenever a couple of simulations did not show the non-equivalence.")
                .def(py::init([](const std::string& str) -> EquivalenceCriterion { return fromString(str); }));

        // Class definitions
        py::class_<Configuration>                configuration(m, "Configuration", "Configuration options for the QCEC quantum circuit equivalence checking tool");
        py::class_<Configuration::Execution>     execution(configuration, "Execution", "Execution options");
        py::class_<Configuration::Optimizations> optimizations(configuration, "Optimizations", "Optimization options");
        py::class_<Configuration::Application>   application(configuration, "Application", "Application options");
        py::class_<Configuration::Functionality> functionality(configuration, "Functionality", "Functionality options");
        py::class_<Configuration::Simulation>    simulation(configuration, "Simulation", "Simulation options");

        py::class_<EquivalenceCheckingManager>          ecm(m, "EquivalenceCheckingManager", "Main class for orchestrating the equivalence check");
        py::class_<EquivalenceCheckingManager::Results> results(ecm, "Results", "Equivalence checking results");

        // Configuration
        configuration.def(py::init<>())
                .def_readwrite("execution", &Configuration::execution)
                .def_readwrite("optimizations", &Configuration::optimizations)
                .def_readwrite("application", &Configuration::application)
                .def_readwrite("functionality", &Configuration::functionality)
                .def_readwrite("simulation", &Configuration::simulation)
                .def("json", &Configuration::json)
                .def("__repr__", &Configuration::toString);

        execution.def(py::init<>())
                .def_readwrite("numerical_tolerance", &Configuration::Execution::numericalTolerance)
                .def_readwrite("parallel", &Configuration::Execution::parallel)
                .def_readwrite("nthreads", &Configuration::Execution::nthreads)
                .def_readwrite("timeout", &Configuration::Execution::timeout)
                .def_readwrite("run_construction_checker", &Configuration::Execution::runConstructionChecker)
                .def_readwrite("run_simulation_checker", &Configuration::Execution::runSimulationChecker)
                .def_readwrite("run_alternating_checker", &Configuration::Execution::runAlternatingChecker);

        optimizations.def(py::init<>())
                .def_readwrite("fix_output_permutation_mismatch", &Configuration::Optimizations::fixOutputPermutationMismatch)
                .def_readwrite("fuse_single_qubit_gates", &Configuration::Optimizations::fuseSingleQubitGates)
                .def_readwrite("reconstruct_swaps", &Configuration::Optimizations::reconstructSWAPs)
                .def_readwrite("remove_diagonal_gates_before_measure", &Configuration::Optimizations::removeDiagonalGatesBeforeMeasure)
                .def_readwrite("transform_dynamic_circuit", &Configuration::Optimizations::transformDynamicCircuit)
                .def_readwrite("reorder_operations", &Configuration::Optimizations::reorderOperations);

        application.def(py::init<>())
                .def_readwrite("constructionScheme", &Configuration::Application::constructionScheme)
                .def_readwrite("simulationScheme", &Configuration::Application::simulationScheme)
                .def_readwrite("alternatingScheme", &Configuration::Application::alternatingScheme)
                .def_readwrite("profile", &Configuration::Application::profile);

        functionality.def(py::init<>())
                .def_readwrite("trace_treshold", &Configuration::Functionality::traceThreshold);

        simulation.def(py::init<>())
                .def_readwrite("fidelity_treshold", &Configuration::Simulation::fidelityThreshold)
                .def_readwrite("max_sims", &Configuration::Simulation::maxSims)
                .def_readwrite("state_type", &Configuration::Simulation::stateType)
                .def_readwrite("seed", &Configuration::Simulation::seed)
                .def_readwrite("store_cex_input", &Configuration::Simulation::storeCEXinput)
                .def_readwrite("store_cex_output", &Configuration::Simulation::storeCEXoutput);

        // Constructors
        ecm.def(py::init(&createManagerFromOptions), "circ1"_a, "circ2"_a,
                "numerical_tolerance"_a                  = dd::ComplexTable<>::tolerance(),
                "parallel"_a                             = true,
                "nthreads"_a                             = std::max(2U, std::thread::hardware_concurrency()),
                "timeout"_a                              = 0U,
                "run_construction_checker"_a             = false,
                "run_simulation_checker"_a               = true,
                "run_alternating_checker"_a              = true,
                "fix_output_permutation_mismatch"_a      = false,
                "fuse_single_qubit_gates"_a              = true,
                "reconstruct_swaps"_a                    = true,
                "remove_diagonal_gates_before_measure"_a = false,
                "transform_dynamic_circuit"_a            = false,
                "reorder_operations"_a                   = true,
                "construction_scheme"_a                  = "proportional",
                "simulation_scheme"_a                    = "proportional",
                "alternating_scheme"_a                   = "proportional",
                "profile"_a                              = "",
                "trace_treshold"_a                       = 1e-8,
                "fidelity_threshold"_a                   = 1e-8,
                "max_sims"_a                             = std::max(16U, std::thread::hardware_concurrency() - 2),
                "state_type"_a                           = "computational_basis",
                "seed"_a                                 = 0U,
                "store_cex_input"_a                      = false,
                "store_cex_output"_a                     = false)
                .def(py::init([](const py::object& circ1, const py::object& circ2, const Configuration& configuration) {
                         return createManagerFromConfiguration(circ1, circ2, configuration);
                     }),
                     "circ1"_a, "circ2"_a, "config"_a)
                .def("get_configuration", &EquivalenceCheckingManager::getConfiguration)

                // Convenience functions
                // Execution
                .def("set_tolerance", &EquivalenceCheckingManager::setTolerance, "tolerance"_a = dd::ComplexTable<>::tolerance(),
                     "Set the :attr:`numerical tolerance <.Configuration.Execution.numerical_tolerance>` of the underlying decision diagram package.")
                .def("set_parallel", &EquivalenceCheckingManager::setParallel, "enable"_a = true,
                     "Set whether execution should happen in :attr:`~Configuration.Execution.parallel`.")
                .def("set_nthreads", &EquivalenceCheckingManager::setNThreads, "nthreads"_a = std::max(2U, std::thread::hardware_concurrency()),
                     "Set the maximum number of :attr:`threads <.Configuration.Execution.nthreads>` to use.")
                .def("set_timeout", &EquivalenceCheckingManager::setTimeout, "timeout"_a = 0s,
                     "Set a :attr:`timeout <.Configuration.Execution.timeout>` for :func:`~EquivalenceCheckingManager.run`.")
                .def("set_construction_checker", &EquivalenceCheckingManager::setConstructionChecker, "enable"_a = false,
                     "Set whether the :attr:`construction checker <.Configuration.Execution.run_construction_checker>` should be executed.")
                .def("set_simulation_checker", &EquivalenceCheckingManager::setSimulationChecker, "enable"_a = true,
                     "Set whether the :attr:`simulation checker <.Configuration.Execution.run_simulation_checker>` should be executed.")
                .def("set_alternating_checker", &EquivalenceCheckingManager::setAlternatingChecker, "enable"_a = true,
                     "Set whether the :attr:`alternating checker <.Configuration.Execution.run_alternating_checker>` should be executed.")
                // Optimization
                .def("fix_output_permutation_mismatch", &EquivalenceCheckingManager::runFixOutputPermutationMismatch,
                     "Try to :attr:`fix potential mismatches in output permutations <.Configuration.Optimizations.fix_output_permutation_mismatch>`. This is experimental.")
                .def("fuse_single_qubit_gates", &EquivalenceCheckingManager::fuseSingleQubitGates,
                     ":attr:`Fuse consecutive single qubit gates <.Configuration.Optimizations.fuse_single_qubit_gates>`.")
                .def("reconstruct_swaps", &EquivalenceCheckingManager::reconstructSWAPs,
                     ":attr:`Try to reconstruct SWAP gates <.Configuration.Optimizations.reconstruct_swaps>` that have been decomposed or optimized away.")
                .def("remove_diagonal_gates_before_measure", &EquivalenceCheckingManager::removeDiagonalGatesBeforeMeasure,
                     ":attr:`Remove any diagonal gates at the end of the circuit <.Configuration.Optimizations.remove_diagonal_gates_before_measure>`.")
                .def("transform_dynamic_circuit", &EquivalenceCheckingManager::transformDynamicCircuit,
                     ":attr:`Transform dynamic circuit <.Configuration.Optimizations.transform_dynamic_circuit>` in order to verify it.")
                .def("reorder_operations", &EquivalenceCheckingManager::reorderOperations,
                     ":attr:`Reorder operations <.Configuration.Optimizations.reorder_operations>` to establish canonical ordering.")
                // Application
                .def("set_application_scheme", &EquivalenceCheckingManager::setApplicationScheme, "scheme"_a = "proportional",
                     "Set the :class:`Application Scheme <.ApplicationScheme>` that is used for all checkers.")
                .def("set_construction_application_scheme", &EquivalenceCheckingManager::setConstructionApplicationScheme, "scheme"_a = "proportional",
                     "Set the :class:`Application Scheme <.ApplicationScheme>` that is used for the construction checker.")
                .def("set_simulation_application_scheme", &EquivalenceCheckingManager::setSimulationApplicationScheme, "scheme"_a = "proportional",
                     "Set the :class:`Application Scheme <.ApplicationScheme>` that is used for the simulation checker.")
                .def("set_alternating_application_scheme", &EquivalenceCheckingManager::setAlternatingApplicationScheme, "scheme"_a = "proportional",
                     "Set the :class:`Application Scheme <.ApplicationScheme>` that is used for the alternating checker.")
                .def("set_gate_cost_profile", &EquivalenceCheckingManager::setGateCostProfile, "profile"_a = "",
                     "Set the :attr:`profile <.Configuration.Application.profile>` used in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` application scheme for all checkers.")
                .def("set_construction_gate_cost_profile", &EquivalenceCheckingManager::setConstructionGateCostProfile, "profile"_a = "",
                     "Set the :attr:`profile <.Configuration.Application.profile>` used in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` application scheme for the construction checker.")
                .def("set_simulation_gate_cost_profile", &EquivalenceCheckingManager::setSimulationGateCostProfile, "profile"_a = "",
                     "Set the :attr:`profile <.Configuration.Application.profile>` used in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` application scheme for the simulation checker.")
                .def("set_alternating_gate_cost_profile", &EquivalenceCheckingManager::setAlternatingGateCostProfile, "profile"_a = "",
                     "Set the :attr:`profile <.Configuration.Application.profile>` used in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` application scheme for the alternating checker.")
                // Functionality
                .def("set_trace_threshold", &EquivalenceCheckingManager::setTraceThreshold, "threshold"_a = 1e-8,
                     "Set the :attr:`trace threshold <.Configuration.Functionality.trace_threshold>` used for comparing two unitaries or functionality matrices.")
                // Simulation
                .def("set_fidelity_threshold", &EquivalenceCheckingManager::setFidelityThreshold, "threshold"_a = 1e-8,
                     "Set the :attr:`fidelity threshold <.Configuration.Simulation.fidelity_threshold>` used for comparing two states or state vectors.")
                .def("set_max_sims", &EquivalenceCheckingManager::setMaxSims, "sims"_a = std::max(16U, std::thread::hardware_concurrency() - 2),
                     "Set the :attr:`maximum number of simulations <.Configuration.Simulation.max_sims>` to be started for the simulation checker.")
                .def("set_state_type", &EquivalenceCheckingManager::setStateType, "type"_a = "computational_basis",
                     "Set the :attr:`type of states <.Configuration.Simulation.state_type>` used for the simulations in the simulation checker.")
                .def("set_seed", &EquivalenceCheckingManager::setSeed, "seed"_a = 0U,
                     "Set the :attr:`seed <.Configuration.Simulation.seed>` for the state generator in the simulation checker.")
                .def("store_cex_input", &EquivalenceCheckingManager::storeCEXinput, "enable"_a = false,
                     "Set whether to :attr:`store the input state <.Configuration.Simulation.store_cex_input>` if a counterexample is obtained.")
                .def("store_cex_output", &EquivalenceCheckingManager::storeCEXoutput, "enable"_a = false,
                     "Set whether to :attr:`store the output states <.Configuration.Simulation.store_cex_input>` if a counterexample is obtained.")

                // Run
                .def("run", &EquivalenceCheckingManager::run,
                     "Execute the equivalence check as configured.")

                // Results
                .def("equivalence", &EquivalenceCheckingManager::equivalence,
                     "Returns the :class:`.EquivalenceCriterion` that has been determined as the result of the equivalence check.")
                .def("get_results", &EquivalenceCheckingManager::getResults,
                     "Returns the :class:`.EquivalenceCheckingManager.Results` of the equivalence check including statistics.")
                .def("json", &EquivalenceCheckingManager::json,
                     "Returns a JSON-style dictionary of all the information present in the :class:`.EquivalenceCheckingManager`")
                .def("__repr__", &EquivalenceCheckingManager::toString,
                     "Prints a JSON-formatted representation of all the information present in the :class:`.EquivalenceCheckingManager`");

        results.def(py::init<>())
                .def_readwrite("preprocessing_time", &EquivalenceCheckingManager::Results::preprocessingTime,
                               "Time spent during preprocessing (in seconds).")
                .def_readwrite("check_time", &EquivalenceCheckingManager::Results::checkTime,
                               "Time spent during equivalence check (in seconds).")
                .def_readwrite("equivalence", &EquivalenceCheckingManager::Results::equivalence,
                               "Final result of the equivalence check.")
                .def_readwrite("started_simulations", &EquivalenceCheckingManager::Results::startedSimulations,
                               "Number of simulations that have been started.")
                .def_readwrite("performed_simulations", &EquivalenceCheckingManager::Results::performedSimulations,
                               "Number of simulations that have been finished.")
                .def_readwrite("cex_input", &EquivalenceCheckingManager::Results::cexInput,
                               "State vector representation of the initial state that produced a counterexample.")
                .def_readwrite("cex_output1", &EquivalenceCheckingManager::Results::cexOutput1,
                               "State vector representation of the first circuit's counterexample output state.")
                .def_readwrite("cex_output2", &EquivalenceCheckingManager::Results::cexOutput2,
                               "State vector representation of the second circuit's counterexample output state.")
                .def("considered_equivalent", &EquivalenceCheckingManager::Results::consideredEquivalent,
                     "Convenience function to check whether the obtained result is to be considered equivalent.")
                .def("json", &EquivalenceCheckingManager::Results::json,
                     "Returns a JSON-style dictionary of the results.")
                .def("__repr__", &EquivalenceCheckingManager::Results::toString,
                     "Prints a JSON-formatted representation of the results.");

#ifdef VERSION_INFO
        m.attr("__version__") = VERSION_INFO;
#else
        m.attr("__version__") = "dev";
#endif
    }
} // namespace ec
