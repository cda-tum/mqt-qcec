/*
 * This file is part of JKQ QCEC library which is released under the MIT license.
 * See file README.md or go to http://iic.jku.at/eda/research/quantum/ for more information.
 */

#include "CompilationFlowEquivalenceChecker.hpp"
#include "EquivalenceChecker.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"
#include "SimulationBasedEquivalenceChecker.hpp"

#include <algorithm>
#include <iostream>
#include <locale>
#include <string>

void show_usage(const std::string& name) {
    std::cerr << "Usage: " << name << " <PATH_TO_FILE_1> <PATH_TO_FILE_2> (--method <method>)    " << std::endl;
    std::cerr << "Supported file formats:                                                        " << std::endl;
    std::cerr << "  .real                                                                        " << std::endl;
    std::cerr << "  .qasm                                                                        " << std::endl;
    std::cerr << "  .tfc                                                                         " << std::endl;
    std::cerr << "  .qc                                                                          " << std::endl;
    std::cerr << "Available methods:                                                             " << std::endl;
    std::cerr << "  reference                                                                    " << std::endl;
    std::cerr << "  naive                                                                        " << std::endl;
    std::cerr << "  proportional (default)                                                       " << std::endl;
    std::cerr << "  lookahead                                                                    " << std::endl;
    std::cerr << "  simulation (using 'classical', 'localquantum', or 'globalquantum' stimuli)   " << std::endl;
    std::cerr << "  compilationflow                                                              " << std::endl;
    std::cerr << "Result Options:                                                                                               " << std::endl;
    std::cerr << "  --ps:                                   Print statistics                                                    " << std::endl;
    std::cerr << "  --csv:                                  Print results as csv string                                         " << std::endl;
    std::cerr << "  --storeCEXinput:                        Store counterexample input state vector (for simulation method)     " << std::endl;
    std::cerr << "  --storeCEXoutput:                       Store resulting counterexample state vectors (for simulation method)" << std::endl;
    std::cerr << "Verification Parameters:                                                                          " << std::endl;
    std::cerr << "  --tol e (default 1e-13):                Numerical tolerance used during computation             " << std::endl;
    std::cerr << "  --nsims r (default 16):                 Number of simulations to conduct (for simulation method)" << std::endl;
    std::cerr << "  --fid F (default 0.999):                Fidelity limit for comparison (for simulation method)   " << std::endl;
    std::cerr << "  --stimuliType s (default 'classical'):  Type of stimuli to use (for simulation method)          " << std::endl;
    std::cerr << "Optimization Options:                                                                             " << std::endl;
    std::cerr << "  --swapReconstruction:                   reconstruct SWAP operations                             " << std::endl;
    std::cerr << "  --singleQubitGateFusion:                fuse consecutive single qubit gates                     " << std::endl;
    std::cerr << "  --removeDiagonalGatesBeforeMeasure:     remove diagonal gates before measurements               " << std::endl;
}

int main(int argc, char** argv) {
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

    // parse configuration options
    if (argc >= 4) {
        for (int i = 3; i < argc; ++i) {
            std::string cmd = argv[i];
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });

            if (cmd == "--tol") {
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
            } else if (cmd == "--method") {
                ++i;
                if (i >= argc) {
                    show_usage(argv[0]);
                    return 1;
                }
                cmd = argv[i];
                std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return ::tolower(c); });

                // try to extract method
                if (cmd == "reference") {
                    config.method = ec::Method::Reference;
                } else if (cmd == "naive") {
                    config.method   = ec::Method::G_I_Gp;
                    config.strategy = ec::Strategy::Naive;
                } else if (cmd == "proportional") {
                    config.method   = ec::Method::G_I_Gp;
                    config.strategy = ec::Strategy::Proportional;
                } else if (cmd == "lookahead") {
                    config.method   = ec::Method::G_I_Gp;
                    config.strategy = ec::Strategy::Lookahead;
                } else if (cmd == "compilationflow") {
                    config.method   = ec::Method::G_I_Gp;
                    config.strategy = ec::Strategy::CompilationFlow;
                } else if (cmd == "simulation") {
                    config.method = ec::Method::Simulation;
                } else {
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
                    config.stimuliType = ec::StimuliType::Classical;
                } else if (cmd == "localquantum") {
                    config.stimuliType = ec::StimuliType::LocalQuantum;
                } else if (cmd == "globalquantum") {
                    config.stimuliType = ec::StimuliType::GlobalQuantum;
                } else {
                    show_usage(argv[0]);
                    return 1;
                }
            } else if (cmd == "--storeCEXinput") {
                config.storeCEXinput = true;
            } else if (cmd == "--storeCEXoutput") {
                config.storeCEXoutput = true;
            } else if (cmd == "--swapReconstruction") {
                config.reconstructSWAPs = true;
            } else if (cmd == "--singleQubitGateFusion") {
                config.fuseSingleQubitGates = true;
            } else if (cmd == "--removeDiagonalGatesBeforeMeasure") {
                config.removeDiagonalGatesBeforeMeasure = true;
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
    ec::EquivalenceCheckingResults results{};
    if (config.strategy == ec::Strategy::CompilationFlow) {
        ec::CompilationFlowEquivalenceChecker ec(qc1, qc2);
        results = ec.check(config);
    } else if (config.method == ec::Method::Simulation) {
        ec::SimulationBasedEquivalenceChecker ec(qc1, qc2);
        results = ec.check(config);
    } else {
        ec::ImprovedDDEquivalenceChecker ec(qc1, qc2);
        results = ec.check(config);
    }
    results.printJSON();

    return 0;
}
