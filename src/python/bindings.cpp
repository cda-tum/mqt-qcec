//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11_json/pybind11_json.hpp"
#include "python/qiskit/QuantumCircuit.hpp"

#include <exception>
#include <memory>

namespace py = pybind11;
using namespace pybind11::literals;

namespace ec {
namespace {
qc::QuantumComputation importCircuit(const py::object& circ) {
  const py::object quantumCircuit =
      py::module::import("qiskit").attr("QuantumCircuit");
  const py::object pyQasmQobjExperiment =
      py::module::import("qiskit.qobj").attr("QasmQobjExperiment");

  auto qc = qc::QuantumComputation();

  if (py::isinstance<py::str>(circ)) {
    const auto file = circ.cast<std::string>();
    qc.import(file);
  } else if (py::isinstance(circ, quantumCircuit)) {
    qc::qiskit::QuantumCircuit::import(qc, circ);
  } else {
    throw std::runtime_error(
        "PyObject is neither py::str, QuantumCircuit, nor QasmQobjExperiment");
  }

  return qc;
}

std::unique_ptr<EquivalenceCheckingManager>
createManagerFromConfiguration(const py::object& circ1, const py::object& circ2,
                               const Configuration& configuration = {}) {
  qc::QuantumComputation qc1;
  try {
    qc1 = importCircuit(circ1);
  } catch (const std::exception& ex) {
    throw std::runtime_error("Could not import first circuit: " +
                             std::string(ex.what()));
  }

  qc::QuantumComputation qc2;
  try {
    qc2 = importCircuit(circ2);
  } catch (const std::exception& ex) {
    throw std::runtime_error("Could not import second circuit: " +
                             std::string(ex.what()));
  }

  return std::make_unique<EquivalenceCheckingManager>(qc1, qc2, configuration);
}
} // namespace

PYBIND11_MODULE(pyqcec, m) {
  m.doc() = "Python interface for the MQT QCEC quantum circuit equivalence "
            "checking tool";

  // Application scheme enum
  py::enum_<ApplicationSchemeType>(m, "ApplicationScheme")
      .value("sequential", ApplicationSchemeType::Sequential,
             "Applies all gates from the first circuit, before proceeding with "
             "the second circuit. Referred to as *reference* in "
             ":cite:p:`burgholzer2021advanced`.")
      .value("one_to_one", ApplicationSchemeType::OneToOne,
             "Alternates between applications from the first and the second "
             "circuit. Referred to as *naive* in "
             ":cite:p:`burgholzer2021advanced`.")
      .value("proportional", ApplicationSchemeType::Proportional,
             "For every gate of the first circuit, proportionally many are "
             "applied from the second circuit according to the difference in "
             "the number of gates.")
      .value(
          "lookahead", ApplicationSchemeType::Lookahead,
          "Looks whether an application from the first circuit or the second "
          "circuit yields the smaller decision diagram. Only works for the "
          ":ref:`alternating checker <EquivalenceChecking:Alternating "
          "Equivalence Checker (using Decision Diagrams)>`.")
      .value(
          "gate_cost", ApplicationSchemeType::GateCost,
          "Each gate of the first circuit is associated with a corresponding "
          "cost according to some cost function *f(...)*. Whenever a gate *g* "
          "from the first circuit is applied *f(g)* gates are applied from the "
          "second circuit. Referred to as *compilation_flow* in "
          ":cite:p:`burgholzer2020verifyingResultsIBM`.")
      // allow construction from a string
      .def(py::init([](const std::string& str) -> ApplicationSchemeType {
        return applicationSchemeFromString(str);
      }))
      // provide a string representation of the enum
      .def(
          "__str__",
          [](const ApplicationSchemeType scheme) { return toString(scheme); },
          py::prepend());
  // allow implicit conversion from string to ApplicationSchemeType
  py::implicitly_convertible<std::string, ApplicationSchemeType>();

  // State type enum
  py::enum_<StateType>(m, "StateType")
      .value("computational_basis", StateType::ComputationalBasis,
             "Randomly choose computational basis states. Also referred to as "
             "*classical*.")
      .value("random_1Q_basis", StateType::Random1QBasis,
             "Randomly choose a single-qubit basis state for each qubit from "
             "the six-tuple *(|0>, |1>, |+>, |->, |L>, |R>)*. Also referred to "
             "as *local_random*.")
      .value("stabilizer", StateType::Stabilizer,
             "Randomly choose a stabilizer state by creating a random Clifford "
             "circuit. Also referred to as *global_random*.")
      // allow construction from a string
      .def(py::init([](const std::string& str) -> StateType {
        return stateTypeFromString(str);
      }))
      // provide a string representation of the enum
      .def(
          "__str__", [](const StateType type) { return toString(type); },
          py::prepend());
  // allow implicit conversion from string to StateType
  py::implicitly_convertible<std::string, StateType>();

  // Equivalence criterion enum
  py::enum_<EquivalenceCriterion>(m, "EquivalenceCriterion")
      .value("no_information", EquivalenceCriterion::NoInformation,
             "No information on the equivalence is available. This can be due "
             "to the fact that the check has not been run or that a timeout "
             "happened.")
      .value("not_equivalent", EquivalenceCriterion::NotEquivalent,
             "Circuits are shown to be non-equivalent.")
      .value("equivalent", EquivalenceCriterion::Equivalent,
             "Circuits are shown to be equivalent.")
      .value(
          "equivalent_up_to_phase", EquivalenceCriterion::EquivalentUpToPhase,
          "Circuits are equivalent up to a certain (global or relative) phase.")
      .value("equivalent_up_to_global_phase",
             EquivalenceCriterion::EquivalentUpToGlobalPhase,
             "Circuits are equivalent up to a global phase factor.")
      .value(
          "probably_equivalent", EquivalenceCriterion::ProbablyEquivalent,
          "Circuits are probably equivalent. A result that is obtained "
          "whenever a couple of simulations did not show the non-equivalence "
          "in the :ref:`simulation checker <EquivalenceChecking:Simulation "
          "Equivalence Checker (using Decision Diagrams)>`.")
      .value("probably_not_equivalent",
             EquivalenceCriterion::ProbablyNotEquivalent,
             "Circuits are probably not equivalent. A result that is obtained "
             "whenever the :ref:`ZX-calculus checker "
             "<EquivalenceChecking:ZX-Calculus Equivalence Checker>` could not "
             "reduce the combined circuit to the identity.")
      // allow construction from a string
      .def(py::init([](const std::string& str) -> EquivalenceCriterion {
        return fromString(str);
      }))
      // provide a string representation of the enum
      .def(
          "__str__",
          [](const EquivalenceCriterion crit) { return toString(crit); },
          py::prepend());
  // allow implicit conversion from string to EquivalenceCriterion
  py::implicitly_convertible<std::string, EquivalenceCriterion>();

  // Class definitions
  py::class_<EquivalenceCheckingManager> ecm(
      m, "EquivalenceCheckingManager",
      "Main class for orchestrating the equivalence check");
  py::class_<EquivalenceCheckingManager::Results> results(
      ecm, "Results", "Equivalence checking results");
  py::class_<Configuration> configuration(
      m, "Configuration",
      "Configuration options for the QCEC quantum circuit equivalence checking "
      "tool");

  // Constructors
  ecm.def(py::init(&createManagerFromConfiguration), "circ1"_a, "circ2"_a,
          "config"_a = Configuration(),
          "Create an equivalence checking manager for two circuits and "
          "configure it with a :class:`Configuration` object.")
      .def("get_configuration", &EquivalenceCheckingManager::getConfiguration)
      // Convenience functions
      // Execution
      .def("set_tolerance", &EquivalenceCheckingManager::setTolerance,
           "tolerance"_a = dd::RealNumber::eps,
           "Set the :attr:`numerical tolerance "
           "<.Configuration.Execution.numerical_tolerance>` of the underlying "
           "decision diagram package.")
      .def("set_parallel", &EquivalenceCheckingManager::setParallel,
           "enable"_a = true,
           "Set whether execution should happen in "
           ":attr:`~Configuration.Execution.parallel`.")
      .def("set_nthreads", &EquivalenceCheckingManager::setNThreads,
           "nthreads"_a = std::max(2U, std::thread::hardware_concurrency()),
           "Set the maximum number of :attr:`threads "
           "<.Configuration.Execution.nthreads>` to use.")
      .def("set_timeout", &EquivalenceCheckingManager::setTimeout,
           "timeout"_a = 0.,
           "Set a :attr:`timeout <.Configuration.Execution.timeout>` (in "
           "seconds) for :func:`~EquivalenceCheckingManager.run`.")
      .def("set_construction_checker",
           &EquivalenceCheckingManager::setConstructionChecker,
           "enable"_a = false,
           "Set whether the :ref:`construction checker "
           "<EquivalenceChecking:Construction Equivalence Checker (using "
           "Decision Diagrams)>` should be executed.")
      .def("set_simulation_checker",
           &EquivalenceCheckingManager::setSimulationChecker, "enable"_a = true,
           "Set whether the :ref:`simulation checker "
           "<EquivalenceChecking:Simulation Equivalence Checker (using "
           "Decision Diagrams)>` should be executed.")
      .def("set_alternating_checker",
           &EquivalenceCheckingManager::setAlternatingChecker,
           "enable"_a = true,
           "Set whether the :ref:`alternating checker "
           "<EquivalenceChecking:Alternating Equivalence Checker (using "
           "Decision Diagrams)>` should be executed.")
      .def("set_zx_checker", &EquivalenceCheckingManager::setZXChecker,
           "enable"_a = true,
           "Set whether the :ref:`ZX-calculus checker "
           "<EquivalenceChecking:ZX-Calculus Equivalence Checker>` should be "
           "executed.")
      .def("disable_all_checkers",
           &EquivalenceCheckingManager::disableAllCheckers,
           "Disable all equivalence checkers.")
      // Optimization
      .def("fix_output_permutation_mismatch",
           &EquivalenceCheckingManager::runFixOutputPermutationMismatch,
           "Try to :attr:`fix potential mismatches in output permutations "
           "<.Configuration.Optimizations.fix_output_permutation_mismatch>`. "
           "This is experimental.")
      .def("fuse_single_qubit_gates",
           &EquivalenceCheckingManager::fuseSingleQubitGates,
           ":attr:`Fuse consecutive single qubit gates "
           "<.Configuration.Optimizations.fuse_single_qubit_gates>`.")
      .def("reconstruct_swaps", &EquivalenceCheckingManager::reconstructSWAPs,
           ":attr:`Try to reconstruct SWAP gates "
           "<.Configuration.Optimizations.reconstruct_swaps>` that have been "
           "decomposed or optimized away.")
      .def("reorder_operations", &EquivalenceCheckingManager::reorderOperations,
           ":attr:`Reorder operations "
           "<.Configuration.Optimizations.reorder_operations>` to establish "
           "canonical ordering.")
      .def("backpropagate_output_permutation",
           &EquivalenceCheckingManager::backpropagateOutputPermutation,
           ":attr:`Backpropagate the output permutation "
           "<.Configuration.Optimizations.backpropagate_output_permutation>` "
           "to the input permutation.")
      .def("elide_permutations", &EquivalenceCheckingManager::elidePermutations,
           ":attr:`Elide permutations "
           "<.Configuration.Optimizations.elide_permutations>`"
           " from the circuit.")
      // Application
      .def("set_application_scheme",
           &EquivalenceCheckingManager::setApplicationScheme,
           "scheme"_a = "proportional",
           "Set the :class:`Application Scheme <.ApplicationScheme>` that is "
           "used for all checkers (based on decision diagrams).")
      .def("set_construction_application_scheme",
           &EquivalenceCheckingManager::setConstructionApplicationScheme,
           "scheme"_a = "proportional",
           "Set the :class:`Application Scheme <.ApplicationScheme>` that is "
           "used for the :ref:`construction checker "
           "<EquivalenceChecking:Construction Equivalence Checker (using "
           "Decision Diagrams)>`.")
      .def("set_simulation_application_scheme",
           &EquivalenceCheckingManager::setSimulationApplicationScheme,
           "scheme"_a = "proportional",
           "Set the :class:`Application Scheme <.ApplicationScheme>` that is "
           "used for the :ref:`simulation checker "
           "<EquivalenceChecking:Simulation Equivalence Checker (using "
           "Decision Diagrams)>`.")
      .def("set_alternating_application_scheme",
           &EquivalenceCheckingManager::setAlternatingApplicationScheme,
           "scheme"_a = "proportional",
           "Set the :class:`Application Scheme <.ApplicationScheme>` that is "
           "used for the :ref:`alternating checker "
           "<EquivalenceChecking:Alternating Equivalence Checker (using "
           "Decision Diagrams)>`.")
      .def("set_gate_cost_profile",
           &EquivalenceCheckingManager::setGateCostProfile, "profile"_a = "",
           "Set the :attr:`profile <.Configuration.Application.profile>` used "
           "in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` "
           "application scheme for all checkers (based on decision diagrams).")
      .def("set_construction_gate_cost_profile",
           &EquivalenceCheckingManager::setConstructionGateCostProfile,
           "profile"_a = "",
           "Set the :attr:`profile <.Configuration.Application.profile>` used "
           "in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` "
           "application scheme for the :ref:`construction checker "
           "<EquivalenceChecking:Construction Equivalence Checker (using "
           "Decision Diagrams)>`.")
      .def("set_simulation_gate_cost_profile",
           &EquivalenceCheckingManager::setSimulationGateCostProfile,
           "profile"_a = "",
           "Set the :attr:`profile <.Configuration.Application.profile>` used "
           "in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` "
           "application scheme for the :ref:`simulation checker "
           "<EquivalenceChecking:Simulation Equivalence Checker (using "
           "Decision Diagrams)>`.")
      .def("set_alternating_gate_cost_profile",
           &EquivalenceCheckingManager::setAlternatingGateCostProfile,
           "profile"_a = "",
           "Set the :attr:`profile <.Configuration.Application.profile>` used "
           "in the :attr:`Gate Cost <.ApplicationScheme.gate_cost>` "
           "application scheme for the :ref:`alternating checker "
           "<EquivalenceChecking:Alternating Equivalence Checker (using "
           "Decision Diagrams)>`.")
      // Functionality
      .def("set_trace_threshold",
           &EquivalenceCheckingManager::setTraceThreshold, "threshold"_a = 1e-8,
           "Set the :attr:`trace threshold "
           "<.Configuration.Functionality.trace_threshold>` used for comparing "
           "two unitaries or functionality matrices.")
      .def(
          "set_check_partial_equivalence",
          &EquivalenceCheckingManager::setCheckPartialEquivalence,
          "enable"_a = false,
          "Set whether to check for partial equivalence. Two circuits are "
          "partially equivalent if, for each possible initial input state, "
          "they have the same probability for each measurement outcome. "
          "If set to false, the checker will output 'not equivalent' for "
          "circuits that are partially equivalent but not totally equivalent. ")
      // Simulation
      .def("set_fidelity_threshold",
           &EquivalenceCheckingManager::setFidelityThreshold,
           "threshold"_a = 1e-8,
           "Set the :attr:`fidelity threshold "
           "<.Configuration.Simulation.fidelity_threshold>` used for comparing "
           "two states or state vectors.")
      .def("set_max_sims", &EquivalenceCheckingManager::setMaxSims,
           "sims"_a = std::max(16U, std::thread::hardware_concurrency() - 2U),
           "Set the :attr:`maximum number of simulations "
           "<.Configuration.Simulation.max_sims>` to be started for the "
           ":ref:`simulation checker <EquivalenceChecking:Simulation "
           "Equivalence Checker (using Decision Diagrams)>`.")
      .def("set_state_type", &EquivalenceCheckingManager::setStateType,
           "state_type"_a = "computational_basis",
           "Set the :attr:`type of states "
           "<.Configuration.Simulation.state_type>` used for the simulations "
           "in the :ref:`simulation checker <EquivalenceChecking:Simulation "
           "Equivalence Checker (using Decision Diagrams)>`.")
      .def("set_seed", &EquivalenceCheckingManager::setSeed, "seed"_a = 0U,
           "Set the :attr:`seed <.Configuration.Simulation.seed>` for the "
           "state generator in the :ref:`simulation checker "
           "<EquivalenceChecking:Simulation Equivalence Checker (using "
           "Decision Diagrams)>`.")
      .def("store_cex_input", &EquivalenceCheckingManager::storeCEXinput,
           "enable"_a = false,
           "Set whether to :attr:`store the input state "
           "<.Configuration.Simulation.store_cex_input>` if a counterexample "
           "is obtained.")
      .def("store_cex_output", &EquivalenceCheckingManager::storeCEXoutput,
           "enable"_a = false,
           "Set whether to :attr:`store the output states "
           "<.Configuration.Simulation.store_cex_input>` if a counterexample "
           "is obtained.")

      // Run
      .def("run", &EquivalenceCheckingManager::run,
           "Execute the equivalence check as configured.")

      // Results
      .def("equivalence", &EquivalenceCheckingManager::equivalence,
           "Returns the :class:`.EquivalenceCriterion` that has been "
           "determined as the result of the equivalence check.")
      .def("get_results", &EquivalenceCheckingManager::getResults,
           "Returns the :class:`.EquivalenceCheckingManager.Results` of the "
           "equivalence check including statistics.")
      .def("__repr__", [](const EquivalenceCheckingManager& ecm) {
        return "<EquivalenceCheckingManager: " + toString(ecm.equivalence()) +
               ">";
      });

  // EquivalenceCheckingManager::Results bindings
  results.def(py::init<>())
      .def_readwrite("name1", &EquivalenceCheckingManager::Results::name1,
                     "Name of the first circuit.")
      .def_readwrite("name2", &EquivalenceCheckingManager::Results::name2,
                     "Name of the second circuit.")
      .def_readwrite("num_qubits1",
                     &EquivalenceCheckingManager::Results::numQubits1,
                     "Number of qubits of the first circuit.")
      .def_readwrite("num_qubits2",
                     &EquivalenceCheckingManager::Results::numQubits2,
                     "Number of qubits of the second circuit.")
      .def_readwrite("num_gates1",
                     &EquivalenceCheckingManager::Results::numGates1,
                     "Number of gates of the first circuit.")
      .def_readwrite("num_gates2",
                     &EquivalenceCheckingManager::Results::numGates2,
                     "Number of gates of the second circuit.")
      .def_readwrite("configuration",
                     &EquivalenceCheckingManager::Results::configuration,
                     ":class:`.Configuration` used for the equivalence check.")
      .def_readwrite("preprocessing_time",
                     &EquivalenceCheckingManager::Results::preprocessingTime,
                     "Time spent during preprocessing (in seconds).")
      .def_readwrite("check_time",
                     &EquivalenceCheckingManager::Results::checkTime,
                     "Time spent during equivalence check (in seconds).")
      .def_readwrite("equivalence",
                     &EquivalenceCheckingManager::Results::equivalence,
                     "Final result of the equivalence check.")
      .def_readwrite("started_simulations",
                     &EquivalenceCheckingManager::Results::startedSimulations,
                     "Number of simulations that have been started.")
      .def_readwrite("performed_simulations",
                     &EquivalenceCheckingManager::Results::performedSimulations,
                     "Number of simulations that have been finished.")
      .def_readwrite("cex_input",
                     &EquivalenceCheckingManager::Results::cexInput,
                     "State vector representation of the initial state that "
                     "produced a counterexample.")
      .def_readwrite("cex_output1",
                     &EquivalenceCheckingManager::Results::cexOutput1,
                     "State vector representation of the first circuit's "
                     "counterexample output state.")
      .def_readwrite("cex_output2",
                     &EquivalenceCheckingManager::Results::cexOutput2,
                     "State vector representation of the second circuit's "
                     "counterexample output state.")
      .def_readwrite(
          "performed_instantiations",
          &EquivalenceCheckingManager::Results::performedInstantiations,
          "Number of circuit instantiations that have been performed during "
          "equivalence checking of parameterized quantum circuits.")
      .def_readwrite("checker_results",
                     &EquivalenceCheckingManager::Results::checkerResults,
                     "Dictionary of the results of the individual checkers.")
      .def("considered_equivalent",
           &EquivalenceCheckingManager::Results::consideredEquivalent,
           "Convenience function to check whether the obtained result is to be "
           "considered equivalent.")
      .def("json", &EquivalenceCheckingManager::Results::json,
           "Returns a JSON-style dictionary of the results.")
      .def("__str__", &EquivalenceCheckingManager::Results::toString,
           "Prints a JSON-formatted representation of the results.")
      .def("__repr__", [](const EquivalenceCheckingManager::Results& res) {
        return "<EquivalenceCheckingManager.Results: " +
               toString(res.equivalence) + ">";
      });

  // Configuration sub-classes
  py::class_<Configuration::Execution> execution(
      configuration, "Execution",
      "Options that orchestrate the :meth:`~.EquivalenceCheckingManager.run` "
      "method.");
  py::class_<Configuration::Optimizations> optimizations(
      configuration, "Optimizations",
      "Options that influence which circuit optimizations are applied during "
      "pre-processing.");
  py::class_<Configuration::Application> application(
      configuration, "Application",
      "Options that describe the :class:`Application Scheme "
      "<.ApplicationScheme>` that is used for the individual equivalence "
      "checkers.");
  py::class_<Configuration::Functionality> functionality(
      configuration, "Functionality",
      "Options for all checkers that consider the whole functionality of a "
      "circuit.");
  py::class_<Configuration::Simulation> simulation(
      configuration, "Simulation",
      "Options that influence the :ref:`simulation checker "
      "<EquivalenceChecking:Simulation Equivalence Checker (using Decision "
      "Diagrams)>`.");
  py::class_<Configuration::Parameterized> parameterized(
      configuration, "Parameterized",
      "Options that influence the equivalence checking scheme for "
      "parameterized circuits.");

  // Configuration
  configuration.def(py::init<>())
      .def_readwrite("execution", &Configuration::execution)
      .def_readwrite("optimizations", &Configuration::optimizations)
      .def_readwrite("application", &Configuration::application)
      .def_readwrite("functionality", &Configuration::functionality)
      .def_readwrite("simulation", &Configuration::simulation)
      .def_readwrite("parameterized", &Configuration::parameterized)
      .def("json", &Configuration::json,
           "Returns a JSON-style dictionary of the configuration.")
      .def("__repr__", &Configuration::toString,
           "Prints a JSON-formatted representation of the configuration.");

  // execution options
  execution.def(py::init<>())
      .def_readwrite("parallel", &Configuration::Execution::parallel,
                     "Set whether execution should happen in parallel. "
                     "Defaults to :code:`True`.")
      .def_readwrite(
          "nthreads", &Configuration::Execution::nthreads,
          "Set the maximum number of threads to use. Defaults to the maximum "
          "number of available threads reported by the OS.")
      .def_readwrite(
          "timeout", &Configuration::Execution::timeout,
          "Set a timeout for :meth:`~.EquivalenceCheckingManager.run` (in "
          "seconds). Defaults to :code:`0.`, which means no timeout.")
      .def_readwrite(
          "run_construction_checker",
          &Configuration::Execution::runConstructionChecker,
          "Set whether the :ref:`construction checker "
          "<EquivalenceChecking:Construction Equivalence Checker (using "
          "Decision Diagrams)>` should be executed. Defaults to :code:`False` "
          "since the :ref:`alternating checker "
          "<EquivalenceChecking:Alternating Equivalence Checker (using "
          "Decision Diagrams)>` is to be preferred in most cases.")
      .def_readwrite("run_simulation_checker",
                     &Configuration::Execution::runSimulationChecker,
                     "Set whether the :ref:`simulation checker "
                     "<EquivalenceChecking:Simulation Equivalence Checker "
                     "(using Decision Diagrams)>` should be executed. Defaults "
                     "to :code:`True` since simulations can quickly show the "
                     "non-equivalence of circuits in many cases.")
      .def_readwrite("run_alternating_checker",
                     &Configuration::Execution::runAlternatingChecker,
                     "Set whether the :ref:`alternating checker "
                     "<EquivalenceChecking:Alternating Equivalence Checker "
                     "(using Decision Diagrams)>` should be executed. Defaults "
                     "to :code:`True` since staying close to the identity can "
                     "quickly show the equivalence of circuits in many cases.")
      .def_readwrite(
          "run_zx_checker", &Configuration::Execution::runZXChecker,
          "Set whether the :ref:`ZX-calculus checker "
          "<EquivalenceChecking:ZX-Calculus Equivalence Checker>` should be "
          "executed. Defaults to :code:`True` but arbitrary multi-controlled "
          "operations are only partially supported.")
      .def_readwrite("numerical_tolerance",
                     &Configuration::Execution::numericalTolerance,
                     "Set the numerical tolerance of the underlying decision "
                     "diagram package. Defaults to :code:`~2e-13` and should "
                     "only be changed by users who know what they are doing.");

  // optimization options
  optimizations.def(py::init<>())
      .def_readwrite(
          "fix_output_permutation_mismatch",
          &Configuration::Optimizations::fixOutputPermutationMismatch,
          "Try to fix potential mismatches in output permutations. This is "
          "experimental and, hence, defaults to :code:`False`.")
      .def_readwrite(
          "fuse_single_qubit_gates",
          &Configuration::Optimizations::fuseSingleQubitGates,
          "Fuse consecutive single-qubit gates by grouping them together. "
          "Defaults to :code:`True` as this typically increases the "
          "performance of the subsequent equivalence check.")
      .def_readwrite(
          "reconstruct_swaps", &Configuration::Optimizations::reconstructSWAPs,
          "Try to reconstruct SWAP gates that have been decomposed (into a "
          "sequence of 3 CNOT gates) or optimized away (as a consequence of a "
          "SWAP preceded or followed by a CNOT on the same qubits). Defaults "
          "to :code:`True` since this reconstruction enables the efficient "
          "tracking of logical to physical qubit permutations throughout "
          "circuits that have been mapped to a target architecture.")
      .def_readwrite(
          "remove_diagonal_gates_before_measure",
          &Configuration::Optimizations::removeDiagonalGatesBeforeMeasure,
          "Remove any diagonal gates at the end of the circuit. This might be "
          "desirable since any diagonal gate in front of a measurement does "
          "not influence the probabilities of the respective states. Defaults "
          "to :code:`False` since, in general, circuits differing by diagonal "
          "gates at the end should still be considered non-equivalent.")
      .def_readwrite(
          "transform_dynamic_circuit",
          &Configuration::Optimizations::transformDynamicCircuit,
          "Circuits containing dynamic circuit primitives such as mid-circuit "
          "measurements, resets, or classically-controlled operations cannot "
          "be verified in a straight-forward fashion due to the non-unitary "
          "nature of these primitives, which is why this setting defaults to "
          ":code:`False`. By enabling this optimization, any dynamic circuit "
          "is first transformed to a circuit without non-unitary primitives "
          "by, first, substituting qubit resets with new qubits and, then, "
          "applying the deferred measurement principle to defer measurements "
          "to the end.")
      .def_readwrite(
          "reorder_operations",
          &Configuration::Optimizations::reorderOperations,
          "The operations of a circuit are stored in a sequential container. "
          "This introduces some dependencies in the order of operations that "
          "are not naturally present in the quantum circuit. As a consequence, "
          "two quantum circuits that contain exactly the same operations, list "
          "their operations in different ways, also apply there operations in "
          "a different order. This optimization pass established a canonical "
          "ordering of operations by, first, constructing a directed, acyclic "
          "graph for the operations and, then, traversing it in a "
          "breadth-first fashion. Defaults to :code:`True`.")
      .def_readwrite(
          "backpropagate_output_permutation",
          &Configuration::Optimizations::backpropagateOutputPermutation,
          "Backpropagate the output permutation to the input permutation. "
          "Defaults to :code:`False` since this might mess up the initially "
          "given input permutation. Can be helpful for dynamic quantum circuits"
          " that have been transformed to a static circuit by enabling the "
          ":attr:`transform_dynamic_circuit "
          "<.Configuration.Optimizations.transform_dynamic_circuit>` "
          "optimization.")
      .def_readwrite(
          "elide_permutations",
          &Configuration::Optimizations::elidePermutations,
          "Elide permutations from the circuit by permuting the qubits in the "
          "circuit and eliminating SWAP gates from the circuits. Defaults to "
          ":code:`True` as this typically boosts performance.");

  // application options
  application.def(py::init<>())
      .def_readwrite(
          "construction_scheme",
          &Configuration::Application::constructionScheme,
          "The :class:`Application Scheme <.ApplicationScheme>` used for the "
          ":ref:`construction checker <EquivalenceChecking:Construction "
          "Equivalence Checker (using Decision Diagrams)>`.")
      .def_readwrite(
          "simulation_scheme", &Configuration::Application::simulationScheme,
          "The :class:`Application Scheme <.ApplicationScheme>` used for the "
          ":ref:`simulation checker <EquivalenceChecking:Simulation "
          "Equivalence Checker (using Decision Diagrams)>`.")
      .def_readwrite(
          "alternating_scheme", &Configuration::Application::alternatingScheme,
          "The :class:`Application Scheme <.ApplicationScheme>` used for the "
          ":ref:`alternating checker <EquivalenceChecking:Alternating "
          "Equivalence Checker (using Decision Diagrams)>`.")
      .def_readwrite(
          "profile", &Configuration::Application::profile,
          "The :attr:`Gate Cost <.ApplicationScheme.gate_cost>` application "
          "scheme can be configured with a profile that specifies the cost of "
          "gates. At the moment, this profile can be set via a file that is "
          "constructed similar to a lookup table. Every line :code:`<GATE_ID> "
          "<N_CONTROLS> <COST>` specified the cost for a given gate type and "
          "with a certain number of controls, e.g., :code:`X 0 1` denotes that "
          "a single-qubit X gate has a cost of :code:`1`, while :code:`X 2 15` "
          "denotes that a Toffoli gate has a cost of :code:`15`.");

  // functionality options
  functionality.def(py::init<>())
      .def_readwrite(
          "trace_threshold", &Configuration::Functionality::traceThreshold,
          "While decision diagrams are canonical in theory, i.e., equivalent "
          "circuits produce equivalent decision diagrams, numerical "
          "inaccuracies and approximations can harm this property. This can "
          "result in a scenario where two decision diagrams are really close "
          "to one another, but cannot be identified as such by standard "
          "methods (i.e., comparing their root pointers). Instead, for two "
          "decision diagrams :code:`U` and :code:`U'` representing the "
          "functionalities of two circuits :code:`G` and :code:`G'`, the trace "
          "of the product of one decision diagram with the inverse of the "
          "other can be computed and compared to the trace of the identity. "
          "Alternatively, it can be checked, whether :code:`U*U`^-1` is "
          "\"close enough\" to the identity by recursively checking that each "
          "decision diagram node is close enough to the identity structure "
          "(i.e., the first and last successor have weights close to one, "
          "while the second and third successor have weights close to zero). "
          "Whenever any decision diagram node differs from this structure by "
          "more than the configured threshold, the circuits are concluded to "
          "be non-equivalent. Defaults to :code:`1e-8`.")
      .def_readwrite(
          "check_partial_equivalence",
          &Configuration::Functionality::checkPartialEquivalence,
          "Two circuits are partially equivalent if, for each possible initial "
          "input state, they have the same probability for each measurement "
          "outcome. If set to :code:`True`, a check for partial equivalence "
          "will be performed and the contributions of garbage qubits to the "
          "circuit are ignored. If set to :code:`False`, the checker will "
          "output 'not equivalent' for circuits that are partially equivalent "
          "but not totally equivalent. In particular, garbage qubits will be "
          "treated as if they were measured qubits. Defaults to "
          ":code:`False`.");

  // simulation options
  simulation.def(py::init<>())
      .def_readwrite(
          "fidelity_threshold", &Configuration::Simulation::fidelityThreshold,
          "Similar to :attr:`trace threshold "
          "<.Configuration.Functionality.trace_threshold>`, this setting is "
          "here to tackle numerical inaccuracies and approximations for the "
          ":ref:`simulation checker <EquivalenceChecking:Simulation "
          "Equivalence Checker (using Decision Diagrams)>`. Instead of "
          "computing a trace, the fidelity between the states resulting from "
          "the simulation is computed. Whenever the fidelity differs from "
          ":code:`1.` by more than the configured threshold, the circuits are "
          "concluded to be non-equivalent. Defaults to :code:`1e-8`.")
      .def_readwrite(
          "max_sims", &Configuration::Simulation::maxSims,
          "The maximum number of simulations to be started for the "
          ":ref:`simulation checker <EquivalenceChecking:Simulation "
          "Equivalence Checker (using Decision Diagrams)>`. In practice, just "
          "a couple of simulations suffice in most cases to detect a potential "
          "non-equivalence. Either defaults to :code:`16` or the maximum "
          "number of available threads minus 2, whichever is more.")
      .def_readwrite(
          "state_type", &Configuration::Simulation::stateType,
          "The :class:`type of states <.StateType>` used for the simulations "
          "in the :ref:`simulation checker <EquivalenceChecking:Simulation "
          "Equivalence Checker (using Decision Diagrams)>`.")
      .def_readwrite("seed", &Configuration::Simulation::seed,
                     "The seed used in the quantum state generator. Defaults "
                     "to :code:`0`, which means that the seed is chosen "
                     "non-deterministically for each program run.")
      .def_readwrite(
          "store_cex_input", &Configuration::Simulation::storeCEXinput,
          "Whether to store the input state that has lead to the determination "
          "of a counterexample. Since the memory required to store a full "
          "representation of a quantum state increases exponentially, this is "
          "only recommended for a small number of qubits and defaults to "
          ":code:`False`.")
      .def_readwrite(
          "store_cex_output", &Configuration::Simulation::storeCEXoutput,
          "Whether to store the resulting states that prove the "
          "non-equivalence of both circuits. Since the memory required to "
          "store a full representation of a quantum state increases "
          "exponentially, this is only recommended for a small number of "
          "qubits and defaults to :code:`False`.");

  // parameterized options
  parameterized.def(py::init<>())
      .def_readwrite("parameterized_tolerance",
                     &Configuration::Parameterized::parameterizedTol,
                     "Set threshold below which instantiated parameters shall "
                     "be considered zero.")
      .def_readwrite(
          "additional_instantiations",
          &Configuration::Parameterized::nAdditionalInstantiations,
          "Number of instantiations shall be performed in addition to the "
          "default ones. "
          "For parameterized circuits that cannot be shown to be equivalent "
          "by the ZX checker "
          "the circuits are instantiated with concrete values for parameters "
          "and subsequently "
          "checked with QCEC's default schemes. The first instantiation "
          "tries to set as many "
          "gate parameters to 0. The last instantiations initializes the "
          "parameters with "
          "random values to guarantee completeness of the equivalence check. "
          "Because random "
          "instantiation is costly, additional instantiations can be "
          "performed that lead "
          "to simpler equivalence checking instances as the random "
          "instantiation. This option "
          "changes how many of those additional checks are performed.");
}
} // namespace ec
