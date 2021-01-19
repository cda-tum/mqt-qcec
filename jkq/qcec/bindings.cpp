/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "nlohmann/json.hpp"
#include "pybind11/pybind11.h"
#include "pybind11_json/pybind11_json.hpp"

#include "QiskitImport.hpp"

#include "CompilationFlowEquivalenceChecker.hpp"
#include "PowerOfSimulationEquivalenceChecker.hpp"

namespace py = pybind11;
namespace nl = nlohmann;
using namespace pybind11::literals;

// c++ binding function
nl::json ver(const py::object& circ1, const py::object& circ2, const nl::json& jsonConfig) {
	qc::QuantumComputation qc1{};
	try {
		if(py::isinstance<py::str>(circ1)) {
			auto&& file1 = circ1.cast<std::string>();
			qc1.import(file1);
		} else {
			import(qc1, circ1);
		}
	} catch (std::exception const& e) {
		std::stringstream ss{};
		ss << "Could not import first circuit: " << e.what();
		return {{"error", ss.str()}};
	}

	qc::QuantumComputation qc2{};
	try {
		if(py::isinstance<py::str>(circ2)) {
			std::string file2 = circ2.cast<std::string>();
			qc2.import(file2);
		} else {
			import(qc2, circ2);
		}
	} catch (std::exception const& e) {
		std::stringstream ss{};
		ss << "Could not import second circuit: " << e.what();
		return {{"error", ss.str()}};
	}

	ec::Configuration config{};
	ec::Method method = ec::Proportional;
	if (jsonConfig.contains("method")) {
		nl::from_json(jsonConfig["method"].get<std::string>(), method);
	}

	if (jsonConfig.contains("tolerance")) {
		config.tolerance = jsonConfig["tolerance"].get<fp>();
	}

	if (jsonConfig.contains("nsims")) {
		config.max_sims = jsonConfig["nsims"].get<unsigned long long>();
	}

	if (jsonConfig.contains("stimuliType")) {
		nl::from_json(jsonConfig["stimuliType"].get<std::string>(), config.stimuliType);
	}

	if (jsonConfig.contains("fidelity")) {
		config.fidelity_limit = jsonConfig["fidelity"].get<fp>();
	}

	if (jsonConfig.contains("csv")) {
		config.printCSV = jsonConfig["csv"].get<bool>();
	}

	if (jsonConfig.contains("statistics")) {
		config.printStatistics = jsonConfig["statistics"].get<bool>();
	}

	if (jsonConfig.contains("storeCEXinput")) {
		config.storeCEXinput = jsonConfig["storeCEXinput"].get<bool>();
	}

	if (jsonConfig.contains("storeCEXoutput")) {
		config.storeCEXoutput = jsonConfig["storeCEXoutput"].get<bool>();
	}

	if (jsonConfig.contains("swapGateFusion")) {
		config.swapGateFusion = jsonConfig["swapGateFusion"].get<bool>();
	}

	if (jsonConfig.contains("singleQubitGateFusion")) {
		config.singleQubitGateFusion = jsonConfig["singleQubitGateFusion"].get<bool>();
	}

	if (jsonConfig.contains("removeDiagonalGatesBeforeMeasure")) {
		config.removeDiagonalGatesBeforeMeasure = jsonConfig["removeDiagonalGatesBeforeMeasure"].get<bool>();
	}

	std::unique_ptr<ec::EquivalenceChecker> ec;
	try {
		if (method == ec::CompilationFlow) {
			ec = std::make_unique<ec::CompilationFlowEquivalenceChecker>(qc1, qc2);
		} else if (method == ec::PowerOfSimulation) {
			ec = std::make_unique<ec::PowerOfSimulationEquivalenceChecker>(qc1, qc2);
		} else {
			ec = std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc1, qc2, method);
		}
	} catch (std::exception const& e) {
		std::stringstream ss{};
		ss << "Could not construct equivalence checker: " << e.what();
		return {{"error", ss.str()}};
	}
	ec->expectNothing();
	try {
		ec->check(config);
	} catch (std::exception const& e) {
		std::stringstream ss{};
		ss << "Error during equivalence check: " << e.what();
		return {{"error", ss.str()}};
	}

	auto result = ec->results.produceJSON(config.printStatistics);
	if (config.printCSV)
		result["csv"] = ec->results.produceCSVEntry();

	return result;
}

PYBIND11_MODULE(pyqcec, m) {
	m.doc() = "pybind11 for the JKQ QCEC quantum circuit equivalence checking tool";
	m.def("ver", &ver, "verify the equivalence of two circuits");

	py::enum_<ec::Method>(m, "Method")
	        .value("reference", ec::Method::Reference)
	        .value("naive", ec::Method::Naive)
	        .value("proportional", ec::Method::Proportional)
	        .value("lookahead", ec::Method::Lookahead)
	        .value("simulation", ec::Method::PowerOfSimulation)
	        .value("compilationflow", ec::Method::CompilationFlow)
	        .export_values();

	py::enum_<ec::StimuliType>(m, "StimuliType")
			.value("classical", ec::StimuliType::Classical)
			.value("localquantum", ec::StimuliType::LocalQuantum)
			.value("globalquantum", ec::StimuliType::GlobalQuantum)
			.export_values();

	#ifdef VERSION_INFO
		m.attr("__version__") = VERSION_INFO;
	#else
		m.attr("__version__") = "dev";
	#endif
}
