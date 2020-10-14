/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "nlohmann/json.hpp"
#include "pybind11/pybind11.h"
#include "pybind11_json/pybind11_json.hpp"

#include "CompilationFlowEquivalenceChecker.hpp"
#include "PowerOfSimulationEquivalenceChecker.hpp"

namespace py = pybind11;
namespace nl = nlohmann;
using namespace pybind11::literals;


nl::json verify(const py::object& instance) {
	if (!instance.contains("file1") || !instance.contains("file2")) {
		return {"error", R"("file1" and "file2" need to be specified in config)"};
	}
	std::string file1 = instance["file1"].cast<std::string>();
	std::string file2 = instance["file2"].cast<std::string>();

	ec::Configuration config{};
	ec::Method method = ec::Proportional;
	if (instance.contains("method")) {
		try {
			method = instance["method"].cast<ec::Method>();
		} catch(std::exception const& e) {
			try {
				nl::from_json(instance["method"].cast<std::string>(), method);
			} catch(std::exception const& e) {
				std::stringstream ss{};
				ss << "Could not parse method: " << e.what();
				return {"error", ss.str()};
			}
		}
	}

	if (instance.contains("tolerance")) {
		config.tolerance = instance["tolerance"].cast<fp>();
	}

	if (instance.contains("nsims")) {
		config.max_sims = instance["nsims"].cast<unsigned long long>();
	}

	if (instance.contains("fidelity")) {
		config.fidelity_limit = instance["fidelity"].cast<fp>();
	}

	if (instance.contains("csv")) {
		config.printCSV = instance["csv"].cast<bool>();
	}

	if (instance.contains("statistics")) {
		config.printStatistics = instance["statistics"].cast<bool>();
	}

	qc::QuantumComputation qc1;
	qc::QuantumComputation qc2;
	// read circuits
	try {
		qc1.import(file1);
		qc2.import(file2);
	} catch (std::exception const& e) {
		std::stringstream ss{};
		ss << "Could not import input files: " << e.what();
		return {"error", ss.str()};
	}

	std::unique_ptr<ec::EquivalenceChecker> ec;
	try {
		if (method == ec::CompilationFlow) {
			ec = std::make_unique<ec::CompilationFlowEquivalenceChecker>(qc1, qc2);
		} else if (method == ec::PowerOfSimulation) {
			ec = std::make_unique<ec::PowerOfSimulationEquivalenceChecker>(qc1, qc2);
		} else {
			ec = std::make_unique<ec::ImprovedDDEquivalenceChecker>(qc1, qc2);
		}
	} catch (std::exception const& e) {
		std::stringstream ss{};
		ss << "Could not construct equivalence checker: " << e.what();
		return {"error", ss.str()};
	}
	ec->expectNothing();
	try {
		ec->check(config);
	} catch (std::exception const& e) {
		std::stringstream ss{};
		ss << "Error during equivalence check: " << e.what();
		return {"error", ss.str()};
	}

	return ec->results.produceJSON(config.printStatistics);
}

PYBIND11_MODULE(_qcec, m) {
	m.doc() = "pybind11 for the JKQ QCEC quantum circuit equivalence checking tool";
	m.attr("__name__") = "jkq.qcec";
	m.def("verify", &verify, "verify the equivalence of two circuits");

	py::enum_<ec::Method>(m, "Method")
	        .value("reference", ec::Method::Reference)
	        .value("naive", ec::Method::Naive)
	        .value("proportional", ec::Method::Proportional)
	        .value("lookahead", ec::Method::Lookahead)
	        .value("simulation", ec::Method::PowerOfSimulation)
	        .value("compilationflow", ec::Method::CompilationFlow)
	        .export_values();

	#ifdef VERSION_INFO
		m.attr("__version__") = VERSION_INFO;
	#else
		m.attr("__version__") = "dev";
	#endif
}
