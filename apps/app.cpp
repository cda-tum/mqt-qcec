/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <string>
#include <locale>
#include <algorithm>

#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"
#include "CompilationFlowEquivalenceChecker.hpp"
#include "PowerOfSimulationEquivalenceChecker.hpp"

void show_usage(const std::string& name) {
	std::cerr << "Usage: " << name << " <PATH_TO_FILE_1> <PATH_TO_FILE_2> (--method <method>)               " << std::endl;
	std::cerr << "Supported file formats:                                                                   " << std::endl;
	std::cerr << "  .real                                                                                   " << std::endl;
	std::cerr << "  .qasm                                                                                   " << std::endl;
	std::cerr << "  .tfc                                                                                    " << std::endl;
	std::cerr << "  .qc                                                                                     " << std::endl;
	std::cerr << "Available methods:                                                                        " << std::endl;
	std::cerr << "  reference                                                                               " << std::endl;
	std::cerr << "  naive                                                                                   " << std::endl;
	std::cerr << "  proportional (default)                                                                  " << std::endl;
	std::cerr << "  lookahead                                                                               " << std::endl;
	std::cerr << "  simulation                                                                              " << std::endl;
	std::cerr << "  compilationflow                                                                         " << std::endl;
	std::cerr << "Options:                                                                                  " << std::endl;
	std::cerr << "  --ps:                           Print statistics                                        " << std::endl;
	std::cerr << "  --csv:                          Print results as csv string                             " << std::endl;
	std::cerr << "  --tol e (default 1e-13):        Numerical tolerance used during computation             " << std::endl;
	std::cerr << "  --nsims r (default 16):         Number of simulations to conduct (for simulation method)" << std::endl;
	std::cerr << "  --fid F (default 0.999):        Fidelity limit for comparison (for simulation method)   " << std::endl;
}

int main(int argc, char** argv){
	if (argc < 3) {
		if (argc == 2) {
			std::string cmd = argv[1];
			std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
			if (cmd == "--help" || cmd == "-h")
				show_usage(argv[0]);
		} else {
			show_usage(argv[0]);
		}
		return 1;
	}

	// get filenames
	std::string file1 = argv[1];
	std::string file2 = argv[2];

	ec::Configuration config{};
	ec::Method method = ec::Proportional;
	bool printStatistics = false;

	// parse configuration options
	if (argc >= 4) {
		for (int i = 3; i < argc; ++i) {
			std::string cmd = argv[i];
			std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });

			if (cmd == "--csv") {
				config.printCSV = true;
			} else if (cmd == "--tol") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					config.tolerance = std::stod(cmd);
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--nsims") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					config.max_sims = std::stoull(cmd);
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--fid") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					config.fidelity_limit = std::stod(cmd);
					if (config.fidelity_limit < 0. || config.fidelity_limit > 1.) {
						std::cerr << "Fidelity should be between 0 and 1" << std::endl;
						show_usage(argv[0]);
						return 1;
					}
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--ps") {
				printStatistics = true;
			} else if (cmd == "--method"){
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });

				// try to extract method
				if (cmd == "reference") {
					method = ec::Reference;
				} else if (cmd == "naive") {
					method = ec::Naive;
				} else if (cmd == "proportional") {
					method = ec::Proportional;
				} else if (cmd == "lookahead") {
					method = ec::Lookahead;
				} else if (cmd == "compilationflow") {
					method = ec::CompilationFlow;
				} else if (cmd == "simulation") {
					method = ec::PowerOfSimulation;
				} else {
					show_usage(argv[0]);
					return 1;
				}
			} else {
				show_usage(argv[0]);
				return 1;
			}
		}
	}

	// read circuits
	qc::QuantumComputation qc1(file1);
	qc::QuantumComputation qc2(file2);

	// perform equivalence check
	if (method == ec::CompilationFlow) {
		ec::CompilationFlowEquivalenceChecker ec(qc1, qc2);
		ec.expectNothing();
		ec.check(config);
		if (config.printCSV) {
			ec.printCSVEntry();
		} else {
			ec.printJSONResult(printStatistics);
		}
	} else if (method == ec::PowerOfSimulation) {
		ec::PowerOfSimulationEquivalenceChecker ec(qc1, qc2);
		ec.expectNothing();
		ec.check(config);
		if (config.printCSV) {
			ec.printCSVEntry();
		} else {
			ec.printJSONResult(printStatistics);
		}
	} else {
		ec::ImprovedDDEquivalenceChecker ec(qc1, qc2, method);
		ec.expectNothing();
		ec.check(config);
		if (config.printCSV) {
			ec.printCSVEntry();
		} else {
			ec.printJSONResult(printStatistics);
		}
	}

	return 0;
}
