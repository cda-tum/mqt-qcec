/*
 * This file is part of MQT QCEC library which is released under the MIT license.
 * See file README.md or go to https://www.cda.cit.tum.de/research/quantum/ for more information.
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

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

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
                                                                         const ApplicationSchemeType& constructionScheme = ApplicationSchemeType::Proportional,
                                                                         const ApplicationSchemeType& simulationScheme   = ApplicationSchemeType::Proportional,
                                                                         const ApplicationSchemeType& alternatingScheme  = ApplicationSchemeType::Proportional,
                                                                         const std::string&           profile            = {},
                                                                         // Functionality
                                                                         double traceThreshold = 1e-8,
                                                                         // Simulation
                                                                         double           fidelityThreshold = 1e-8,
                                                                         std::size_t      maxSims           = std::max(16U, std::thread::hardware_concurrency() - 2U),
                                                                         const StateType& stateType         = StateType::ComputationalBasis,
                                                                         std::size_t      seed              = 0U,
                                                                         bool             storeCEXinput     = false,
                                                                         bool             storeCEXoutput    = false) {
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
        configuration.application.profile            = profile;
        configuration.application.constructionScheme = constructionScheme;
        if (configuration.application.constructionScheme == ApplicationSchemeType::Lookahead) {
            throw std::invalid_argument("Lookahead application scheme must not be used with construction checker.");
        }
        configuration.application.simulationScheme = simulationScheme;
        if (configuration.application.simulationScheme == ApplicationSchemeType::Lookahead) {
            throw std::invalid_argument("Lookahead application scheme must not be used with simulation checker.");
        }
        configuration.application.alternatingScheme = alternatingScheme;
        // Functionality
        configuration.functionality.traceThreshold = traceThreshold;
        // Simulation
        configuration.simulation.fidelityThreshold = fidelityThreshold;
        configuration.simulation.maxSims           = maxSims;
        configuration.simulation.stateType         = stateType;
        configuration.simulation.seed              = seed;
        configuration.simulation.storeCEXinput     = storeCEXinput;
        configuration.simulation.storeCEXoutput    = storeCEXoutput;

        return createManagerFromConfiguration(circ1, circ2, configuration);
    }

    ec::EquivalenceCheckingManager::Results verify(const py::object& circ1, const py::object& circ2, const Configuration& configuration) {
        auto ecm = createManagerFromConfiguration(circ1, circ2, configuration);
        ecm->run();
        return ecm->getResults();
    }

    PYBIND11_MODULE(pyqcec, m) {
        m.doc() = "Python interface for the MQT QCEC quantum circuit equivalence checking tool";

        py::enum_<ApplicationSchemeType>(m, "ApplicationScheme")
                .value("sequential", ApplicationSchemeType::Sequential,
                       "Applies all gates from the first circuit, before proceeding with the second circuit. Referred to as *reference* in :cite:p:`burgholzer2021advanced`.")
                .value("one_to_one", ApplicationSchemeType::OneToOne,
                       "Alternates between applications from the first and the second circuit. Referred to as *naive* in :cite:p:`burgholzer2021advanced`.")
                .value("proportional", ApplicationSchemeType::Proportional,
                       "For every gate of the first circuit, proportionally many are applied from the second circuit according to the difference in the number of gates.")
                .value("lookahead", ApplicationSchemeType::Lookahead,
                       "Looks whether an application from the first circuit or the second circuit yields the smaller decision diagram. Only works for the :attr:`alternating checker <.Configuration.Execution.run_alternating_checker>`.")
                .value("gate_cost", ApplicationSchemeType::GateCost,
                       "Each gate of the first circuit is associated with a corresponding cost according to some cost function *f(...)*. Whenever a gate *g* from the first circuit is applied *f(g)* gates are applied from the second circuit. Referred to as *compilation_flow* in :cite:p:`burgholzer2020verifyingResultsIBM`.")
                .def(py::init([](const std::string& str) -> ApplicationSchemeType { return applicationSchemeFromString(str); }))
                .def(
                        "__str__", [](ApplicationSchemeType scheme) { return toString(scheme); }, py::prepend());
        py::implicitly_convertible<std::string, ApplicationSchemeType>();

        py::enum_<StateType>(m, "StateType")
                .value("computational_basis", StateType::ComputationalBasis,
                       "Randomly choose computational basis states. Also referred to as *classical*.")
                .value("random_1Q_basis", StateType::Random1QBasis,
                       "Randomly choose a single-qubit basis state for each qubit from the six-tuple *(|0>, |1>, |+>, |->, |L>, |R>)*. Also referred to as *local_random*.")
                .value("stabilizer", StateType::Stabilizer,
                       "Randomly choose a stabilizer state by creating a random Clifford circuit. Also referred to as *global_random*.")
                .def(py::init([](const std::string& str) -> StateType { return stateTypeFromString(str); }))
                .def(
                        "__str__", [](StateType type) { return toString(type); }, py::prepend());
        py::implicitly_convertible<std::string, StateType>();

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
                .def(py::init([](const std::string& str) -> EquivalenceCriterion { return fromString(str); }))
                .def(
                        "__str__", [](EquivalenceCriterion crit) { return toString(crit); }, py::prepend());
        py::implicitly_convertible<std::string, EquivalenceCriterion>();

        // Class definitions
        py::class_<EquivalenceCheckingManager>          ecm(m, "EquivalenceCheckingManager", "Main class for orchestrating the equivalence check");
        py::class_<EquivalenceCheckingManager::Results> results(ecm, "Results", "Equivalence checking results");

        py::class_<Configuration> configuration(m, "Configuration", "Configuration options for the QCEC quantum circuit equivalence checking tool");

        // Constructors
        ecm.def(py::init(&createManagerFromOptions), "circ1"_a, "circ2"_a,
                "numerical_tolerance"_a                  = dd::ComplexTable<>::tolerance(),
                "parallel"_a                             = true,
                "nthreads"_a                             = std::max(2U, std::thread::hardware_concurrency()),
                "timeout"_a                              = 0s,
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
                "trace_threshold"_a                      = 1e-8,
                "fidelity_threshold"_a                   = 1e-8,
                "max_sims"_a                             = std::max(16U, std::thread::hardware_concurrency() - 2U),
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
                .def("set_timeout", &EquivalenceCheckingManager::setTimeout, "timeout"_a = 0.0,
                     "Set a :attr:`timeout <.Configuration.Execution.timeout>` (in seconds) for :func:`~EquivalenceCheckingManager.run`. The timeout can also be specified by a :class:`float`.")
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
                .def("set_max_sims", &EquivalenceCheckingManager::setMaxSims, "sims"_a = std::max(16U, std::thread::hardware_concurrency() - 2U),
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

        py::class_<Configuration::Execution>     execution(configuration, "Execution", "Options that orchestrate the :meth:`~.EquivalenceCheckingManager.run` method.");
        py::class_<Configuration::Optimizations> optimizations(configuration, "Optimizations", "Options that influence which circuit optimizations are applied during pre-processing.");
        py::class_<Configuration::Application>   application(configuration, "Application", "Options that describe the :class:`Application Scheme <.ApplicationScheme>` that is used for the individual equivalence checkers.");
        py::class_<Configuration::Functionality> functionality(configuration, "Functionality", "Options for all checkers that consider the whole functionality of a circuit.");
        py::class_<Configuration::Simulation>    simulation(configuration, "Simulation", "Options that influence the simulation-based equivalence checker.");

        // Configuration
        configuration.def(py::init<>())
                .def_readwrite("execution", &Configuration::execution)
                .def_readwrite("optimizations", &Configuration::optimizations)
                .def_readwrite("application", &Configuration::application)
                .def_readwrite("functionality", &Configuration::functionality)
                .def_readwrite("simulation", &Configuration::simulation)
                .def("json", &Configuration::json, "Returns a JSON-style dictionary of the configuration.")
                .def("__repr__", &Configuration::toString, "Prints a JSON-formatted representation of the configuration.");

        execution.def(py::init<>())
                .def_readwrite("parallel", &Configuration::Execution::parallel, "Set whether execution should happen in parallel. Defaults to :code:`True`.")
                .def_readwrite("nthreads", &Configuration::Execution::nthreads, "Set the maximum number of threads to use. Defaults to the maximum number of available threads reported by the OS.")
                .def_readwrite("timeout", &Configuration::Execution::timeout, "Set a timeout for :meth:`~.EquivalenceCheckingManager.run` (in seconds). Either a :class:`datetime.timedelta` or :class:`float`. Defaults to :code:`0.`, which means no timeout.")
                .def_readwrite("run_construction_checker", &Configuration::Execution::runConstructionChecker, "Set whether the construction checker should be executed. Defaults to :code:`False` since the alternating checker is to be preferred in most cases.")
                .def_readwrite("run_simulation_checker", &Configuration::Execution::runSimulationChecker, "Set whether the simulation checker should be executed. Defaults to :code:`True` since simulations can quickly show the non-equivalence of circuits in many cases.")
                .def_readwrite("run_alternating_checker", &Configuration::Execution::runAlternatingChecker, "Set whether the alternating checker should be executed. Defaults to :code:`True` since staying close to the identity can quickly show the equivalence of circuits in many cases.")
                .def_readwrite("numerical_tolerance", &Configuration::Execution::numericalTolerance, "Set the numerical tolerance of the underlying decision diagram package. Defaults to :code:`~2e-13` and should only be changed by users who know what they are doing.");

        optimizations.def(py::init<>())
                .def_readwrite("fix_output_permutation_mismatch", &Configuration::Optimizations::fixOutputPermutationMismatch, "Try to fix potential mismatches in output permutations. This is experimental and, hence, defaults to :code:`False`.")
                .def_readwrite("fuse_single_qubit_gates", &Configuration::Optimizations::fuseSingleQubitGates, "Fuse consecutive single-qubit gates by grouping them together. Defaults to :code:`True` as this typically increases the performance of the subsequent equivalence check.")
                .def_readwrite("reconstruct_swaps", &Configuration::Optimizations::reconstructSWAPs, "Try to reconstruct SWAP gates that have been decomposed (into a sequence of 3 CNOT gates) or optimized away (as a consequence of a SWAP preceded or followed by a CNOT on the same qubits). Defaults to :code:`True` since this reconstruction enables the efficient tracking of logical to physical qubit permutations throughout circuits that have been mapped to a target architecture.")
                .def_readwrite("remove_diagonal_gates_before_measure", &Configuration::Optimizations::removeDiagonalGatesBeforeMeasure, "Remove any diagonal gates at the end of the circuit. This might be desirable since any diagonal gate in front of a measurement does not influence the probabilities of the respective states. Defaults to :code:`False` since, in general, circuits differing by diagonal gates at the end should still be considered non-equivalent.")
                .def_readwrite("transform_dynamic_circuit", &Configuration::Optimizations::transformDynamicCircuit, "Circuits containing dynamic circuit primitives such as mid-circuit measurements, resets, or classically-controlled operations cannot be verified in a straight-forward fashion due to the non-unitary nature of these primitives, which is why this setting defaults to :code:`False`. By enabling this optimization, any dynamic circuit is first transformed to a circuit without non-unitary primitives by, first, substituting qubit resets with new qubits and, then, applying the deferred measurement principle to defer measurements to the end.")
                .def_readwrite("reorder_operations", &Configuration::Optimizations::reorderOperations, "The operations of a circuit are stored in a sequential container. This introduces some dependencies in the order of operations that are not naturally present in the quantum circuit. As a consequence, two quantum circuits that contain exactly the same operations, list their operations in different ways, also apply there operations in a different order. This optimization pass established a canonical ordering of operations by, first, constructing a directed, acyclic graph for the operations and, then, traversing it in a breadth-first fashion. Defaults to :code:`True`.");

        application.def(py::init<>())
                .def_readwrite("construction_scheme", &Configuration::Application::constructionScheme, "The :class:`Application Scheme <.ApplicationScheme>` used for the construction checker.")
                .def_readwrite("simulation_scheme", &Configuration::Application::simulationScheme, "The :class:`Application Scheme <.ApplicationScheme>` used for the simulation checker.")
                .def_readwrite("alternating_scheme", &Configuration::Application::alternatingScheme, "The :class:`Application Scheme <.ApplicationScheme>` used for the alternating checker.")
                .def_readwrite("profile", &Configuration::Application::profile, "The :attr:`Gate Cost <.ApplicationScheme.gate_cost>` application scheme can be configured with a profile that specifies the cost of gates. At the moment, this profile can be set via a file that is constructed similar to a lookup table. Every line :code:`<GATE_ID> <N_CONTROLS> <COST>` specified the cost for a given gate type and with a certain number of controls, e.g., :code:`X 0 1` denotes that a single-qubit X gate has a cost of :code:`1`, while :code:`X 2 15` denotes that a Toffoli gate has a cost of :code:`15`.");

        functionality.def(py::init<>())
                .def_readwrite("trace_threshold", &Configuration::Functionality::traceThreshold, "While decision diagrams are canonical in theory, i.e., equivalent circuits produce equivalent decision diagrams, numerical inaccuracies and approximations can harm this property. This can result in a scenario where two decision diagrams are really close to one another, but cannot be identified as such by standard methods (i.e., comparing their root pointers). Instead, for two decision diagrams :code:`U` and :code:`U'` representing the functionalities of two circuits :code:`G` and :code:`G'`, the trace of the product of one decision diagram with the inverse of the other can be computed and compared to the trace of the identity. Alternatively, it can be checked, whether :code:`U*U`^-1` is \"close enough\" to the identity by recursively checking that each decision diagram node is close enough to the identity structure (i.e., the first and last successor have weights close to one, while the second and third successor have weights close to zero). Whenever any decision diagram node differs from this structure by more than the configured threshold, the circuits are concluded to be non-equivalent. Defaults to :code:`1e-8`.");

        simulation.def(py::init<>())
                .def_readwrite("fidelity_threshold", &Configuration::Simulation::fidelityThreshold, "Similar to :attr:`trace threshold <.Configuration.Functionality.trace_threshold>`, this setting is here to tackle numerical inaccuracies and approximations for the simulation checker. Instead of computing a trace, the fidelity between the states resulting from the simulation is computed. Whenever the fidelity differs from :code:`1.` by more than the configured threshold, the circuits are concluded to be non-equivalent. Defaults to :code:`1e-8`.")
                .def_readwrite("max_sims", &Configuration::Simulation::maxSims, "The maximum number of simulations to be started for the simulation checker. In practice, just a couple of simulations suffice in most cases to detect a potential non-equivalence. Either defaults to :code:`16` or the maximum number of available threads minus 2, whichever is more.")
                .def_readwrite("state_type", &Configuration::Simulation::stateType, "The :class:`type of states <.StateType>` used for the simulations in the simulation checker.")
                .def_readwrite("seed", &Configuration::Simulation::seed, "The seed used in the quantum state generator. Defaults to :code:`0`, which means that the seed is chosen non-deterministically for each program run.")
                .def_readwrite("store_cex_input", &Configuration::Simulation::storeCEXinput, "Whether to store the input state that has lead to the determination of a counterexample. Since the memory required to store a full representation of a quantum state increases exponentially, this is only recommended for a small number of qubits and defaults to :code:`False`.")
                .def_readwrite("store_cex_output", &Configuration::Simulation::storeCEXoutput, "Whether to store the resulting states that prove the non-equivalence of both circuits. Since the memory required to store a full representation of a quantum state increases exponentially, this is only recommended for a small number of qubits and defaults to :code:`False`.");

        m.def("verify", &verify,
              "circ1"_a, "circ2"_a,
              "config"_a = ec::Configuration{},
              "Convenience function for verifying the equivalence of two circuits. Wraps creating an instance of :class:`EquivalenceCheckingManager <.EquivalenceCheckingManager>`, calling :meth:`EquivalenceCheckingManager.run` and calling :meth:`EquivalenceCheckingManager.get_result`.");

#ifdef VERSION_INFO
        m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
        m.attr("__version__") = "dev";
#endif
    }
} // namespace ec
