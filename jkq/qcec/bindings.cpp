/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "EquivalenceCheckingManager.hpp"
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
                                                                         dd::fp      numericalTolerance    = dd::ComplexTable<>::tolerance(),
                                                                         bool        parallel              = true,
                                                                         std::size_t nthreads              = std::max(2U, std::thread::hardware_concurrency()),
                                                                         std::size_t timeout               = 0,
                                                                         bool        runConstructionScheme = false,
                                                                         bool        runSimulationScheme   = true,
                                                                         bool        runAlternatingScheme  = true,
                                                                         // Optimization
                                                                         bool fixOutputPermutationMismatch     = false,
                                                                         bool fuseSingleQubitGates             = true,
                                                                         bool reconstructSWAPs                 = true,
                                                                         bool removeDiagonalGatesBeforeMeasure = false,
                                                                         bool transformDynamicCircuit          = false,
                                                                         bool reorderOperations                = true,
                                                                         // Application
                                                                         ApplicationSchemeType scheme  = ApplicationSchemeType::Proportional,
                                                                         const std::string&    profile = {},
                                                                         // Functionality
                                                                         double traceThreshold = 1e-8,
                                                                         // Simulation
                                                                         double      fidelityThreshold = 1e-8,
                                                                         std::size_t maxSims           = std::max(16U, std::thread::hardware_concurrency() - 2),
                                                                         StateType   stateType         = StateType::ComputationalBasis,
                                                                         std::size_t seed              = 0U,
                                                                         bool        storeCEXinput     = false,
                                                                         bool        storeCEXoutput    = false) {
        Configuration configuration{};
        // Execution
        configuration.execution.numericalTolerance    = numericalTolerance;
        configuration.execution.parallel              = parallel;
        configuration.execution.nthreads              = nthreads;
        configuration.execution.timeout               = std::chrono::seconds{timeout};
        configuration.execution.runConstructionScheme = runConstructionScheme;
        configuration.execution.runSimulationScheme   = runSimulationScheme;
        configuration.execution.runAlternatingScheme  = runAlternatingScheme;
        // Optimization
        configuration.optimizations.fixOutputPermutationMismatch     = fixOutputPermutationMismatch;
        configuration.optimizations.fuseSingleQubitGates             = fuseSingleQubitGates;
        configuration.optimizations.reconstructSWAPs                 = reconstructSWAPs;
        configuration.optimizations.removeDiagonalGatesBeforeMeasure = removeDiagonalGatesBeforeMeasure;
        configuration.optimizations.transformDynamicCircuit          = transformDynamicCircuit;
        configuration.optimizations.reorderOperations                = reorderOperations;
        // Application
        if (!profile.empty()) {
            configuration.application.scheme  = ApplicationSchemeType::GateCost;
            configuration.application.profile = profile;
        } else {
            configuration.application.scheme = scheme;
        }
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

    PYBIND11_MODULE(pyqcec, m) {
        m.doc() = "Python interface for the JKQ QCEC quantum circuit equivalence checking tool";

        py::enum_<ApplicationSchemeType>(m, "ApplicationScheme")
                .value("sequential", ApplicationSchemeType::Sequential)
                .value("one_to_one", ApplicationSchemeType::OneToOne)
                .value("proportional", ApplicationSchemeType::Proportional)
                .value("lookahead", ApplicationSchemeType::Lookahead)
                .value("gate_cost", ApplicationSchemeType::GateCost)
                .export_values()
                .def(py::init([](const std::string& str) -> ApplicationSchemeType { return applicationSchemeFromString(str); }));

        py::enum_<StateType>(m, "StateType")
                .value("computational_basis", StateType::ComputationalBasis)
                .value("random_1Q_basis", StateType::Random1QBasis)
                .value("stabilizer", StateType::Stabilizer)
                .export_values()
                .def(py::init([](const std::string& str) -> StateType { return stateTypeFromString(str); }));

        py::enum_<EquivalenceCriterion>(m, "EquivalenceCriterion")
                .value("no_information", EquivalenceCriterion::NoInformation)
                .value("not_equivalent", EquivalenceCriterion::NotEquivalent)
                .value("equivalent", EquivalenceCriterion::Equivalent)
                .value("equivalent_up_to_phase", EquivalenceCriterion::EquivalentUpToPhase)
                .value("equivalent_up_to_global_phase", EquivalenceCriterion::EquivalentUpToGlobalPhase)
                .value("probably_equivalent", EquivalenceCriterion::ProbablyEquivalent)
                .export_values()
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
                .def_readwrite("run_construction_scheme", &Configuration::Execution::runConstructionScheme)
                .def_readwrite("run_simulation_scheme", &Configuration::Execution::runSimulationScheme)
                .def_readwrite("run_alternating_scheme", &Configuration::Execution::runAlternatingScheme);

        optimizations.def(py::init<>())
                .def_readwrite("fix_output_permutation_mismatch", &Configuration::Optimizations::fixOutputPermutationMismatch)
                .def_readwrite("fuse_single_qubit_gates", &Configuration::Optimizations::fuseSingleQubitGates)
                .def_readwrite("reconstruct_swaps", &Configuration::Optimizations::reconstructSWAPs)
                .def_readwrite("remove_diagonal_gates_before_measure", &Configuration::Optimizations::removeDiagonalGatesBeforeMeasure)
                .def_readwrite("transform_dynamic_circuit", &Configuration::Optimizations::transformDynamicCircuit)
                .def_readwrite("reorder_operations", &Configuration::Optimizations::reorderOperations);

        application.def(py::init<>())
                .def_readwrite("scheme", &Configuration::Application::scheme)
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
                "numerical_tolerance"_a,
                "parallel"_a, "nthreads"_a,
                "timeout"_a,
                "run_construction_scheme"_a, "run_simulation_scheme"_a, "run_alternating_scheme"_a,
                "fix_output_permutation_mismatch"_a,
                "fuse_single_qubit_gates"_a,
                "reconstruct_swaps"_a,
                "remove_diagonal_gates_before_measure"_a,
                "transform_dynamic_circuit"_a,
                "reorder_operations"_a,
                "scheme"_a, "profile"_a,
                "trace_treshold"_a,
                "fidelity_threshold"_a,
                "max_sims"_a, "state_type"_a, "seed"_a,
                "store_cex_input"_a, "store_cex_output"_a)
                .def(py::init([](const py::object& circ1, const py::object& circ2, const Configuration& configuration) {
                         return createManagerFromConfiguration(circ1, circ2, configuration);
                     }),
                     "circ1"_a, "circ2"_a, "config"_a)
                .def("get_configuration", &EquivalenceCheckingManager::getConfiguration)
                // Convenience functions
                .def("set_tolerance", &EquivalenceCheckingManager::setTolerance)
                .def("set_parallel", &EquivalenceCheckingManager::setParallel)
                .def("set_nthreads", &EquivalenceCheckingManager::setNThreads)
                .def("set_timeout", &EquivalenceCheckingManager::setTimeout)
                .def("set_construction_scheme", &EquivalenceCheckingManager::setConstructionScheme)
                .def("set_simulation_scheme", &EquivalenceCheckingManager::setSimulationScheme)
                .def("set_alternating_scheme", &EquivalenceCheckingManager::setAlternatingScheme)
                .def("fix_output_permutation_mismatch", &EquivalenceCheckingManager::runFixOutputPermutationMismatch)
                .def("fuse_single_qubit_gates", &EquivalenceCheckingManager::fuseSingleQubitGates)
                .def("reconstruct_swaps", &EquivalenceCheckingManager::reconstructSWAPs)
                .def("remove_diagonal_gates_before_measure", &EquivalenceCheckingManager::removeDiagonalGatesBeforeMeasure)
                .def("transform_dynamic_circuit", &EquivalenceCheckingManager::transformDynamicCircuit)
                .def("reorder_operations", &EquivalenceCheckingManager::reorderOperations)
                .def("set_application_scheme", &EquivalenceCheckingManager::setApplicationScheme)
                .def("set_gate_cost_profile", &EquivalenceCheckingManager::setGateCostProfile)
                .def("set_trace_threshold", &EquivalenceCheckingManager::setTraceThreshold)
                .def("set_fidelity_threshold", &EquivalenceCheckingManager::setFidelityThreshold)
                .def("set_max_sims", &EquivalenceCheckingManager::setMaxSims)
                .def("set_state_type", &EquivalenceCheckingManager::setStateType)
                .def("set_seed", &EquivalenceCheckingManager::setSeed)
                .def("store_cex_input", &EquivalenceCheckingManager::storeCEXinput)
                .def("store_cex_output", &EquivalenceCheckingManager::storeCEXoutput)
                // Execution
                .def("run", &EquivalenceCheckingManager::run)
                // Results
                .def("equivalence", &EquivalenceCheckingManager::equivalence)
                .def("get_results", &EquivalenceCheckingManager::getResults)
                .def("json", &EquivalenceCheckingManager::json)
                .def("__repr__", &EquivalenceCheckingManager::toString);

        results.def(py::init<>())
                .def_readwrite("preprocessing_time", &EquivalenceCheckingManager::Results::preprocessingTime)
                .def_readwrite("check_time", &EquivalenceCheckingManager::Results::checkTime)
                .def_readwrite("equivalence", &EquivalenceCheckingManager::Results::equivalence)
                .def_readwrite("started_simulations", &EquivalenceCheckingManager::Results::startedSimulations)
                .def_readwrite("performed_simulations", &EquivalenceCheckingManager::Results::performedSimulations)
                .def_readwrite("cex_input", &EquivalenceCheckingManager::Results::cexInput)
                .def_readwrite("cex_output1", &EquivalenceCheckingManager::Results::cexOutput1)
                .def_readwrite("cex_output2", &EquivalenceCheckingManager::Results::cexOutput2)
                .def("considered_equivalent", &EquivalenceCheckingManager::Results::consideredEquivalent)
                .def("json", &EquivalenceCheckingManager::Results::json)
                .def("__repr__", &EquivalenceCheckingManager::Results::toString);

#ifdef VERSION_INFO
        m.attr("__version__") = VERSION_INFO;
#else
        m.attr("__version__") = "dev";
#endif
    }
} // namespace ec
