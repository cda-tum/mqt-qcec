/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "CompilationFlowEquivalenceChecker.hpp"
#include "SimulationBasedEquivalenceChecker.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11_json/pybind11_json.hpp"
#include "qiskit/QuantumCircuit.hpp"

namespace py = pybind11;
namespace nl = nlohmann;
using namespace pybind11::literals;

ec::EquivalenceCheckingResults verify(const py::object&        circ1,
                                      const py::object&        circ2,
                                      const ec::Configuration& config) {
    ec::EquivalenceCheckingResults results{};

    qc::QuantumComputation qc1{};
    try {
        if (py::isinstance<py::str>(circ1)) {
            auto&& file1 = circ1.cast<std::string>();
            qc1.import(file1);
        } else {
            qc::qiskit::QuantumCircuit::import(qc1, circ1);
        }
    } catch (std::exception const& e) {
        py::print("Could not import first circuit: ", e.what());
        return results;
    }

    qc::QuantumComputation qc2{};
    try {
        if (py::isinstance<py::str>(circ2)) {
            auto&& file2 = circ2.cast<std::string>();
            qc2.import(file2);
        } else {
            qc::qiskit::QuantumCircuit::import(qc2, circ2);
        }
    } catch (std::exception const& e) {
        py::print("Could not import second circuit: ", e.what());
        return results;
    }

    std::unique_ptr<ec::EquivalenceChecker> ec;
    try {
        if (config.method == ec::Method::Reference) {
            ec = std::make_unique<ec::EquivalenceChecker>(qc1, qc2);
        } else if (config.method == ec::Method::G_I_Gp) {
            if (config.strategy == ec::Strategy::CompilationFlow) {
                ec = std::make_unique<ec::CompilationFlowEquivalenceChecker>(qc1, qc2);
            } else {
                ec = std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc1, qc2);
            }
        } else if (config.method == ec::Method::Simulation) {
            ec = std::make_unique<ec::SimulationBasedEquivalenceChecker>(qc1, qc2);
        }
    } catch (std::exception const& e) {
        py::print("Could not construct equivalence checker: ", e.what());
        return results;
    }

    try {
        results = ec->check(config);
    } catch (std::exception const& e) {
        py::print("Error during equivalence check: ", e.what());
        return results;
    }

    return results;
}

PYBIND11_MODULE(pyqcec, m) {
    m.doc() = "Python interface for the JKQ QCEC quantum circuit equivalence checking tool";

    py::enum_<ec::Method>(m, "Method")
            .value("reference", ec::Method::Reference)
            .value("G_I_Gp", ec::Method::G_I_Gp)
            .value("simulation", ec::Method::Simulation)
            .export_values();

    py::enum_<ec::Strategy>(m, "Strategy")
            .value("naive", ec::Strategy::Naive)
            .value("proportional", ec::Strategy::Proportional)
            .value("lookahead", ec::Strategy::Lookahead)
            .value("compilationflow", ec::Strategy::CompilationFlow)
            .export_values();

    py::enum_<ec::StimuliType>(m, "StimuliType")
            .value("classical", ec::StimuliType::Classical)
            .value("localquantum", ec::StimuliType::LocalQuantum)
            .value("globalquantum", ec::StimuliType::GlobalQuantum)
            .export_values();

    py::enum_<ec::Equivalence>(m, "Equivalence")
            .value("no_information", ec::Equivalence::NoInformation)
            .value("not_equivalent", ec::Equivalence::NotEquivalent)
            .value("equivalent", ec::Equivalence::Equivalent)
            .value("equivalent_up_to_global_phase", ec::Equivalence::EquivalentUpToGlobalPhase)
            .value("probably_equivalent", ec::Equivalence::ProbablyEquivalent);

    py::class_<ec::Configuration>(m, "Configuration",
                                  "Configuration options for the JKQ QCEC quantum circuit equivalence checking tool")
            .def(py::init<>())
            .def_readwrite(
                    "method", &ec::Configuration::method,
                    R"pbdoc(
					Equivalence checking method to use
					- reference
					- G_I_Gp (*default*)
					- simulation
				)pbdoc")
            .def_readwrite(
                    "strategy", &ec::Configuration::strategy,
                    R"pbdoc(
					Strategy to use for the G -> I <- G' scheme
					- naive
					- proportional (*default*)
					- lookahead
					- compilationflow
				)pbdoc")
            .def_readwrite("tolerance", &ec::Configuration::tolerance,
                           R"pbdoc(
					Numerical tolerance used during computation
				)pbdoc")
            .def_readwrite("reconstruct_swaps", &ec::Configuration::reconstructSWAPs,
                           R"pbdoc(
					Optimization pass reconstructing SWAP operations
				)pbdoc")
            .def_readwrite("fuse_single_qubit_gates", &ec::Configuration::fuseSingleQubitGates,
                           R"pbdoc(
					Optimization pass fusing consecutive single qubit gates
				)pbdoc")
            .def_readwrite("remove_diagonal_gates_before_measure", &ec::Configuration::removeDiagonalGatesBeforeMeasure,
                           R"pbdoc(
					Optimization pass removing diagonal gates before measurements
				)pbdoc")
            .def_readwrite("fidelity", &ec::Configuration::fidelity_limit,
                           R"pbdoc(
					Fidelity limit for comparison (for simulation method)
				)pbdoc")
            .def_readwrite("max_sims", &ec::Configuration::max_sims,
                           R"pbdoc(
					Maximum number of simulations to conduct (for simulation method)
				)pbdoc")
            .def_readwrite("stimuli_type", &ec::Configuration::stimuliType,
                           R"pbdoc(
					Type of stimuli to use (for simulation method):
					- classical (*default*)
					- localquantum
					- globalquantum
				)pbdoc")
            .def_readwrite("store_cex_input", &ec::Configuration::storeCEXinput,
                           R"pbdoc(
					Store counterexample input state vector (for simulation method)
				)pbdoc")
            .def_readwrite("store_cex_output", &ec::Configuration::storeCEXoutput,
                           R"pbdoc(
					Store resulting counterexample state vectors (for simulation method)
				)pbdoc")
            .def("__repr__", &ec::Configuration::toString);

    py::class_<ec::EquivalenceCheckingResults>(m, "Results",
                                               "Results of the JKQ QCEC quantum circuit equivalence checking tool")
            .def(py::init<>())
            .def_readwrite(
                    "circuit1", &ec::EquivalenceCheckingResults::circuit1,
                    R"pbdoc(
					Information on first circuit
				)pbdoc")
            .def_readwrite(
                    "circuit2", &ec::EquivalenceCheckingResults::circuit2,
                    R"pbdoc(
					Information on second circuit
				)pbdoc")
            .def_readwrite(
                    "method", &ec::EquivalenceCheckingResults::method,
                    R"pbdoc(
					Equivalence checking method used
				)pbdoc")
            .def_readwrite(
                    "strategy", &ec::EquivalenceCheckingResults::strategy,
                    R"pbdoc(
					Strategy used for the G -> I <- G' scheme
				)pbdoc")
            .def_readwrite(
                    "stimuli_type", &ec::EquivalenceCheckingResults::stimuliType,
                    R"pbdoc(
					Stimuli type used
				)pbdoc")
            .def_readwrite(
                    "equivalence", &ec::EquivalenceCheckingResults::equivalence,
                    R"pbdoc(
					Equivalence of both circuits
					- no information
					- not equivalent
					- equivalent
					- equivalent up to global phase
					- probably equivalent
				)pbdoc")
            .def_readwrite(
                    "preprocessing_time", &ec::EquivalenceCheckingResults::preprocessingTime,
                    R"pbdoc(
					Runtime of preprocessing in [s]
				)pbdoc")
            .def_readwrite(
                    "verification_time", &ec::EquivalenceCheckingResults::verificationTime,
                    R"pbdoc(
					Runtime of verification in [s]
				)pbdoc")
            .def_readwrite(
                    "max_active", &ec::EquivalenceCheckingResults::maxActive,
                    R"pbdoc(
					Maximum active decision diagram nodes during verification
				)pbdoc")
            .def_readwrite(
                    "n_sims", &ec::EquivalenceCheckingResults::nsims,
                    R"pbdoc(
					Number of simulations performed for simulation-based verification
				)pbdoc")
            .def_readwrite(
                    "cex_input", &ec::EquivalenceCheckingResults::cexInput,
                    R"pbdoc(
					Counterexample input state
				)pbdoc")
            .def_readwrite(
                    "fidelity", &ec::EquivalenceCheckingResults::fidelity,
                    R"pbdoc(
					Fidelity of the two resulting states
				)pbdoc")
            .def("__repr__", &ec::EquivalenceCheckingResults::toString)
            .def_static("csv_header", &ec::EquivalenceCheckingResults::getCSVHeader)
            .def("csv", &ec::EquivalenceCheckingResults::produceCSVEntry)
            .def("json", &ec::EquivalenceCheckingResults::produceJSON);

    py::class_<ec::EquivalenceCheckingResults::CircuitInfo>(m, "CircuitInfo",
                                                            "Circuit information")
            .def(py::init<>())
            .def_readwrite(
                    "name", &ec::EquivalenceCheckingResults::CircuitInfo::name,
                    R"pbdoc(
					Name of the circuit
				)pbdoc")
            .def_readwrite(
                    "n_qubits", &ec::EquivalenceCheckingResults::CircuitInfo::nqubits,
                    R"pbdoc(
					Number of qubits in the circuit
				)pbdoc")
            .def_readwrite(
                    "n_gates", &ec::EquivalenceCheckingResults::CircuitInfo::ngates,
                    R"pbdoc(
					Number of gates in the circuit
				)pbdoc")
            .def_readwrite(
                    "cex_output", &ec::EquivalenceCheckingResults::CircuitInfo::cexOutput,
                    R"pbdoc(
					Counterexample output state
				)pbdoc")
            .def("__repr__", &ec::EquivalenceCheckingResults::CircuitInfo::toString);

    m.def("verify", &verify, "verify the equivalence of two circuits",
          "circ1"_a, "circ2"_a,
          "config"_a = ec::Configuration{});

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
