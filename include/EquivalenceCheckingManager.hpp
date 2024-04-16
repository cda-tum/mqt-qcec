//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#pragma once

#include "Configuration.hpp"
#include "EquivalenceCriterion.hpp"
#include "QuantumComputation.hpp"
#include "ThreadSafeQueue.hpp"
#include "checker/dd/DDSimulationChecker.hpp"
#include "checker/dd/simulation/StateGenerator.hpp"
#include "dd/ComplexNumbers.hpp"
#include "dd/DDDefinitions.hpp"

#include <condition_variable>
#include <cstddef>
#include <exception>
#include <future>
#include <memory>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ec {

class EquivalenceCheckingManager {
public:
  struct Results {
    std::string name1;
    std::string name2;

    std::size_t numQubits1{};
    std::size_t numQubits2{};

    std::size_t numGates1{};
    std::size_t numGates2{};

    Configuration configuration{};

    double preprocessingTime{};
    double checkTime{};

    EquivalenceCriterion equivalence = EquivalenceCriterion::NoInformation;

    std::size_t startedSimulations   = 0U;
    std::size_t performedSimulations = 0U;
    dd::CVec    cexInput;
    dd::CVec    cexOutput1;
    dd::CVec    cexOutput2;
    std::size_t performedInstantiations = 0U;

    nlohmann::json checkerResults = nlohmann::json::array();

    [[nodiscard]] bool consideredEquivalent() const {
      switch (equivalence) {
      case EquivalenceCriterion::Equivalent:
      case EquivalenceCriterion::ProbablyEquivalent:
      case EquivalenceCriterion::EquivalentUpToGlobalPhase:
      case EquivalenceCriterion::EquivalentUpToPhase:
        return true;
      default:
        return false;
      }
    }

    [[nodiscard]] nlohmann::json json() const;

    static void toJson(nlohmann::json& j, const dd::CVec& stateVector) {
      j = nlohmann::json::array();
      for (const auto& amp : stateVector) {
        j.emplace_back(std::pair{amp.real(), amp.imag()});
      }
    }
    [[nodiscard]] std::string toString() const { return json().dump(2); }
    friend std::ostream&
    operator<<(std::ostream&                              os,
               const EquivalenceCheckingManager::Results& res) {
      return os << res.toString();
    }
  };

  EquivalenceCheckingManager(const qc::QuantumComputation& circ1,
                             const qc::QuantumComputation& circ2,
                             Configuration configuration = Configuration{});

  void run();

  void reset() {
    stateGenerator.clear();
    results = Results{};
    checkers.clear();
  }

  [[nodiscard]] EquivalenceCriterion equivalence() const {
    return results.equivalence;
  }
  [[nodiscard]] Configuration getConfiguration() const { return configuration; }
  [[nodiscard]] Results       getResults() const { return results; }

  // convenience functions for changing the configuration after the manager has
  // been constructed: Execution: These settings may be changed to influence
  // what is executed during `run`
  void setTolerance(dd::fp tol) {
    configuration.execution.numericalTolerance = tol;
    dd::ComplexNumbers::setTolerance(tol);
  }
  void setParallel(bool parallel) {
    configuration.execution.parallel = parallel;
  }
  void setNThreads(std::size_t nthreads) {
    configuration.execution.nthreads = nthreads;
  }
  void setTimeout(const double timeout) {
    configuration.execution.timeout = timeout;
  }
  void setConstructionChecker(bool run) {
    configuration.execution.runConstructionChecker = run;
  }
  void setSimulationChecker(bool run) {
    configuration.execution.runSimulationChecker = run;
  }
  void setAlternatingChecker(bool run) {
    configuration.execution.runAlternatingChecker = run;
  }
  void setZXChecker(bool run) { configuration.execution.runZXChecker = run; }

  void disableAllCheckers() {
    configuration.execution.runConstructionChecker = false;
    configuration.execution.runZXChecker           = false;
    configuration.execution.runSimulationChecker   = false;
    configuration.execution.runAlternatingChecker  = false;
  }

  // Optimization: Optimizations are applied during initialization. Already
  // configured and applied optimizations cannot be reverted.
  void runFixOutputPermutationMismatch();
  void fuseSingleQubitGates();
  void reconstructSWAPs();
  void reorderOperations();
  void backpropagateOutputPermutation();
  void elidePermutations();

  // Application: These settings may be changed to influence the sequence in
  // which gates are applied during the equivalence check
  void setConstructionApplicationScheme(
      const ApplicationSchemeType applicationScheme) {
    configuration.application.constructionScheme = applicationScheme;
  }
  void setSimulationApplicationScheme(
      const ApplicationSchemeType applicationScheme) {
    configuration.application.simulationScheme = applicationScheme;
  }
  void setAlternatingApplicationScheme(
      const ApplicationSchemeType applicationScheme) {
    configuration.application.alternatingScheme = applicationScheme;
  }
  void setApplicationScheme(const ApplicationSchemeType applicationScheme) {
    setConstructionApplicationScheme(applicationScheme);
    setSimulationApplicationScheme(applicationScheme);
    setAlternatingApplicationScheme(applicationScheme);
  }
  void setConstructionGateCostProfile(std::string_view profileLocation) {
    configuration.application.constructionScheme =
        ApplicationSchemeType::GateCost;
    configuration.application.profile = profileLocation;
  }
  void setSimulationGateCostProfile(std::string_view profileLocation) {
    configuration.application.simulationScheme =
        ApplicationSchemeType::GateCost;
    configuration.application.profile = profileLocation;
  }
  void setAlternatingGateCostProfile(std::string_view profileLocation) {
    configuration.application.alternatingScheme =
        ApplicationSchemeType::GateCost;
    configuration.application.profile = profileLocation;
  }
  void setGateCostProfile(std::string_view profileLocation) {
    setConstructionGateCostProfile(profileLocation);
    setSimulationGateCostProfile(profileLocation);
    setAlternatingGateCostProfile(profileLocation);
  }
  void setConstructionGateCostFunction(const CostFunction& costFunction) {
    configuration.application.constructionScheme =
        ApplicationSchemeType::GateCost;
    configuration.application.costFunction = costFunction;
  }
  void setSimulationGateCostFunction(const CostFunction& costFunction) {
    configuration.application.simulationScheme =
        ApplicationSchemeType::GateCost;
    configuration.application.costFunction = costFunction;
  }
  void setAlternatingGateCostFunction(const CostFunction& costFunction) {
    configuration.application.alternatingScheme =
        ApplicationSchemeType::GateCost;
    configuration.application.costFunction = costFunction;
  }
  void setGateCostFunction(const CostFunction& costFunction) {
    setConstructionGateCostFunction(costFunction);
    setSimulationGateCostFunction(costFunction);
    setAlternatingGateCostFunction(costFunction);
  }
  // Functionality: These settings may be changed to adjust options for checkers
  // considering the whole functionality
  void setTraceThreshold(double traceThreshold) {
    configuration.functionality.traceThreshold = traceThreshold;
  }
  void setCheckPartialEquivalence(bool checkPE) {
    configuration.functionality.checkPartialEquivalence = checkPE;
  }

  // Simulation: These setting may be changed to adjust the kinds of simulations
  // that are performed
  void setFidelityThreshold(double fidelityThreshold) {
    configuration.simulation.fidelityThreshold = fidelityThreshold;
  }
  void setMaxSims(std::size_t sims) {
    if (sims == 0U) {
      configuration.execution.runSimulationChecker = false;
    }
    configuration.simulation.maxSims = sims;
  }
  void setStateType(StateType stateType) {
    configuration.simulation.stateType = stateType;
  }
  void setSeed(std::size_t seed) {
    configuration.simulation.seed = seed;
    stateGenerator.seedGenerator(seed);
  }
  void storeCEXinput(bool store) {
    configuration.simulation.storeCEXinput = store;
  }
  void storeCEXoutput(bool store) {
    configuration.simulation.storeCEXoutput = store;
  }

protected:
  qc::QuantumComputation qc1;
  qc::QuantumComputation qc2;

  Configuration configuration{};

  StateGenerator stateGenerator;
  std::mutex     stateGeneratorMutex;

  bool                                             done{false};
  std::condition_variable                          doneCond;
  std::mutex                                       doneMutex;
  std::vector<std::unique_ptr<EquivalenceChecker>> checkers;

  Results results{};

  /// Given that one circuit has more qubits than the other, the difference is
  /// assumed to arise from ancillary qubits. This function changes the
  /// additional qubits in the larger circuit to ancillary qubits. Furthermore
  /// it adds corresponding ancillaries in the smaller circuit
  void setupAncillariesAndGarbage();

  /// In some cases both circuits calculate the same function, but on different
  /// qubits. This function tries to correct such mismatches. Note that this is
  /// still highly experimental!
  void fixOutputPermutationMismatch();

  /// Run all configured optimization passes
  void runOptimizationPasses();

  /// Sequential Equivalence Check (TCAD'21)
  /// First, a couple of simulations with various stimuli are conducted.
  /// If any of those stimuli produce output states with a fidelity not close to
  /// 1, the non-equivalence has been shown and the check is finished. Given
  /// that a couple of simulations did not show any signs of non-equivalence,
  /// the circuits are probably equivalent. To assure this, the alternating
  /// decision diagram checker is invoked to determine the equivalence.
  void checkSequential();

  void checkSymbolic();

  /// Parallel Equivalence Check
  /// The parallel flow makes use of the available processing power by
  /// orchestrating all configured checks in a parallel fashion
  void checkParallel();

  /// Signal all checker that they shall abort the computation as soon as
  /// possible since a result has been determined
  void setAndSignalDone() {
    done = true;
    for (const auto& checker : checkers) {
      if (checker) {
        checker->signalDone();
      }
    }
  }

  /// \brief Run an EquivalenceChecker asynchronously
  ///
  /// This function is used to asynchronously run an EquivalenceChecker. It also
  /// takes care of creating the checker if it does not exist yet. Additionally,
  /// it takes care that the checker signals the main thread when it is done
  /// (even in case of an exception).
  ///
  /// \tparam Checker The type of the checker (must be derived from the
  /// EquivalenceChecker class).
  /// \param id The id in the checkers vector where the checker is stored.
  /// \param queue The queue to which the checker shall push its id
  /// once it is done.
  /// \return A future that can be used to wait for the checker to finish.
  template <class Checker>
  std::future<void> asyncRunChecker(const std::size_t             id,
                                    ThreadSafeQueue<std::size_t>& queue) {
    static_assert(std::is_base_of_v<EquivalenceChecker, Checker>,
                  "Checker must be derived from EquivalenceChecker");
    return std::async(std::launch::async, [this, id, &queue]() {
      try {
        auto& checker = checkers[id];
        if (!checker) {
          checker = std::make_unique<Checker>(qc1, qc2, configuration);
        }

        if constexpr (std::is_same_v<Checker, DDSimulationChecker>) {
          auto* const simChecker =
              dynamic_cast<DDSimulationChecker*>(checker.get());
          const std::lock_guard stateGeneratorLock(stateGeneratorMutex);
          simChecker->setRandomInitialState(stateGenerator);
        }

        if (!done) {
          checker->run();
        }
        queue.push(id);
      } catch (const std::exception& e) {
        queue.push(id);
        throw;
      }
    });
  }

  [[nodiscard]] bool simulationsFinished() const {
    return results.performedSimulations == configuration.simulation.maxSims;
  }
};
} // namespace ec
