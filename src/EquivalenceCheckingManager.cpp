/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
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

        [[maybe_unused]] auto& smallerInitial   = smallerCircuit.initialLayout;
        auto&                  smallerOutput    = smallerCircuit.outputPermutation;
        [[maybe_unused]] auto& smallerAncillary = smallerCircuit.ancillary;
        auto&                  smallerGarbage   = smallerCircuit.garbage;

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
            [[maybe_unused]] dd::Qubit physicalQubitInSmallerCircuit = 0;
            for (const auto& out: smallerOutput) {
                if (out.second == outputQubitInSmallerCircuit) {
                    physicalQubitInSmallerCircuit = out.first;
                    break;
                }
            }
            // std::cout << " which is assigned to physical qubit " << static_cast<std::size_t>(physicalQubitInSmallerCircuit) << " at the end of the circuit" << std::endl;

            //            if (outputQubitInLargerCircuit != outputQubitInSmallerCircuit) {
            //                std::cout << "Mismatch in the logical output qubits" << std::endl;
            //                if (smallerAncillary[outputQubitInLargerCircuit] && smallerGarbage[outputQubitInLargerCircuit]) {
            //                    bool      found                          = false;
            //                    dd::Qubit physicalIndexOfLargerInSmaller = 0;
            //                    for (const auto& in: smallerInitial) {
            //                        if (in.second == outputQubitInLargerCircuit) {
            //                            found                          = true;
            //                            physicalIndexOfLargerInSmaller = in.first;
            //                            break;
            //                        }
            //                    }
            // if (found) {
            //  std::cout << "Found logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " in smaller circuit at physical qubit " << static_cast<std::size_t>(physicalIndexOfLargerInSmaller) << std::endl;
            //  std::cout << "This qubit is idle: " << circuit.isIdleQubit(physicalIndexOfLargerInSmaller) << std::endl;
            // }
            //                    if (!found || smallerCircuit.isIdleQubit(physicalIndexOfLargerInSmaller)) {
            // std::cout << "Logical qubit " << static_cast<std::size_t>(outputQubitInSmallerCircuit) << " can be moved to logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << std::endl;
            //                        for (auto& in: smallerInitial) {
            //                            if (in.second == outputQubitInSmallerCircuit) {
            //                                in.second = outputQubitInLargerCircuit;
            // std::cout << "Physical qubit " << static_cast<std::size_t>(in.first) << " has been assigned logical qubit " << static_cast<std::size_t>(in.second) << " as input" << std::endl;
            //                                break;
            //                            }
            //                        }
            //                        smallerOutput[physicalQubitInSmallerCircuit] = outputQubitInLargerCircuit;
            // std::cout << "Physical qubit " << static_cast<std::size_t>(physicalQubitInSmallerCircuit) << " has been assigned logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " as output" << std::endl;
            //                        smallerAncillary[outputQubitInLargerCircuit]  = smallerAncillary[outputQubitInSmallerCircuit];
            //                        smallerAncillary[outputQubitInSmallerCircuit] = true;
            // std::cout << "Logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " was assigned the ancillary status of qubit " << static_cast<std::size_t>(outputQubitInSmallerCircuit) << " (i.e., " << smallerAncillary[outputQubitInLargerCircuit] << ")" << std::endl;
            //                        smallerGarbage[outputQubitInLargerCircuit]  = false;
            //                        smallerGarbage[outputQubitInSmallerCircuit] = true;
            // std::cout << "Logical qubit " << static_cast<std::size_t>(outputQubitInLargerCircuit) << " has been removed from the garbage outputs" << std::endl;
            //                    }
            //                } else {
            std::cerr << "Uncorrected mismatch in output qubits!" << std::endl;
            //                }
            //        }
        }
    }

    void EquivalenceCheckingManager::runOptimizationPasses() {
        if (qc1.empty() && qc2.empty())
            return;

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

    void EquivalenceCheckingManager::run() {
        done                = false;
        results.equivalence = EquivalenceCriterion::NoInformation;

        if (!configuration.anythingToExecute()) {
            std::clog << "Nothing to be executed. Check your configuration!" << std::endl;
            return;
        }

        if (!configuration.execution.parallel || configuration.execution.nthreads <= 1 || configuration.onlySingleTask()) {
            checkSequential();
        } else {
            checkParallel();
        }
    }

    EquivalenceCheckingManager::EquivalenceCheckingManager(const qc::QuantumComputation& qc1, const qc::QuantumComputation& qc2, const Configuration& configuration):
        configuration(configuration) {
        const auto start = std::chrono::steady_clock::now();

        // clone both circuits (the circuit with fewer gates always gets to be qc1)
        this->qc1 = qc1.size() > qc2.size() ? qc2.clone() : qc1.clone();
        this->qc2 = qc1.size() > qc2.size() ? qc1.clone() : qc2.clone();

        // set numeric tolerance used throughout the check
        setTolerance(configuration.execution.numericalTolerance);

        // run all configured optimization passes
        runOptimizationPasses();

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

        // initialize the stimuli generator
        stateGenerator = StateGenerator(configuration.simulation.seed);

        // check whether the number of selected stimuli does exceed the maximum number of unique computational basis states
        if (configuration.execution.runSimulationChecker && configuration.simulation.stateType == StateType::ComputationalBasis) {
            const auto        nq           = this->qc1.getNqubitsWithoutAncillae();
            const std::size_t uniqueStates = 1ULL << nq;
            if (nq <= 63U && configuration.simulation.maxSims > uniqueStates) {
                this->configuration.simulation.maxSims = uniqueStates;
            }
        }

        const auto end            = std::chrono::steady_clock::now();
        results.preprocessingTime = std::chrono::duration<double>(end - start).count();
    }

    void EquivalenceCheckingManager::checkSequential() {
        const auto start = std::chrono::steady_clock::now();

        // in case a timeout is configured, a separate thread is started that sets the `done` flag after the timeout has passed
        std::thread timeoutThread{};
        if (configuration.execution.timeout > 0s) {
            timeoutThread = std::thread([&, timeout = configuration.execution.timeout] {
                std::unique_lock doneLock(doneMutex);
                auto             finished = doneCond.wait_for(doneLock, timeout, [&] { return done; });
                // if the thread has already finished within the timeout, nothing has to be done
                if (!finished) {
                    done = true;
                }
            });
        }

        if (configuration.execution.runSimulationChecker) {
            checkers.emplace_back(std::make_unique<DDSimulationChecker>(qc1, qc2, configuration));
            auto* simulationChecker = dynamic_cast<DDSimulationChecker*>(checkers.back().get());
            while (results.startedSimulations < configuration.simulation.maxSims && !done) {
                // configure simulation based checker
                simulationChecker->setRandomInitialState(stateGenerator);

                // run the simulation
                ++results.startedSimulations;
                const auto result = simulationChecker->run();
                ++results.performedSimulations;

                // if the run completed but has not yielded any information this indicates a timeout
                if (result == EquivalenceCriterion::NoInformation) {
                    if (!done) {
                        std::clog << "Simulation run returned without any information. Something probably went wrong. Exiting!" << std::endl;
                    }
                    return;
                }

                // break if non-equivalence has been shown
                if (result == EquivalenceCriterion::NotEquivalent) {
                    results.equivalence = EquivalenceCriterion::NotEquivalent;
                    break;
                }

                // Otherwise, circuits are probably equivalent and execution can continue
                results.equivalence = EquivalenceCriterion::ProbablyEquivalent;
            }

            // Circuits have been shown to be non-equivalent
            if (results.equivalence == EquivalenceCriterion::NotEquivalent) {
                if (configuration.simulation.storeCEXinput) {
                    results.cexInput = simulationChecker->getInitialVector();
                }
                if (configuration.simulation.storeCEXoutput) {
                    results.cexOutput1 = simulationChecker->getInternalVector1();
                    results.cexOutput2 = simulationChecker->getInternalVector2();
                }

                // everything is done
                done = true;
                doneCond.notify_one();
            }

            // in case only simulations are performed and every single one is done, everything is done
            if (!configuration.execution.runAlternatingChecker &&
                !configuration.execution.runConstructionChecker &&
                results.performedSimulations == configuration.simulation.maxSims) {
                done = true;
                doneCond.notify_one();
            }
        }

        if (configuration.execution.runAlternatingChecker && !done) {
            checkers.emplace_back(std::make_unique<DDAlternatingChecker>(qc1, qc2, configuration));
            auto&      alternatingChecker = checkers.back();
            const auto result             = alternatingChecker->run();

            // if the alternating check produces a result, this is final
            if (result != EquivalenceCriterion::NoInformation) {
                results.equivalence = result;

                // everything is done
                done = true;
                doneCond.notify_one();
            }
        }

        if (configuration.execution.runConstructionChecker && !done) {
            checkers.emplace_back(std::make_unique<DDConstructionChecker>(qc1, qc2, configuration));
            auto&      constructionChecker = checkers.back();
            const auto result              = constructionChecker->run();

            // if the construction check produces a result, this is final
            if (result != EquivalenceCriterion::NoInformation) {
                results.equivalence = result;

                // everything is done
                done = true;
                doneCond.notify_one();
            }
        }

        const auto end    = std::chrono::steady_clock::now();
        results.checkTime = std::chrono::duration<double>(end - start).count();

        // appropriately join the timeout thread, if it was launched
        if (timeoutThread.joinable()) {
            timeoutThread.join();
        }
    }

    void EquivalenceCheckingManager::checkParallel() {
        const auto start = std::chrono::steady_clock::now();

        std::chrono::steady_clock::time_point deadline{};
        if (configuration.execution.timeout > 0s) {
            deadline = start + configuration.execution.timeout;
        }

        const auto threadLimit = std::thread::hardware_concurrency();
        if (threadLimit != 0 && configuration.execution.nthreads > threadLimit) {
            std::clog << "Trying to use more threads than the underlying architecture claims to support. Over-subscription might impact performance!" << std::endl;
        }

        const auto maxThreads      = configuration.execution.nthreads;
        const auto runAlternating  = configuration.execution.runAlternatingChecker;
        const auto runConstruction = configuration.execution.runConstructionChecker;
        const auto runSimulation   = configuration.execution.runSimulationChecker && configuration.simulation.maxSims > 0;

        const std::size_t tasksToExecute = configuration.simulation.maxSims +
                                           (runAlternating ? 1U : 0U) +
                                           (runConstruction ? 1U : 0U);

        const auto effectiveThreads = std::min(maxThreads, tasksToExecute);

        // reserve space for as many equivalence checkers as there will be parallel threads
        checkers.resize(effectiveThreads);

        // create a thread safe queue which is used to check for available results
        ThreadSafeQueue<std::size_t> queue{};
        std::size_t                  id = 0U;

        // reserve space for the threads
        std::vector<std::thread> threads{};
        threads.reserve(effectiveThreads);

        if (runAlternating) {
            // start a new thread that constructs and runs the alternating check
            threads.emplace_back([&, id] {
                checkers[id] = std::make_unique<DDAlternatingChecker>(qc1, qc2, configuration);
                checkers[id]->run();
                queue.push(id);
            });
            ++id;
        }

        if (runConstruction && !done) {
            // start a new thread that constructs and runs the construction check
            threads.emplace_back([&, id] {
                checkers[id] = std::make_unique<DDConstructionChecker>(qc1, qc2, configuration);
                if (!done)
                    checkers[id]->run();
                queue.push(id);
            });
            ++id;
        }

        if (runSimulation) {
            const auto effectiveThreadsLeft = effectiveThreads - threads.size();
            // launch as many simulations as possible
            for (std::size_t i = 0; i < effectiveThreadsLeft && !done; ++i) {
                threads.emplace_back([&, id] {
                    checkers[id]  = std::make_unique<DDSimulationChecker>(qc1, qc2, configuration);
                    auto* checker = dynamic_cast<DDSimulationChecker*>(checkers[id].get());
                    {
                        std::lock_guard stateGeneratorLock(stateGeneratorMutex);
                        checker->setRandomInitialState(stateGenerator);
                    }
                    if (!done)
                        checkers[id]->run();
                    queue.push(id);
                });
                ++id;
                ++results.startedSimulations;
            }
        }

        // wait in a loop while no definitive result has been obtained
        while (!done) {
            std::shared_ptr<std::size_t> completedID{};
            if (configuration.execution.timeout > 0s) {
                completedID = queue.waitAndPopUntil(deadline);
            } else {
                completedID = queue.waitAndPop();
            }

            // in case no completed ID has been returned this indicates a timeout and the computation should stop
            if (!completedID) {
                setAndSignalDone();
                break;
            }

            // otherwise, a checker has finished its execution
            // join the respective thread (which should return immediately)
            threads.at(*completedID).join();

            // in case non-equivalence has been shown, the execution can be stopped
            auto*      checker = checkers.at(*completedID).get();
            const auto result  = checker->getEquivalence();
            if (result == EquivalenceCriterion::NoInformation) {
                std::clog << "Finished equivalence check provides no information. Something probably went wrong. Exiting." << std::endl;
                break;
            }

            if (result == EquivalenceCriterion::NotEquivalent) {
                setAndSignalDone();
                results.equivalence = result;

                // some special handling in case non-equivalence has been shown by a simulation run
                if (auto simulationChecker = dynamic_cast<DDSimulationChecker*>(checker)) {
                    results.performedSimulations++;

                    if (configuration.simulation.storeCEXinput) {
                        results.cexInput = simulationChecker->getInitialVector();
                    }
                    if (configuration.simulation.storeCEXoutput) {
                        results.cexOutput1 = simulationChecker->getInternalVector1();
                        results.cexOutput2 = simulationChecker->getInternalVector2();
                    }
                }

                break;
            }

            // the alternating and the construction checker provide definitive answers once they finish
            if (dynamic_cast<DDAlternatingChecker*>(checker) || dynamic_cast<DDConstructionChecker*>(checker)) {
                setAndSignalDone();
                results.equivalence = result;
                break;
            }

            // at this point, the only option is that this is a simulation checker
            if (dynamic_cast<DDSimulationChecker*>(checker)) {
                // if the simulation has not shown the non-equivalence, then both circuits are considered probably equivalent
                results.equivalence = EquivalenceCriterion::ProbablyEquivalent;
                ++results.performedSimulations;

                // it has to be checked, whether further simulations shall be conducted
                if (results.startedSimulations < configuration.simulation.maxSims) {
                    threads[*completedID] = std::thread([&, id = *completedID] {
                        {
                            auto*           checker = dynamic_cast<DDSimulationChecker*>(checkers[id].get());
                            std::lock_guard stateGeneratorLock(stateGeneratorMutex);
                            checker->setRandomInitialState(stateGenerator);
                        }
                        if (!done)
                            checkers[id]->run();
                        queue.push(id);
                    });
                    ++results.startedSimulations;
                } else {
                    // in case only simulations are performed and every single one is done, everything is done
                    if (!runAlternating && !runConstruction && results.performedSimulations == configuration.simulation.maxSims) {
                        setAndSignalDone();
                        break;
                    }
                }
            }
        }

        const auto end    = std::chrono::steady_clock::now();
        results.checkTime = std::chrono::duration<double>(end - start).count();

        // cleanup threads that are still running by joining them
        // start by joining all the completed threads, which should succeed instantly
        while (!queue.empty()) {
            const auto completedID = queue.waitAndPop();
            auto&      thread      = threads.at(*completedID);
            if (thread.joinable()) {
                thread.join();
            }
        }

        // afterwards, join all threads that are still (potentially) running.
        // at the moment there seems to be no solution for prematurely killing running threads without risking synchronisation issues.
        // on the positive side, joining should avoid all of these potential issues
        // on the negative side, one of the threads might still be stuck in a long-running operation and does not check for the `done` signal until this operation completes
        for (auto& thread: threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    nlohmann::json EquivalenceCheckingManager::json() const {
        nlohmann::json res{};

        addCircuitDescription(qc1, res["circuit1"]);
        addCircuitDescription(qc2, res["circuit2"]);
        res["configuration"] = configuration.json();
        res["results"]       = results.json();

        if (!checkers.empty()) {
            res["checkers"]      = nlohmann::json::array();
            auto& checkerResults = res["checkers"];
            for (auto& checker: checkers) {
                nlohmann::json j{};
                checker->json(j);
                checkerResults.push_back(j);
            }
        }
        return res;
    }
    void EquivalenceCheckingManager::runFixOutputPermutationMismatch() {
        if (!configuration.optimizations.fixOutputPermutationMismatch) {
            fixOutputPermutationMismatch();
            configuration.optimizations.fixOutputPermutationMismatch = true;
        }
    }
    void EquivalenceCheckingManager::fuseSingleQubitGates() {
        if (!configuration.optimizations.fuseSingleQubitGates) {
            qc::CircuitOptimizer::singleQubitGateFusion(qc1);
            qc::CircuitOptimizer::singleQubitGateFusion(qc2);
            configuration.optimizations.fuseSingleQubitGates = true;
        }
    }
    void EquivalenceCheckingManager::reconstructSWAPs() {
        if (!configuration.optimizations.reconstructSWAPs) {
            qc::CircuitOptimizer::swapReconstruction(qc1);
            qc::CircuitOptimizer::swapReconstruction(qc2);
            configuration.optimizations.reconstructSWAPs = true;
        }
    }
    void EquivalenceCheckingManager::reorderOperations() {
        if (!configuration.optimizations.reorderOperations) {
            qc::CircuitOptimizer::reorderOperations(qc1);
            qc::CircuitOptimizer::reorderOperations(qc2);
            configuration.optimizations.reorderOperations = true;
        }
    }
    nlohmann::json EquivalenceCheckingManager::Results::json() const {
        nlohmann::json res{};
        res["preprocessing_time"] = preprocessingTime;
        res["check_time"]         = checkTime;
        res["equivalence"]        = ec::toString(equivalence);

        if (startedSimulations > 0) {
            auto& sim        = res["simulations"];
            sim["started"]   = startedSimulations;
            sim["performed"] = performedSimulations;

            if (!cexInput.empty() || !cexOutput1.empty() || !cexOutput2.empty()) {
                auto& cex = sim["verification_cex"];
                if (!cexInput.empty()) {
                    toJson(cex["input"], cexInput);
                }
                if (!cexOutput1.empty()) {
                    toJson(cex["output1"], cexOutput1);
                }
                if (!cexOutput2.empty()) {
                    toJson(cex["output2"], cexOutput2);
                }
            }
        }

        return res;
    }
} // namespace ec
