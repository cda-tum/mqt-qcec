//
// This file is part of MQT QCEC library which is released under the MIT license.
// See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
//

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"
#include <functional>
#include <iostream>
#include <random>
#include <string>

class JournalTestNonEQ: public testing::TestWithParam<std::tuple<std::string, unsigned short>> {
protected:
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_transpiled;
    ec::Configuration      config;

    std::string test_original_dir   = "./circuits/original/";
    std::string test_transpiled_dir = "./circuits/transpiled/";

    unsigned short tries = 10U;

    double      min_time = 0.;
    double      max_time = 0.;
    double      avg_time = 0.;
    std::size_t min_sims = 0U;
    std::size_t max_sims = 0U;
    double      avg_sims = 0.;

    std::string transpiled_file{};

    std::mt19937_64                                   mt;
    std::uniform_int_distribution<unsigned long long> distribution;
    std::function<unsigned long long()>               rng;

    unsigned short                         gates_to_remove = 0U;
    std::set<std::set<unsigned long long>> already_removed{};

    unsigned short successes = 0U;

    static bool setExists(const std::set<std::set<unsigned long long>>& all, const std::set<unsigned long long>& test) {
        // first entry
        if (all.empty()) {
            return false;
        }

        for (const auto& set: all) {
            bool equals = true;
            for (auto setit = set.begin(), testit = test.begin(); setit != set.end(); ++setit, ++testit) {
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
        ss << test_transpiled_dir << std::get<0>(GetParam()) << "_transpiled.qasm";
        transpiled_file = ss.str();

        std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> random_data{};
        std::random_device                                                    rd;
        std::generate(begin(random_data), end(random_data), [&]() { return rd(); });
        std::seed_seq seeds(begin(random_data), end(random_data));
        mt.seed(seeds);
        distribution = decltype(distribution)(0U, qc_transpiled.getNops() - 1U);
        rng          = [this]() { return distribution(mt); };

        gates_to_remove = std::get<1>(GetParam());

        min_time = 0.;
        max_time = 0.;
        avg_time = 0.;
        min_sims = 0U;
        max_sims = 0U;
        avg_sims = 0.;
    }

    void addToStatistics(unsigned short try_count, double time, std::size_t nsims) {
        if (try_count == 0U) {
            min_time = time;
            max_time = time;
            avg_time = time;
            min_sims = nsims;
            max_sims = nsims;
            avg_sims = static_cast<double>(nsims);
        } else {
            min_time = std::min(min_time, time);
            min_sims = std::min(min_sims, nsims);
            max_time = std::max(max_time, time);
            max_sims = std::max(max_sims, nsims);
            avg_time = (avg_time * try_count + time) / static_cast<double>(try_count + 1U);
            avg_sims = (avg_sims * try_count + static_cast<double>(nsims)) / static_cast<double>(try_count + 1U);
        }
    }
};

INSTANTIATE_TEST_SUITE_P(Journal, JournalTestNonEQ,
                         testing::Combine(
                                 testing::Values(
                                         "dk27_225",
                                         "pcler8_248",
                                         "5xp1_194",
                                         "alu1_198",
                                         //				                         "mlp4_245",
                                         "dk17_224",
                                         //				                         "add6_196",
                                         //				                         "C7552_205",
                                         "cu_219",
                                         //				                         "example2_231",
                                         "c2_181",
                                         "rd73_312",
                                         //				                         "cm150a_210",
                                         "cm163a_213",
                                         "c2_182",
                                         "sym9_317",
                                         "mod5adder_306",
                                         "rd84_313"
                                         //				                         "cm151a_211",
                                         //				                         "apla_203"
                                         ),
                                 testing::Range(static_cast<unsigned short>(1U), static_cast<unsigned short>(4U), 2)),
                         [](const testing::TestParamInfo<JournalTestNonEQ::ParamType>& info) {
	                         std::string name = std::get<0>(info.param);
	                         unsigned short gates_to_remove = std::get<1>(info.param);
	                         std::replace( name.begin(), name.end(), '-', '_');
	                         std::stringstream ss{};
	                         ss << name << "_removed_" << gates_to_remove;
	                         return ss.str(); });

TEST_P(JournalTestNonEQ, PowerOfSimulation) {
    config.execution.runAlternatingChecker  = false;
    config.execution.runConstructionChecker = false;
    config.execution.runSimulationChecker   = true;
    config.execution.parallel               = false;
    config.execution.timeout                = 60s;
    config.simulation.maxSims               = 16U;
    config.application.simulationScheme     = ec::ApplicationSchemeType::Sequential;

    for (unsigned short i = 0U; i < tries; ++i) {
        qc_original.import(test_original_dir + std::get<0>(GetParam()) + ".real");

        // generate non-eq circuit
        std::set<unsigned long long> removed{};
        do {
            qc_transpiled.import(transpiled_file);
            removed.clear();
            for (unsigned short j = 0U; j < gates_to_remove; ++j) {
                auto gate_to_remove = rng() % qc_transpiled.getNops();
                while (removed.count(gate_to_remove)) {
                    gate_to_remove = rng() % qc_transpiled.getNops();
                }
                removed.insert(gate_to_remove);
                auto it = qc_transpiled.begin();
                std::advance(it, gate_to_remove);
                if (it == qc_transpiled.end()) {
                    continue;
                }

                qc_transpiled.erase(it);
            }
        } while (setExists(already_removed, removed));
        already_removed.insert(removed);
        ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
        ecm.run();
        auto results = ecm.getResults();
        std::cout << "[" << i << "] ";
        std::cout << toString(results.equivalence) << std::endl;
        addToStatistics(i, results.checkTime + results.preprocessingTime, results.performedSimulations);
        if (results.equivalence == ec::EquivalenceCriterion::NotEquivalent) {
            successes++;
        }
    }
    std::cout << qc_original.getName() << ";" << qc_original.getNqubits() << ";" << qc_original.getNops() << ";" << qc_transpiled.getNops()
              << ";" << tries << ";"
              << min_sims << ";" << max_sims << ";" << avg_sims << ";"
              << min_time << ";" << max_time << ";" << avg_time << ";"
              << static_cast<double>(successes) / static_cast<double>(tries) << ";" << std::endl;
}

TEST_P(JournalTestNonEQ, PowerOfSimulationParallel) {
    config.execution.runAlternatingChecker  = false;
    config.execution.runConstructionChecker = false;
    config.execution.runZXChecker           = false;
    config.execution.runSimulationChecker   = true;
    config.execution.parallel               = true;
    config.execution.timeout                = 60s;
    config.simulation.maxSims               = 16U;
    config.application.simulationScheme     = ec::ApplicationSchemeType::Sequential;

    for (unsigned short i = 0; i < tries; ++i) {
        qc_original.import(test_original_dir + std::get<0>(GetParam()) + ".real");

        // generate non-eq circuit
        std::set<unsigned long long> removed{};
        do {
            qc_transpiled.import(transpiled_file);
            removed.clear();
            for (unsigned short j = 0U; j < gates_to_remove; ++j) {
                auto gate_to_remove = rng() % qc_transpiled.getNops();
                while (removed.count(gate_to_remove)) {
                    gate_to_remove = rng() % qc_transpiled.getNops();
                }
                removed.insert(gate_to_remove);
                auto it = qc_transpiled.begin();
                std::advance(it, gate_to_remove);
                if (it == qc_transpiled.end()) {
                    continue;
                }

                qc_transpiled.erase(it);
            }
        } while (setExists(already_removed, removed));
        already_removed.insert(removed);
        ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
        ecm.run();
        auto results = ecm.getResults();
        std::cout << "[" << i << "] ";
        std::cout << toString(results.equivalence) << std::endl;
        addToStatistics(i, results.checkTime + results.preprocessingTime, results.performedSimulations);
        if (results.equivalence == ec::EquivalenceCriterion::NotEquivalent) {
            successes++;
        }
    }
    std::cout << qc_original.getName() << ";" << qc_original.getNqubits() << ";" << qc_original.getNops() << ";" << qc_transpiled.getNops()
              << ";" << tries << ";"
              << min_sims << ";" << max_sims << ";" << avg_sims << ";"
              << min_time << ";" << max_time << ";" << avg_time << ";"
              << static_cast<double>(successes) / static_cast<double>(tries) << ";" << std::endl;
}

class JournalTestEQ: public testing::TestWithParam<std::string> {
protected:
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_transpiled;
    ec::Configuration      config{};

    std::string test_original_dir   = "./circuits/original/";
    std::string test_transpiled_dir = "./circuits/transpiled/";

    std::string transpiled_file{};

    void SetUp() override {
        config.execution.parallel               = false;
        config.execution.runConstructionChecker = false;
        config.execution.runAlternatingChecker  = false;
        config.execution.runZXChecker           = false;
        config.execution.runSimulationChecker   = false;
        config.simulation.maxSims               = 16;
        config.application.simulationScheme     = ec::ApplicationSchemeType::OneToOne;
        config.execution.timeout                = 60s;

        std::stringstream ss{};
        ss << test_transpiled_dir << GetParam() << "_transpiled.qasm";
        transpiled_file = ss.str();

        qc_original.import(test_original_dir + GetParam() + ".real");
        qc_transpiled.import(transpiled_file);
    }
};

INSTANTIATE_TEST_SUITE_P(Journal, JournalTestEQ,
                         testing::Values(
                                 "dk27_225",
                                 "pcler8_248",
                                 "5xp1_194",
                                 "alu1_198"
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
	                         std::replace( name.begin(), name.end(), '-', '_');
	                         std::stringstream ss{};
	                         ss << name;
	                         return ss.str(); });

TEST_P(JournalTestEQ, EQReference) {
    config.execution.runConstructionChecker = true;
    config.application.constructionScheme   = ec::ApplicationSchemeType::OneToOne;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQNaive) {
    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::OneToOne;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQProportional) {
    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::Proportional;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQLookahead) {
    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::Lookahead;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQPowerOfSimulation) {
    config.execution.runSimulationChecker = true;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQReferenceParallel) {
    config.execution.parallel               = true;
    config.execution.runConstructionChecker = true;
    config.execution.runSimulationChecker   = true; // additionally run simulations
    config.application.constructionScheme   = ec::ApplicationSchemeType::OneToOne;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQNaiveParallel) {
    config.execution.parallel              = true;
    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::OneToOne;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQProportionalParallel) {
    config.execution.parallel              = true;
    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::Proportional;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQLookaheadParallel) {
    config.execution.parallel              = true;
    config.execution.runAlternatingChecker = true;
    config.application.alternatingScheme   = ec::ApplicationSchemeType::Lookahead;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(JournalTestEQ, EQPowerOfSimulationParallel) {
    config.execution.parallel             = true;
    config.execution.runSimulationChecker = true;

    ec::EquivalenceCheckingManager ecm(qc_original, qc_transpiled, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}
