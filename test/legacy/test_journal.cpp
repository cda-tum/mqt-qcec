//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"
#include <functional>
#include <iostream>
#include <random>
#include <string>

class JournalTestNonEQ
    : public testing::TestWithParam<std::tuple<std::string, unsigned short>> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcTranspiled;
  ec::Configuration      config;

  std::string testOriginalDir   = "./circuits/original/";
  std::string testTranspiledDir = "./circuits/transpiled/";

  unsigned short tries = 10U;

  double      minTime = 0.;
  double      maxTime = 0.;
  double      avgTime = 0.;
  std::size_t minSims = 0U;
  std::size_t maxSims = 0U;
  double      avgSims = 0.;

  std::string transpiledFile{};

  std::mt19937_64                                   mt;
  std::uniform_int_distribution<unsigned long long> distribution;
  std::function<unsigned long long()>               rng;

  unsigned short                         gatesToRemove = 0U;
  std::set<std::set<unsigned long long>> alreadyRemoved{};

  unsigned short successes = 0U;

  static bool setExists(const std::set<std::set<unsigned long long>>& all,
                        const std::set<unsigned long long>&           test) {
    // first entry
    if (all.empty()) {
      return false;
    }

    for (const auto& set : all) {
      bool equals = true;
      for (auto setit = set.begin(), testit = test.begin(); setit != set.end();
           ++setit, ++testit) {
        if (*setit != *testit) {
          equals = false;
          break;
        }
      }
      if (equals) {
        return true;
      }
    }
    return false;
  }

  void SetUp() override {
    std::stringstream ss{};
    ss << testTranspiledDir << std::get<0>(GetParam()) << "_transpiled.qasm";
    transpiledFile = ss.str();

    std::array<std::mt19937_64::result_type, std::mt19937_64::state_size>
                       random_data{};
    std::random_device rd;
    std::generate(begin(random_data), end(random_data), [&]() { return rd(); });
    std::seed_seq seeds(begin(random_data), end(random_data));
    mt.seed(seeds);
    distribution = decltype(distribution)(0U, qcTranspiled.getNops() - 1U);
    rng          = [this]() { return distribution(mt); };

    gatesToRemove = std::get<1>(GetParam());

    minTime = 0.;
    maxTime = 0.;
    avgTime = 0.;
    minSims = 0U;
    maxSims = 0U;
    avgSims = 0.;
  }

  void addToStatistics(unsigned short try_count, double time,
                       std::size_t nsims) {
    if (try_count == 0U) {
      minTime = time;
      maxTime = time;
      avgTime = time;
      minSims = nsims;
      maxSims = nsims;
      avgSims = static_cast<double>(nsims);
    } else {
      minTime = std::min(minTime, time);
      minSims = std::min(minSims, nsims);
      maxTime = std::max(maxTime, time);
      maxSims = std::max(maxSims, nsims);
      avgTime =
          (avgTime * try_count + time) / static_cast<double>(try_count + 1U);
      avgSims = (avgSims * try_count + static_cast<double>(nsims)) /
                static_cast<double>(try_count + 1U);
    }
  }
};

INSTANTIATE_TEST_SUITE_P(
    Journal, JournalTestNonEQ,
    testing::Combine(
        testing::Values(
            "dk27_225", "pcler8_248", "5xp1_194", "alu1_198",
            //				                         "mlp4_245",
            "dk17_224",
            //				                         "add6_196",
            //				                         "C7552_205",
            "cu_219",
            //				                         "example2_231",
            "c2_181", "rd73_312",
            //				                         "cm150a_210",
            "cm163a_213", "c2_182", "sym9_317", "mod5adder_306", "rd84_313"
            //				                         "cm151a_211",
            //				                         "apla_203"
            ),
        testing::Range(static_cast<unsigned short>(1U),
                       static_cast<unsigned short>(4U), 2)),
    [](const testing::TestParamInfo<JournalTestNonEQ::ParamType>& info) {
      std::string    name            = std::get<0>(info.param);
      unsigned short gates_to_remove = std::get<1>(info.param);
      std::replace(name.begin(), name.end(), '-', '_');
      std::stringstream ss{};
      ss << name << "_removed_" << gates_to_remove;
      return ss.str();
    });

TEST_P(JournalTestNonEQ, PowerOfSimulation) {
  config.execution.runAlternatingChecker  = false;
  config.execution.runConstructionChecker = false;
  config.execution.runSimulationChecker   = true;
  config.execution.parallel               = false;
  config.execution.timeout                = 60s;
  config.simulation.maxSims               = 16U;
  config.application.simulationScheme = ec::ApplicationSchemeType::Sequential;

  for (unsigned short i = 0U; i < tries; ++i) {
    qcOriginal.import(testOriginalDir + std::get<0>(GetParam()) + ".real");

    // generate non-eq circuit
    std::set<unsigned long long> removed{};
    do {
      qcTranspiled.import(transpiledFile);
      removed.clear();
      for (unsigned short j = 0U; j < gatesToRemove; ++j) {
        auto gate_to_remove = rng() % qcTranspiled.getNops();
        while (removed.count(gate_to_remove) != 0U) {
          gate_to_remove = rng() % qcTranspiled.getNops();
        }
        removed.insert(gate_to_remove);
        auto it = qcTranspiled.begin();
        std::advance(it, gate_to_remove);
        if (it == qcTranspiled.end()) {
          continue;
        }

        qcTranspiled.erase(it);
      }
    } while (setExists(alreadyRemoved, removed));
    alreadyRemoved.insert(removed);
    ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
    ecm.run();
    auto results = ecm.getResults();
    std::cout << "[" << i << "] ";
    std::cout << toString(results.equivalence) << std::endl;
    addToStatistics(i, results.checkTime + results.preprocessingTime,
                    results.performedSimulations);
    if (results.equivalence == ec::EquivalenceCriterion::NotEquivalent) {
      successes++;
    }
  }
  std::cout << qcOriginal.getName() << ";" << qcOriginal.getNqubits() << ";"
            << qcOriginal.getNops() << ";" << qcTranspiled.getNops() << ";"
            << tries << ";" << minSims << ";" << maxSims << ";" << avgSims
            << ";" << minTime << ";" << maxTime << ";" << avgTime << ";"
            << static_cast<double>(successes) / static_cast<double>(tries)
            << ";" << std::endl;
}

TEST_P(JournalTestNonEQ, PowerOfSimulationParallel) {
  config.execution.runAlternatingChecker  = false;
  config.execution.runConstructionChecker = false;
  config.execution.runZXChecker           = false;
  config.execution.runSimulationChecker   = true;
  config.execution.parallel               = true;
  config.execution.timeout                = 60s;
  config.simulation.maxSims               = 16U;
  config.application.simulationScheme = ec::ApplicationSchemeType::Sequential;

  for (unsigned short i = 0; i < tries; ++i) {
    qcOriginal.import(testOriginalDir + std::get<0>(GetParam()) + ".real");

    // generate non-eq circuit
    std::set<unsigned long long> removed{};
    do {
      qcTranspiled.import(transpiledFile);
      removed.clear();
      for (unsigned short j = 0U; j < gatesToRemove; ++j) {
        auto gate_to_remove = rng() % qcTranspiled.getNops();
        while (removed.count(gate_to_remove) != 0U) {
          gate_to_remove = rng() % qcTranspiled.getNops();
        }
        removed.insert(gate_to_remove);
        auto it = qcTranspiled.begin();
        std::advance(it, gate_to_remove);
        if (it == qcTranspiled.end()) {
          continue;
        }

        qcTranspiled.erase(it);
      }
    } while (setExists(alreadyRemoved, removed));
    alreadyRemoved.insert(removed);
    ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
    ecm.run();
    auto results = ecm.getResults();
    std::cout << "[" << i << "] ";
    std::cout << toString(results.equivalence) << std::endl;
    addToStatistics(i, results.checkTime + results.preprocessingTime,
                    results.performedSimulations);
    if (results.equivalence == ec::EquivalenceCriterion::NotEquivalent) {
      successes++;
    }
  }
  std::cout << qcOriginal.getName() << ";" << qcOriginal.getNqubits() << ";"
            << qcOriginal.getNops() << ";" << qcTranspiled.getNops() << ";"
            << tries << ";" << minSims << ";" << maxSims << ";" << avgSims
            << ";" << minTime << ";" << maxTime << ";" << avgTime << ";"
            << static_cast<double>(successes) / static_cast<double>(tries)
            << ";" << std::endl;
}

class JournalTestEQ : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcTranspiled;
  ec::Configuration      config{};

  std::string testOriginalDir   = "./circuits/original/";
  std::string testTranspiledDir = "./circuits/transpiled/";

  std::string transpiledFile{};

  void SetUp() override {
    config.execution.parallel               = false;
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runZXChecker           = false;
    config.execution.runSimulationChecker   = false;
    config.simulation.maxSims               = 16;
    config.application.simulationScheme = ec::ApplicationSchemeType::OneToOne;
    config.execution.timeout            = 60s;

    std::stringstream ss{};
    ss << testTranspiledDir << GetParam() << "_transpiled.qasm";
    transpiledFile = ss.str();

    qcOriginal.import(testOriginalDir + GetParam() + ".real");
    qcTranspiled.import(transpiledFile);
  }
};

INSTANTIATE_TEST_SUITE_P(
    Journal, JournalTestEQ,
    testing::Values("dk27_225", "pcler8_248", "5xp1_194", "alu1_198"
                    //"mlp4_245"
                    //"dk17_224",
                    //"add6_196",
                    //"C7552_205",
                    //"cu_219",
                    //"example2_231",
                    //"c2_181",
                    //"rd73_312",
                    //"cm150a_210",
                    //"cm163a_213",
                    //"c2_182",
                    //"sym9_317",
                    //"mod5adder_306",
                    //"rd84_313",
                    //"cm151a_211",
                    //"apla_203"
                    ),
    [](const testing::TestParamInfo<JournalTestEQ::ParamType>& info) {
      std::string name = info.param;
      std::replace(name.begin(), name.end(), '-', '_');
      std::stringstream ss{};
      ss << name;
      return ss.str();
    });

TEST_P(JournalTestEQ, EQReference) {
  config.execution.runConstructionChecker = true;
  config.application.constructionScheme   = ec::ApplicationSchemeType::OneToOne;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQNaive) {
  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme   = ec::ApplicationSchemeType::OneToOne;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQProportional) {
  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme =
      ec::ApplicationSchemeType::Proportional;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQLookahead) {
  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme   = ec::ApplicationSchemeType::Lookahead;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQPowerOfSimulation) {
  config.execution.runSimulationChecker = true;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQReferenceParallel) {
  config.execution.parallel               = true;
  config.execution.runConstructionChecker = true;
  config.execution.runSimulationChecker = true; // additionally run simulations
  config.application.constructionScheme = ec::ApplicationSchemeType::OneToOne;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQNaiveParallel) {
  config.execution.parallel              = true;
  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme   = ec::ApplicationSchemeType::OneToOne;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQProportionalParallel) {
  config.execution.parallel              = true;
  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme =
      ec::ApplicationSchemeType::Proportional;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQLookaheadParallel) {
  config.execution.parallel              = true;
  config.execution.runAlternatingChecker = true;
  config.application.alternatingScheme   = ec::ApplicationSchemeType::Lookahead;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQPowerOfSimulationParallel) {
  config.execution.parallel             = true;
  config.execution.runSimulationChecker = true;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, config);
  ecm.run();
  std::cout << ecm.toString() << std::endl;
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}
