//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "CircuitOptimizer.hpp"
#include "Definitions.hpp"
#include "EquivalenceCriterion.hpp"
#include "checker/dd/DDAlternatingChecker.hpp"
#include "checker/dd/DDConstructionChecker.hpp"
#include "checker/dd/DDSimulationChecker.hpp"
#include "checker/dd/simulation/StateType.hpp"
#include "checker/zx/ZXChecker.hpp"
#include "zx/FunctionalityConstruction.hpp"

#include <cassert>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace ec {
void EquivalenceCheckingManager::setupAncillariesAndGarbage() {
  auto& largerCircuit =
      qc1.getNqubits() > qc2.getNqubits() ? this->qc1 : this->qc2;
  auto& smallerCircuit =
      qc1.getNqubits() > qc2.getNqubits() ? this->qc2 : this->qc1;
  const auto qubitDifference =
      largerCircuit.getNqubits() - smallerCircuit.getNqubits();

  std::vector<std::pair<qc::Qubit, std::optional<qc::Qubit>>> removed{};
  removed.reserve(qubitDifference);

  const auto        nqubits = largerCircuit.getNqubits();
  std::vector<bool> garbage(nqubits);

  for (std::size_t i = 0; i < qubitDifference; ++i) {
    const auto logicalQubitIndex =
        qc::QuantumComputation::getHighestLogicalQubitIndex(
            largerCircuit.initialLayout);
    garbage[logicalQubitIndex] =
        largerCircuit.logicalQubitIsGarbage(logicalQubitIndex);
    removed.push_back(largerCircuit.removeQubit(logicalQubitIndex));
  }

  // add appropriate ancillary register to smaller circuit
  smallerCircuit.addAncillaryRegister(qubitDifference);

  // reverse iterate over the removed qubits and add them back into the larger
  // circuit as ancillary
  const auto rend = removed.rend();
  for (auto it = removed.rbegin(); it != rend; ++it) {
    largerCircuit.addAncillaryQubit(it->first, it->second);
    // restore garbage
    if (garbage[largerCircuit.getNqubits() - 1]) {
      largerCircuit.setLogicalQubitGarbage(
          static_cast<qc::Qubit>(largerCircuit.getNqubits() - 1));
    }
    // also set the appropriate qubits in the smaller circuit as garbage
    smallerCircuit.setLogicalQubitGarbage(
        static_cast<qc::Qubit>(largerCircuit.getNqubits() - 1));
  }
}

void EquivalenceCheckingManager::fixOutputPermutationMismatch() {
  // Try to fix potential mismatches in output permutations
  auto& smallerCircuit = qc1.getNqubits() > qc2.getNqubits() ? qc2 : qc1;
  auto& largerCircuit  = qc1.getNqubits() > qc2.getNqubits() ? qc1 : qc2;

  auto& smallerGarbage = smallerCircuit.garbage;

  const auto& largerOutput  = largerCircuit.outputPermutation;
  auto&       largerGarbage = largerCircuit.garbage;

  for (const auto& [lPhysical, lLogical] : largerOutput) {
    const qc::Qubit outputQubitInLargerCircuit = lLogical;
    qc::Qubit       nout                       = 1;
    for (qc::Qubit i = 0; i < outputQubitInLargerCircuit; ++i) {
      if (!largerGarbage[i]) {
        ++nout;
      }
    }

    bool       existsInSmaller = false;
    const auto nqubits         = smallerCircuit.getNqubits();
    for (std::size_t i = 0U; i < nqubits; ++i) {
      if (!smallerGarbage[i]) {
        --nout;
      }
      if (nout == 0) {
        existsInSmaller = true;
        break;
      }
    }
    // algorithm has logical qubit that does not exist in the smaller circuit
    if (!existsInSmaller) {
      continue;
    }

    std::cerr << "Uncorrected mismatch in output qubits!\n";
  }
}

void EquivalenceCheckingManager::runOptimizationPasses() {
  if (qc1.empty() && qc2.empty()) {
    return;
  }

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
      throw std::runtime_error(
          "One of the circuits contains mid-circuit non-unitary primitives. "
          "To verify such circuits, the checker must be configured with "
          "`transformDynamicCircuit=true` (`transform_dynamic_circuits=True` "
          "in Python).");
    }
  }

  // first, make sure any potential SWAPs are reconstructed
  if (configuration.optimizations.reconstructSWAPs) {
    qc::CircuitOptimizer::swapReconstruction(qc1);
    qc::CircuitOptimizer::swapReconstruction(qc2);
  }

  // then, optionally backpropagate the output permutation
  if (configuration.optimizations.backpropagateOutputPermutation) {
    qc::CircuitOptimizer::backpropagateOutputPermutation(qc1);
    qc::CircuitOptimizer::backpropagateOutputPermutation(qc2);
  }

  // based on the above, all SWAPs should be reconstructed and accounted for,
  // so we can elide them.
  if (configuration.optimizations.elidePermutations) {
    qc::CircuitOptimizer::elidePermutations(qc1);
    qc::CircuitOptimizer::elidePermutations(qc2);
  }

  // fuse consecutive single qubit gates into compound operations (includes some
  // simple cancellation rules).
  if (configuration.optimizations.fuseSingleQubitGates) {
    qc::CircuitOptimizer::singleQubitGateFusion(qc1);
    qc::CircuitOptimizer::singleQubitGateFusion(qc2);
  }

  // optionally remove diagonal gates before measurements
  if (configuration.optimizations.removeDiagonalGatesBeforeMeasure) {
    qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc1);
    qc::CircuitOptimizer::removeDiagonalGatesBeforeMeasure(qc2);
  }

  if (configuration.optimizations.reorderOperations) {
    qc::CircuitOptimizer::reorderOperations(qc1);
    qc::CircuitOptimizer::reorderOperations(qc2);
  }

  // remove final measurements from both circuits so that the underlying
  // functionality should be unitary
  qc::CircuitOptimizer::removeFinalMeasurements(qc1);
  qc::CircuitOptimizer::removeFinalMeasurements(qc2);
}

void EquivalenceCheckingManager::run() {
  done = false;

  results.name1      = qc1.getName();
  results.name2      = qc2.getName();
  results.numQubits1 = qc1.getNqubits();
  results.numQubits2 = qc2.getNqubits();
  results.numGates1  = qc1.getNops();
  results.numGates2  = qc2.getNops();

  results.configuration = configuration;

  results.equivalence = EquivalenceCriterion::NoInformation;

  const bool garbageQubitsPresent =
      qc1.getNgarbageQubits() > 0 || qc2.getNgarbageQubits() > 0;

  if (!configuration.anythingToExecute()) {
    std::clog << "Nothing to be executed. Check your configuration!\n";
    return;
  }

  if (qc1.empty() && qc2.empty()) {
    results.equivalence = EquivalenceCriterion::Equivalent;
    done                = true;
    return;
  }

  if (qc1.isVariableFree() && qc2.isVariableFree()) {
    if (!configuration.execution.parallel ||
        configuration.execution.nthreads <= 1 ||
        configuration.onlySingleTask()) {
      checkSequential();
    } else {
      checkParallel();
    }
  } else {
    checkSymbolic();
  }

  for (const auto& checker : checkers) {
    nlohmann::json j{};
    checker->json(j);
    results.checkerResults.emplace_back(j);
  }

  if (!configuration.functionality.checkPartialEquivalence &&
      garbageQubitsPresent &&
      equivalence() == EquivalenceCriterion::NotEquivalent) {
    std::clog << "[QCEC] Warning: at least one of the circuits has garbage "
                 "qubits, but partial equivalence checking is turned off. In "
                 "order to take into account the garbage qubits, enable partial"
                 " equivalence checking. Consult the documentation for more"
                 "information.\n";
  }
}

EquivalenceCheckingManager::EquivalenceCheckingManager(
    // Circuits not passed by value and moved because this would cause slicing.
    // NOLINTNEXTLINE(modernize-pass-by-value)
    const qc::QuantumComputation& circ1, const qc::QuantumComputation& circ2,
    Configuration config)
    : qc1(circ1), qc2(circ2), configuration(std::move(config)) {
  const auto start = std::chrono::steady_clock::now();

  // set numeric tolerance used throughout the check
  setTolerance(configuration.execution.numericalTolerance);

  if (qc1.isVariableFree() && qc2.isVariableFree()) {
    // run all configured optimization passes
    runOptimizationPasses();
  }

  // strip away qubits that are not acted upon
  this->qc1.stripIdleQubits();
  this->qc2.stripIdleQubits();

  // given that one circuit has more qubits than the other, the difference is
  // assumed to arise from ancillary qubits. adjust both circuits accordingly
  setupAncillariesAndGarbage();

  if (this->qc1.getNqubitsWithoutAncillae() !=
      this->qc2.getNqubitsWithoutAncillae()) {
    std::clog << "[QCEC] Warning: circuits have different number of primary "
                 "inputs! Proceed with caution!\n";
  }

  // try to fix a potential mismatch in the output permutations of both circuits
  if (configuration.optimizations.fixOutputPermutationMismatch) {
    fixOutputPermutationMismatch();
  }

  // check whether the alternating checker is configured and can handle the
  // circuits
  if (configuration.execution.runAlternatingChecker &&
      !DDAlternatingChecker::canHandle(this->qc1, this->qc2)) {
    std::clog << "[QCEC] Warning: alternating checker cannot handle the "
                 "circuits. Falling back to construction checker.\n";
    this->configuration.execution.runAlternatingChecker  = false;
    this->configuration.execution.runConstructionChecker = true;
  }

  // initialize the stimuli generator
  stateGenerator = StateGenerator(configuration.simulation.seed);

  // check whether the number of selected stimuli does exceed the maximum
  // number of unique computational basis states
  if (configuration.execution.runSimulationChecker &&
      configuration.simulation.stateType == StateType::ComputationalBasis) {
    const auto        nq           = this->qc1.getNqubitsWithoutAncillae();
    const std::size_t uniqueStates = 1ULL << nq;
    if (nq <= 63U && configuration.simulation.maxSims > uniqueStates) {
      this->configuration.simulation.maxSims = uniqueStates;
    }
  }

  const auto end = std::chrono::steady_clock::now();
  results.preprocessingTime =
      std::chrono::duration<double>(end - start).count();
}

void EquivalenceCheckingManager::checkSequential() {
  const auto start = std::chrono::steady_clock::now();

  // in case a timeout is configured, a separate thread is started that sets
  // the `done` flag after the timeout has passed
  std::thread timeoutThread{};
  if (configuration.execution.timeout > 0.) {
    timeoutThread = std::thread([this, timeout = std::chrono::duration<double>(
                                           configuration.execution.timeout)] {
      std::unique_lock doneLock(doneMutex);
      const auto       finished =
          doneCond.wait_for(doneLock, timeout, [this] { return done; });
      // if the thread has already finished within the timeout, nothing
      // has to be done
      if (!finished) {
        setAndSignalDone();
      }
    });
  }

  if (configuration.execution.runSimulationChecker) {
    checkers.emplace_back(
        std::make_unique<DDSimulationChecker>(qc1, qc2, configuration));
    auto* const simulationChecker =
        dynamic_cast<DDSimulationChecker*>(checkers.back().get());
    while (!simulationsFinished() && !done) {
      // configure simulation based checker
      simulationChecker->setRandomInitialState(stateGenerator);

      // run the simulation
      ++results.startedSimulations;
      const auto result = simulationChecker->run();
      ++results.performedSimulations;

      // if the run completed but has not yielded any information this
      // indicates a timeout
      if (result == EquivalenceCriterion::NoInformation) {
        if (!done) {
          std::clog << "Simulation run returned without any information. "
                       "Something probably went wrong. Exiting!\n";
        }
        return;
      }

      // break if non-equivalence has been shown
      if (result == EquivalenceCriterion::NotEquivalent) {
        results.equivalence = EquivalenceCriterion::NotEquivalent;
        break;
      }

      // Otherwise, circuits are probably equivalent and execution can
      // continue
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

    // in case only simulations are performed and every single one is done,
    // everything is done
    if (configuration.onlySimulationCheckerConfigured() &&
        simulationsFinished()) {
      done = true;
      doneCond.notify_one();
    }
  }

  if (configuration.execution.runAlternatingChecker && !done) {
    checkers.emplace_back(
        std::make_unique<DDAlternatingChecker>(qc1, qc2, configuration));
    const auto& alternatingChecker = checkers.back();
    if (!done) {
      const auto result = alternatingChecker->run();

      // if the alternating check produces a result, this is final
      if (result != EquivalenceCriterion::NoInformation) {
        results.equivalence = result;

        // everything is done
        done = true;
        doneCond.notify_one();
      }
    }
  }

  if (configuration.execution.runConstructionChecker && !done) {
    checkers.emplace_back(
        std::make_unique<DDConstructionChecker>(qc1, qc2, configuration));
    const auto& constructionChecker = checkers.back();
    if (!done) {
      const auto result = constructionChecker->run();

      // if the construction check produces a result, this is final
      if (result != EquivalenceCriterion::NoInformation) {
        results.equivalence = result;

        // everything is done
        done = true;
        doneCond.notify_one();
      }
    }
  }

  if (configuration.execution.runZXChecker && !done) {
    if (zx::FunctionalityConstruction::transformableToZX(&qc1) &&
        zx::FunctionalityConstruction::transformableToZX(&qc2)) {
      checkers.emplace_back(
          std::make_unique<ZXEquivalenceChecker>(qc1, qc2, configuration));
      const auto& zxChecker = checkers.back();
      if (!done) {
        const auto result = zxChecker->run();

        // no matter the result, everything is done as this is the last check
        done = true;
        doneCond.notify_one();

        if (result == EquivalenceCriterion::Equivalent ||
            result == EquivalenceCriterion::EquivalentUpToGlobalPhase) {
          results.equivalence = result;
        } else if (result == EquivalenceCriterion::ProbablyNotEquivalent) {
          if (results.equivalence == EquivalenceCriterion::ProbablyEquivalent) {
            std::clog << "The ZX checker suggests that the circuits are not "
                         "equivalent, but the simulation checker suggests that "
                         "they are probably equivalent. Thus, no conclusion "
                         "can be drawn.\n";
            results.equivalence = EquivalenceCriterion::NoInformation;
          } else {
            results.equivalence = result;
          }
        } else {
          assert(result == EquivalenceCriterion::NoInformation);
          if (results.equivalence == EquivalenceCriterion::NoInformation) {
            // this can only happen if the ZX checker is the only checker
            assert(configuration.onlyZXCheckerConfigured());
            std::clog
                << "Only ZX checker specified, but it was not able to conclude "
                   "anything about the equivalence of the circuits!\n"
                << "This can happen since the ZX checker is not complete in "
                   "general.\n"
                << "Consider enabling other checkers to get more "
                   "information.\n";
          }
        }
      }
    } else if (configuration.onlyZXCheckerConfigured()) {
      std::clog
          << "Only ZX checker specified, but one of the circuits contains "
             "operations not supported by this checker! Exiting!\n";
      checkers.clear();
      results.equivalence = EquivalenceCriterion::NoInformation;
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

  std::chrono::time_point<std::chrono::steady_clock,
                          std::chrono::duration<double>>
      deadline{};
  if (configuration.execution.timeout > 0.) {
    deadline =
        start + std::chrono::duration<double>(configuration.execution.timeout);
  }

  if (const auto threadLimit = std::thread::hardware_concurrency();
      threadLimit != 0U && configuration.execution.nthreads > threadLimit) {
    std::clog
        << "Trying to use more threads than the underlying architecture claims "
           "to support. Over-subscription might impact performance!\n";
  }
  const auto maxThreads = configuration.execution.nthreads;

  std::size_t tasksToExecute = 0U;
  if (configuration.execution.runAlternatingChecker) {
    ++tasksToExecute;
  }
  if (configuration.execution.runConstructionChecker) {
    ++tasksToExecute;
  }
  if (configuration.execution.runSimulationChecker) {
    if (configuration.simulation.maxSims > 0U) {
      tasksToExecute += configuration.simulation.maxSims;
    } else {
      configuration.execution.runSimulationChecker = false;
    }
  }
  if (configuration.execution.runZXChecker) {
    if (zx::FunctionalityConstruction::transformableToZX(&qc1) &&
        zx::FunctionalityConstruction::transformableToZX(&qc2)) {
      ++tasksToExecute;
    } else if (configuration.onlyZXCheckerConfigured()) {
      std::clog
          << "Only ZX checker specified, but one of the circuits contains "
             "operations not supported by this checker! Exiting!\n";
      setAndSignalDone();
      results.equivalence = EquivalenceCriterion::NoInformation;
    } else {
      configuration.execution.runZXChecker = false;
    }
  }

  const auto effectiveThreads = std::min(maxThreads, tasksToExecute);

  // reserve space for as many equivalence checkers as there will be
  // parallel threads
  checkers.resize(effectiveThreads);

  // create a thread safe queue which is used to check for available results
  ThreadSafeQueue<std::size_t> queue{};
  std::size_t                  id = 0U;

  // reserve space for the futures received from the async calls
  std::vector<std::future<void>> futures{};
  futures.reserve(effectiveThreads);

  if (configuration.execution.runAlternatingChecker) {
    // start a new thread that constructs and runs the alternating check
    futures.emplace_back(asyncRunChecker<DDAlternatingChecker>(id, queue));
    ++id;
  }

  if (configuration.execution.runConstructionChecker && !done) {
    // start a new thread that constructs and runs the construction check
    futures.emplace_back(asyncRunChecker<DDConstructionChecker>(id, queue));
    ++id;
  }

  if (configuration.execution.runZXChecker && !done) {
    // start a new thread that constructs and runs the ZX checker
    futures.emplace_back(asyncRunChecker<ZXEquivalenceChecker>(id, queue));
    ++id;
  }

  if (configuration.execution.runSimulationChecker) {
    const auto effectiveThreadsLeft = effectiveThreads - futures.size();
    const auto simulationsToStart =
        std::min(effectiveThreadsLeft, configuration.simulation.maxSims);
    // launch as many simulations as possible
    for (std::size_t i = 0; i < simulationsToStart && !done; ++i) {
      futures.emplace_back(asyncRunChecker<DDSimulationChecker>(id, queue));
      ++id;
      ++results.startedSimulations;
    }
  }

  // wait in a loop while no definitive result has been obtained
  while (!done) {
    std::shared_ptr<std::size_t> completedID{};
    if (configuration.execution.timeout > 0.) {
      completedID = queue.waitAndPopUntil(deadline);
    } else {
      completedID = queue.waitAndPop();
    }

    // in case no completed ID has been returned this indicates a timeout
    // and the computation should stop
    if (!completedID) {
      setAndSignalDone();
      break;
    }

    // otherwise, a checker has finished its execution
    // get the result of the future (which should be ready)
    // this makes sure exceptions are thrown if necessary
    futures.at(*completedID).get();

    // in case non-equivalence has been shown, the execution can be stopped
    const auto* const checker = checkers.at(*completedID).get();
    const auto        result  = checker->getEquivalence();

    if (result == EquivalenceCriterion::NoInformation) {
      if (dynamic_cast<const ZXEquivalenceChecker*>(checker) != nullptr) {
        if (configuration.onlyZXCheckerConfigured()) {
          std::clog
              << "Only ZX checker specified, but it was not able to conclude "
                 "anything about the equivalence of the circuits!\n"
              << "This can happen since the ZX checker is not complete in "
                 "general.\n"
              << "Consider enabling other checkers to get more "
                 "information.\n";
          setAndSignalDone();
          break;
        }
        continue;
      }
      std::clog << "Finished equivalence check provides no information. "
                   "Something probably went wrong. Exiting.\n";
      setAndSignalDone();
      results.equivalence = result;
      break;
    }

    if (result == EquivalenceCriterion::NotEquivalent) {
      setAndSignalDone();
      results.equivalence = result;

      // some special handling in case non-equivalence has been shown by a
      // simulation run
      if (const auto* const simulationChecker =
              dynamic_cast<const DDSimulationChecker*>(checker)) {
        ++results.performedSimulations;

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

    // the alternating and the construction checker provide definitive answers
    // once they finish
    if ((dynamic_cast<const DDAlternatingChecker*>(checker) != nullptr) ||
        (dynamic_cast<const DDConstructionChecker*>(checker) != nullptr)) {
      setAndSignalDone();
      results.equivalence = result;
      break;
    }

    if (dynamic_cast<const ZXEquivalenceChecker*>(checker) != nullptr) {
      if (result == EquivalenceCriterion::Equivalent ||
          result == EquivalenceCriterion::EquivalentUpToGlobalPhase) {
        setAndSignalDone();
        results.equivalence = result;
        break;
      }

      if (result == EquivalenceCriterion::ProbablyNotEquivalent) {
        if (results.equivalence == EquivalenceCriterion::ProbablyEquivalent) {
          if (simulationsFinished()) {
            std::clog << "The ZX checker suggests that the circuits are not "
                         "equivalent, but the simulation checker suggests that "
                         "they are probably equivalent. Thus, no conclusion "
                         "can be drawn.\n";
            setAndSignalDone();
            results.equivalence = EquivalenceCriterion::NoInformation;
            break;
          }
          results.equivalence = result;
          continue;
        }

        // update result if no information is known
        if (results.equivalence == EquivalenceCriterion::NoInformation) {
          results.equivalence = result;
          if (configuration.onlyZXCheckerConfigured()) {
            setAndSignalDone();
            break;
          }
          // Since the ZX checker is not complete, it cannot conclude
          // non-equivalence, but only suggest it. If the ZX checker is not the
          // only checker configured to run, the run continues uninterrupted.
          continue;
        }
      }
    }

    // at this point, the only option is that this is a simulation checker
    if (dynamic_cast<const DDSimulationChecker*>(checker) != nullptr) {
      ++results.performedSimulations;

      // if no information is known, the successful simulation suggests that
      // both circuits are likely to be equivalent.
      if (results.equivalence == EquivalenceCriterion::NoInformation) {
        results.equivalence = EquivalenceCriterion::ProbablyEquivalent;
      }

      if (simulationsFinished()) {
        if (configuration.onlySimulationCheckerConfigured()) {
          // if only simulations are performed and all of them are successful,
          // the circuits are most likely equivalent, and the procedure is done.
          setAndSignalDone();
          break;
        }

        if (results.equivalence ==
            EquivalenceCriterion::ProbablyNotEquivalent) {
          std::clog
              << "The ZX checker suggests that the circuits are not "
                 "equivalent, but the simulation checker suggests that they "
                 "are probably equivalent. Thus, no conclusion can be drawn.\n";
          setAndSignalDone();
          results.equivalence = EquivalenceCriterion::NoInformation;
          break;
        }
        // if all simulations finished and none of them showed non-equivalence,
        // the run continues uninterrupted.
        continue;
      }

      // it has to be checked, whether further simulations shall be
      // conducted
      if (results.startedSimulations < configuration.simulation.maxSims) {
        futures[*completedID] =
            asyncRunChecker<DDSimulationChecker>(*completedID, queue);
        ++results.startedSimulations;
      }
    }
  }

  const auto end    = std::chrono::steady_clock::now();
  results.checkTime = std::chrono::duration<double>(end - start).count();

  // Futures are not explicitly waited for here, since the destructor of the
  // `std::future` object will block until the associated thread has finished.
  // If any thread is still stuck in a long-running operation, this might take a
  // while, but the program will terminate anyway. C++20 introduces
  // `std::jthread`, which allows to explicitly cancel a thread. This could be a
  // solution for the future to avoid this problem (and reduce the number of
  // `isDone` checks).
}

void EquivalenceCheckingManager::checkSymbolic() {
  const auto start = std::chrono::steady_clock::now();
  // in case a timeout is configured, a separate thread is started that
  // sets the `done` flag after the timeout has passed
  std::thread timeoutThread{};
  if (configuration.execution.timeout > 0.) {
    timeoutThread = std::thread([this, timeout = std::chrono::duration<double>(
                                           configuration.execution.timeout)] {
      std::unique_lock doneLock(doneMutex);
      auto             finished =
          doneCond.wait_for(doneLock, timeout, [this] { return done; });
      // if the thread has already finished within the timeout,
      // nothing has to be done
      if (!finished) {
        setAndSignalDone();
      }
    });
  }

  if (!done) {
    if (zx::FunctionalityConstruction::transformableToZX(&qc1) &&
        zx::FunctionalityConstruction::transformableToZX(&qc2)) {
      checkers.emplace_back(
          std::make_unique<ZXEquivalenceChecker>(qc1, qc2, configuration));
      const auto& zxChecker = checkers.back();
      if (!done) {
        const auto result   = zxChecker->run();
        results.equivalence = result;
        done                = true;
        doneCond.notify_one();
      }
    } else {
      std::clog << "Checking symbolic circuits requires transformation "
                   "to ZX-diagram but one of the circuits contains "
                   "operations not supported by this checker! Exiting!"
                << std::endl;
      checkers.clear();
      results.equivalence = EquivalenceCriterion::NoInformation;
      return;
    }
  }
  const auto end    = std::chrono::steady_clock::now();
  results.checkTime = std::chrono::duration<double>(end - start).count();
  // appropriately join the timeout thread, if it was launched
  if (timeoutThread.joinable()) {
    timeoutThread.join();
  }
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

void EquivalenceCheckingManager::backpropagateOutputPermutation() {
  if (!configuration.optimizations.backpropagateOutputPermutation) {
    qc::CircuitOptimizer::backpropagateOutputPermutation(qc1);
    qc::CircuitOptimizer::backpropagateOutputPermutation(qc2);
    configuration.optimizations.backpropagateOutputPermutation = true;
  }
}

void EquivalenceCheckingManager::elidePermutations() {
  if (!configuration.optimizations.elidePermutations) {
    qc::CircuitOptimizer::elidePermutations(qc1);
    qc::CircuitOptimizer::elidePermutations(qc2);
    configuration.optimizations.elidePermutations = true;
  }
}

nlohmann::json EquivalenceCheckingManager::Results::json() const {
  nlohmann::json res{};

  auto& circuit1         = res["circuit1"];
  circuit1["name"]       = name1;
  circuit1["num_qubits"] = numQubits1;
  circuit1["num_gates"]  = numGates1;

  auto& circuit2         = res["circuit2"];
  circuit2["name"]       = name2;
  circuit2["num_qubits"] = numQubits2;
  circuit2["num_gates"]  = numGates2;

  res["configuration"] = configuration.json();

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
  auto& par                       = res["parameterized"];
  par["performed_instantiations"] = performedInstantiations;

  res["checkers"] = checkerResults;

  return res;
}
} // namespace ec
