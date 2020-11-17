/*
 * This file is part of IIC-JKU QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include <iostream>
#include <string>
#include <locale>
#include <algorithm>

#include "PowerOfSimulationEquivalenceChecker.hpp"

void show_usage(const std::string& name) {
	std::cerr << "Usage: " << name << " <PATH_TO_FILE_1> <PATH_TO_FILE_2>                                                           " << std::endl;
	std::cerr << "Supported file formats:                                                                                           " << std::endl;
	std::cerr << "  .real                                                                                                           " << std::endl;
	std::cerr << "  .qasm                                                                                                           " << std::endl;
	std::cerr << "Options:                                                                                                          " << std::endl;
	std::cerr << "  --modify_seed mod_seed                                      seed for circuit modifications                      " << std::endl;
	std::cerr << "  --remove X                                                  remove X random gates from 2nd circuit              " << std::endl;
	std::cerr << "  --add X                                                     add X random gates to 2nd circuit                   " << std::endl;
	std::cerr << "  --toffFront X                                               add X random Toffolis to front of 2nd circuit       " << std::endl;
	std::cerr << "  --toffRear X                                                add X random Toffolis to rear of 2nd circuit        " << std::endl;
	std::cerr << "  --simulation_seed sim_seed                                  seed for simulation inputs                          " << std::endl;
	std::cerr << "  --stimuliType classical | localquantum | globalquantum      type of stimuli to use                              " << std::endl;
}

int main(int argc, char** argv){
	if (argc < 3) {
		show_usage(argv[0]);
		return 1;
	}

	std::array<qc::OpType, 6> ops {qc::X, qc::Y, qc::Z , qc::H, qc::S, qc::T};

	// get filenames
	std::string file1 = argv[1];
	std::string file2 = argv[2];

	// read circuits
	qc::QuantumComputation qc1;
	qc1.import(file1);

	qc::QuantumComputation qc2;
	qc2.import(file2);

	ec::Configuration config{};
	config.stimuliType = ec::Classical;

	unsigned long long modify_seed = 0;
	unsigned long long gates_to_modify = 0;
	unsigned long long simulation_seed = 0;

	auto mt = std::mt19937_64(modify_seed);
	auto distribution = std::uniform_int_distribution<unsigned long long>(0, qc2.getNops()-1);
	auto rng = [&]() { return distribution(mt); };

	std::ostringstream resoss{};
	resoss << "results";

	// parse configuration options
	if (argc >= 4) {
		for (int i = 3; i < argc; ++i) {
			std::string cmd = argv[i];
			std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });

			if (cmd == "--modify_seed") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					modify_seed = std::stoull(cmd);
					mt.seed(modify_seed);
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--simulation_seed") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					simulation_seed = std::stoull(cmd);
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--stimuliType") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				if (cmd == "classical") {
					config.stimuliType = ec::Classical;
				} else if (cmd == "localquantum") {
					config.stimuliType = ec::LocalQuantum;
					resoss << "_" << "localquantum";
				} else if (cmd == "globalquantum" ) {
					config.stimuliType = ec::GlobalQuantum;
					resoss << "_" << "globalquantum";
				} else {
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--remove") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					gates_to_modify = std::stoull(cmd);

					for (auto j = 0u; j < gates_to_modify; ++j) {
						auto gate_to_remove = rng() % qc2.getNops();
						auto it = qc2.begin();
						std::advance(it, gate_to_remove);
						if (it == qc2.end()) {
							qc2.erase(--it);
						} else {
							qc2.erase(it);
						}
					}
					resoss << "_" << "remove" << gates_to_modify;
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--add") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					gates_to_modify = std::stoull(cmd);

					for (auto k=0u; k<gates_to_modify; ++k) {
						auto target = rng() % qc2.getNqubits();
						auto gate = ops.at(rng() % ops.size());
						auto position = rng() % qc2.getNops();
						auto it = qc2.begin();
						std::advance(it, position);
						qc2.insert(it, std::make_unique<qc::StandardOperation>(qc2.getNqubits(), target, gate));
					}
					resoss << "_" << "add" << gates_to_modify;
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--tofffront") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					gates_to_modify = std::stoull(cmd);

					for (auto k=0u; k<gates_to_modify; ++k) {
						auto target = rng() % qc2.getNqubits();
						auto control0 = qc::Control(rng() % qc2.getNqubits());
						while (control0.qubit == target) {
							control0 = qc::Control(rng() % qc2.getNqubits());
						}
						auto control1 = qc::Control(rng() % qc2.getNqubits());
						while (control1.qubit == target || control1.qubit == control0.qubit) {
							control1 = qc::Control(rng() % qc2.getNqubits());
						}
						qc2.insert<std::unique_ptr<qc::StandardOperation>>(qc2.begin(), std::make_unique<qc::StandardOperation>(qc2.getNqubits(), std::vector<qc::Control>{control0, control1}, target));
					}
					resoss << "_" << "toffFront" << gates_to_modify;
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else if (cmd == "--toffrear") {
				++i;
				if (i >= argc) {
					show_usage(argv[0]);
					return 1;
				}
				cmd = argv[i];
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });
				try {
					gates_to_modify = std::stoull(cmd);

					for (auto k=0u; k<gates_to_modify; ++k) {
						auto target = rng() % qc2.getNqubits();
						auto control0 = qc::Control(rng() % qc2.getNqubits());
						while (control0.qubit == target) {
							control0 = qc::Control(rng() % qc2.getNqubits());
						}
						auto control1 = qc::Control(rng() % qc2.getNqubits());
						while (control1.qubit == target || control1.qubit == control0.qubit) {
							control1 = qc::Control(rng() % qc2.getNqubits());
						}
						qc2.emplace_back<qc::StandardOperation>(qc2.getNqubits(), std::vector<qc::Control>{control0, control1}, target);
					}
					resoss << "_" << "toffRear" << gates_to_modify;
				} catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					show_usage(argv[0]);
					return 1;
				}
			} else {
				show_usage(argv[0]);
				return 1;
			}
		}
	}
	resoss << ".csv";

	try {
		ec::PowerOfSimulationEquivalenceChecker ec(qc1, qc2, simulation_seed);
		ec.expectNothing();
		ec.check(config);

		std::ostringstream oss{};
		oss << qc1.getName() << ";" << modify_seed << ";" << simulation_seed
			<< ";" << ec.results.nsims << ";" << ec.results.time << ";"
			<< (ec.results.equivalence == ec::NonEquivalent) << std::endl;
		std::cout << oss.str();

		std::ofstream ofs{};
		ofs.open(resoss.str(), std::ios::app);
		if (!ofs.is_open()) {
			std::cout << "Could not open results file" << std::endl;
			return 1;
		}
		ofs << oss.str() << std::flush;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
