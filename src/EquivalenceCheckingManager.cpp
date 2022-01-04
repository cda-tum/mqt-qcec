/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "EquivalenceCheckingManager.hpp"

namespace ec {
    void EquivalenceCheckingManager::setupAncillariesAndGarbage() {
        auto&          largerCircuit   = qc1.getNqubits() > qc2.getNqubits() ? this->qc1 : this->qc2;
        auto&          smallerCircuit  = qc1.getNqubits() > qc2.getNqubits() ? this->qc2 : this->qc1;
        dd::QubitCount qubitDifference = largerCircuit.getNqubits() - smallerCircuit.getNqubits();

        std::vector<std::pair<dd::Qubit, dd::Qubit>> removed{};
        removed.reserve(qubitDifference);

        const auto        nqubits = largerCircuit.getNqubits();
        std::vector<bool> garbage(nqubits);

        for (dd::QubitCount i = 0; i < qubitDifference; ++i) {
            auto logicalQubitIndex     = qc::QuantumComputation::getHighestLogicalQubitIndex(largerCircuit.initialLayout);
            garbage[logicalQubitIndex] = largerCircuit.logicalQubitIsGarbage(logicalQubitIndex);
            removed.push_back(largerCircuit.removeQubit(logicalQubitIndex));
        }

        // add appropriate ancillary register to smaller circuit
        smallerCircuit.addAncillaryRegister(qubitDifference);

        // reverse iterate over the removed qubits and add them back into the larger circuit as ancillary
        for (auto it = removed.rbegin(); it != removed.rend(); ++it) {
            largerCircuit.addAncillaryQubit(it->first, it->second);
            // restore garbage
            if (garbage[largerCircuit.getNqubits() - 1]) {
                largerCircuit.setLogicalQubitGarbage(static_cast<dd::Qubit>(largerCircuit.getNqubits() - 1));
            }
            // also set the appropriate qubits in the smaller circuit as garbage
            smallerCircuit.setLogicalQubitGarbage(static_cast<dd::Qubit>(largerCircuit.getNqubits() - 1));
        }
    }

    void EquivalenceCheckingManager::fixOutputPermutationMismatch() {
        // Try to fix potential mismatches in output permutations
        auto& smallerCircuit = qc1.getNqubits() > qc2.getNqubits() ? qc2 : qc1;
        auto& largerCircuit  = qc1.getNqubits() > qc2.getNqubits() ? qc1 : qc2;

        auto& smallerInitial   = smallerCircuit.initialLayout;
        auto& smallerOutput    = smallerCircuit.outputPermutation;
        auto& smallerAncillary = smallerCircuit.ancillary;
        auto& smallerGarbage   = smallerCircuit.garbage;

        auto& largerOutput  = largerCircuit.outputPermutation;
        auto& largerGarbage = largerCircuit.garbage;

        for (const auto& o: largerOutput) {
            dd::Qubit outputQubitInLargerCircuit = o.second;
            //             dd::Qubit physicalQubitInLargerCircuit = o.first;
            //             std::cout << "Output logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " at physical qubit " << static_cast<std::size_t>(physicalQubitInLargerCircuit);
            dd::Qubit nout = 1;
            for (dd::Qubit i = 0; i < outputQubitInLargerCircuit; ++i) {
                if (!largerGarbage[i])
                    ++nout;
            }
            // std::cout << " which is logical output qubit number " << static_cast<std::size_t>(nout) << std::endl;

            dd::QubitCount outcount                    = nout;
            dd::Qubit      outputQubitInSmallerCircuit = 0;
            bool           existsInSmaller             = false;
            for (dd::QubitCount i = 0; i < smallerCircuit.getNqubits(); ++i) {
                if (!smallerGarbage[i]) {
                    --outcount;
                }
                if (outcount == 0) {
                    outputQubitInSmallerCircuit = static_cast<dd::Qubit>(i);
                    existsInSmaller             = true;
                    break;
                }
            }
            // algorithm has logical qubit that does not exist in the smaller circuit
            if (!existsInSmaller)
                continue;

            // std::cout << "This is logical qubit " << static_cast<std::size_t>(outputQubitInSmallerCircuit) << " in the smaller circuit";
            dd::Qubit physicalQubitInSmallerCircuit = 0;
            for (const auto& out: smallerOutput) {
                if (out.second == outputQubitInSmallerCircuit) {
                    physicalQubitInSmallerCircuit = out.first;
                    break;
                }
            }
            // std::cout << " which is assigned to physical qubit " << static_cast<std::size_t>(physicalQubitInSmallerCircuit) << " at the end of the circuit" << std::endl;

            if (outputQubitInLargerCircuit != outputQubitInSmallerCircuit) {
                // std::cout << "Mismatch in the logical output qubits" << std::endl;
                if (smallerAncillary[outputQubitInLargerCircuit] && smallerGarbage[outputQubitInLargerCircuit]) {
                    bool      found                          = false;
                    dd::Qubit physicalIndexOfLargerInSmaller = 0;
                    for (const auto& in: smallerInitial) {
                        if (in.second == outputQubitInLargerCircuit) {
                            found                          = true;
                            physicalIndexOfLargerInSmaller = in.first;
                            break;
                        }
                    }
                    if (found) {
                        // std::cout << "Found logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " in smaller circuit at physical qubit " << static_cast<std::size_t>(physicalIndexOfLargerInSmaller) << std::endl;
                        // std::cout << "This qubit is idle: " << circuit.isIdleQubit(physicalIndexOfLargerInSmaller) << std::endl;
                    }
                    if (!found || smallerCircuit.isIdleQubit(physicalIndexOfLargerInSmaller)) {
                        // std::cout << "Logical qubit " << static_cast<std::size_t>(outputQubitInSmallerCircuit) << " can be moved to logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << std::endl;
                        for (auto& in: smallerInitial) {
                            if (in.second == outputQubitInSmallerCircuit) {
                                in.second = outputQubitInLargerCircuit;
                                // std::cout << "Physical qubit " << static_cast<std::size_t>(in.first) << " has been assigned logical qubit " << static_cast<std::size_t>(in.second) << " as input" << std::endl;
                                break;
                            }
                        }
                        smallerOutput[physicalQubitInSmallerCircuit] = outputQubitInLargerCircuit;
                        // std::cout << "Physical qubit " << static_cast<std::size_t>(physicalQubitInSmallerCircuit) << " has been assigned logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " as output" << std::endl;
                        smallerAncillary[outputQubitInLargerCircuit]  = smallerAncillary[outputQubitInSmallerCircuit];
                        smallerAncillary[outputQubitInSmallerCircuit] = true;
                        // std::cout << "Logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " was assigned the ancillary status of qubit " << static_cast<std::size_t>(outputQubitInSmallerCircuit) << " (i.e., " << smallerAncillary[outputQubitInLargerCircuit] << ")" << std::endl;
                        smallerGarbage[outputQubitInLargerCircuit]  = false;
                        smallerGarbage[outputQubitInSmallerCircuit] = true;
                        // std::cout << "Logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " has been removed from the garbage outputs" << std::endl;
                    }
                } else {
                    std::cerr << "Uncorrected mismatch in output qubits!" << std::endl;
                }
            }
        }
    }

    void EquivalenceCheckingManager::runOptimizationPasses() {
        const auto isDynamicCircuit1 = qc::CircuitOptimizer::isDynamicCircuit(qc1);
        const auto isDynamicCircuit2 = qc::CircuitOptimizer::isDynamicCircuit(qc2);
        if (isDynamicCircuit1 || isDynamicCircuit2) {
            if (configuration.optimizations.transformDynamicCircuit) {
                if (isDynamicCircuit1) {
                    qc::CircuitOptimizer::eliminateResets(qc1);
                    qc::CircuitOptimizer::deferMeasurements(qc1);
                }
                if (isDynamicCircuit2) {
                    qc::CircuitOptimizer::eliminateResets(qc2);
                    qc::CircuitOptimizer::deferMeasurements(qc2);
                }
            } else {
                throw std::runtime_error("One of the circuits contains mid-circuit non-unitary primitives. "
                                         "Configure your instance with `transformDynamicCircuit=true`.");
            }
        }

        if (configuration.optimizations.removeDiagonalGatesBeforeMeasure) {
            qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc1);
            qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc2);
        }

        if (configuration.optimizations.reconstructSWAPs) {
            qc::CircuitOptimizer::swapReconstruction(qc1);
            qc::CircuitOptimizer::swapReconstruction(qc2);
        }

        if (configuration.optimizations.fuseSingleQubitGates) {
            qc::CircuitOptimizer::singleQubitGateFusion(qc1);
            qc::CircuitOptimizer::singleQubitGateFusion(qc2);
        }

        if (configuration.optimizations.reorderOperations) {
            qc::CircuitOptimizer::reorderOperations(qc1);
            qc::CircuitOptimizer::reorderOperations(qc2);
        }

        // remove final measurements from both circuits so that the underlying functionality should be unitary
        qc::CircuitOptimizer::removeFinalMeasurements(qc1);
        qc::CircuitOptimizer::removeFinalMeasurements(qc2);
    }

    EquivalenceCriterion EquivalenceCheckingManager::check() {
        const auto start = std::chrono::steady_clock::now();

        auto equivalence = EquivalenceCriterion::NoInformation;
        if (!configuration.execution.parallel || configuration.execution.nthreads <= 1) {
            equivalence = checkSequential();
        } else {
            equivalence = checkParallel();
        }
        const auto end = std::chrono::steady_clock::now();
        checkTime      = std::chrono::duration<double>(end - start).count();
        return equivalence;
    }

    EquivalenceCheckingManager::EquivalenceCheckingManager(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration):
        configuration(configuration) {
        const auto start = std::chrono::steady_clock::now();

        // clone both circuits (the circuit with fewer gates always gets to be qc1)
        this->qc1 = qc1.size() > qc2.size() ? qc2.clone() : qc1.clone();
        this->qc2 = qc1.size() > qc2.size() ? qc1.clone() : qc2.clone();

        // set numeric tolerance used throughout the check
        setTolerance(configuration.execution.numericalTolerance);

        // strip away qubits that are not acted upon
        this->qc1.stripIdleQubits();
        this->qc2.stripIdleQubits();

        // given that one circuit has more qubits than the other, the difference is assumed to arise from ancillary qubits.
        // adjust both circuits accordingly
        setupAncillariesAndGarbage();

        if (this->qc1.getNqubitsWithoutAncillae() != this->qc2.getNqubitsWithoutAncillae()) {
            std::clog << "[QCEC] Warning: circuits have different number of primary inputs! Proceed with caution!" << std::endl;
        }

        // try to fix a potential mismatch in the output permutations of both circuits
        if (configuration.optimizations.fixOutputPermutationMismatch) {
            fixOutputPermutationMismatch();
        }

        // run all configured optimization passes
        runOptimizationPasses();

        // initialize the stimuli generator
        stateGenerator = StateGenerator(configuration.simulation.seed);

        // check whether the number of selected stimuli does exceed the maximum number of unique computational basis states
        if (configuration.execution.runSimulationScheme && configuration.simulation.stateType == StateType::ComputationalBasis) {
            const auto        nq           = this->qc1.getNqubitsWithoutAncillae();
            const std::size_t uniqueStates = 1ULL << nq;
            if (configuration.simulation.maxSims > uniqueStates) {
                this->configuration.simulation.maxSims = uniqueStates;
                std::clog << "[QCEC] Info: Maximum number of simulations reduced due to number of qubits" << std::endl;
            }
        }

        const auto end    = std::chrono::steady_clock::now();
        preprocessingTime = std::chrono::duration<double>(end - start).count();
    }

    EquivalenceCriterion EquivalenceCheckingManager::checkSequential() {
        auto equivalence = EquivalenceCriterion::NoInformation;

        if (configuration.execution.runSimulationScheme) {
            DDSimulationChecker simulationChecker(qc1, qc2, configuration);
            while (performedSimulations < configuration.simulation.maxSims) {
                // configure simulation based checker
                simulationChecker.setRandomInitialState(stateGenerator);

                // run the simulation
                const auto result = simulationChecker.run();
                ++performedSimulations;

                // break if non-equivalence has been shown
                if (result == EquivalenceCriterion::NotEquivalent) {
                    equivalence = EquivalenceCriterion::NotEquivalent;
                    break;
                }

                // Otherwise, circuits are probably equivalent and execution can continue
                equivalence = EquivalenceCriterion::ProbablyEquivalent;
            }

            // Circuits have been shown to be non-equivalent
            if (equivalence == EquivalenceCriterion::NotEquivalent) {
                if (configuration.simulation.storeCEXinput) {
                    cexInput = simulationChecker.getInitialVector();
                }
                if (configuration.simulation.storeCEXoutput) {
                    cexOutput1 = simulationChecker.getInternalVector1();
                    cexOutput2 = simulationChecker.getInternalVector2();
                }
                return equivalence;
            }
        }

        if (configuration.execution.runAlternatingScheme) {
            DDAlternatingChecker alternatingChecker(qc1, qc2, configuration);
            const auto           result = alternatingChecker.run();

            // if the alternating check produces a result, this is final
            if (result != EquivalenceCriterion::NoInformation) {
                return result;
            }
        }

        if (configuration.execution.runConstructionScheme) {
            DDConstructionChecker constructionChecker(qc1, qc2, configuration);
            const auto            result = constructionChecker.run();

            // if the construction check produces a result, this is final
            if (result != EquivalenceCriterion::NoInformation) {
                return result;
            }
        }

        return equivalence;
    }

    EquivalenceCriterion EquivalenceCheckingManager::checkParallel() {
        /// TODO: orchestrate the parallel check

        return EquivalenceCriterion::NoInformation;
    }
} // namespace ec
