/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#include "EquivalenceCheckingManager.hpp"
#include "algorithms/BernsteinVazirani.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>
#include <sstream>
#include <string>

using ::testing::HasSubstr;

class GeneralTest: public ::testing::Test {
protected:
    qc::QuantumComputation qc1;
    qc::QuantumComputation qc2;
};

TEST_F(GeneralTest, DynamicCircuit) {
    auto s   = qc::BitString(15U);
    auto bv  = qc::BernsteinVazirani(s);
    auto dbv = qc::BernsteinVazirani(s, true);

    auto config = ec::Configuration{};
    EXPECT_THROW(ec::EquivalenceCheckingManager(bv, dbv, config), std::runtime_error);

    config.optimizations.transformDynamicCircuit = true;

    auto ecm = ec::EquivalenceCheckingManager(bv, dbv, config);

    ecm.run();

    EXPECT_TRUE(ecm.getResults().consideredEquivalent());

    std::cout << ecm.toString() << std::endl;

    auto ecm2 = ec::EquivalenceCheckingManager(dbv, dbv, config);

    ecm2.run();

    EXPECT_TRUE(ecm2.getResults().consideredEquivalent());

    std::cout << ecm2.toString() << std::endl;
}

TEST_F(GeneralTest, FixOutputPermutationMismatch) {
    qc1.addQubitRegister(2U);
    qc1.x(0);
    qc1.x(1);
    qc1.setLogicalQubitAncillary(1);
    std::cout << qc1 << std::endl;

    qc2.addQubitRegister(3U);
    qc2.x(0);
    qc2.i(1);
    qc2.x(2);

    qc2.outputPermutation.erase(1);
    qc2.setLogicalQubitAncillary(1);
    qc2.setLogicalQubitGarbage(1);
    std::cout << qc2 << std::endl;

    auto config                   = ec::Configuration{};
    config.execution.runZXChecker = false;

    config.optimizations.fixOutputPermutationMismatch = false;
    ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
    ecm.run();
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());

    // also try post initialization routine
    ec::EquivalenceCheckingManager ecm2(qc1, qc2);
    ecm2.runFixOutputPermutationMismatch();
    ecm2.run();
    EXPECT_TRUE(ecm2.getResults().consideredEquivalent());
}

TEST_F(GeneralTest, RemoveDiagonalGatesBeforeMeasure) {
    qc1.addQubitRegister(1U);
    qc1.addClassicalRegister(1U);
    qc1.x(0);
    qc1.measure(0, 0U);
    std::cout << qc1 << std::endl;
    std::cout << "-----------------------------" << std::endl;

    qc2.addQubitRegister(1U);
    qc2.addClassicalRegister(1U);
    qc2.x(0);
    qc2.z(0);
    qc2.measure(0, 0U);
    std::cout << qc2 << std::endl;
    std::cout << "-----------------------------" << std::endl;

    // the standard check should reveal that both circuits are not equivalent
    auto ecm = ec::EquivalenceCheckingManager(qc1, qc2);
    ecm.run();
    EXPECT_FALSE(ecm.getResults().consideredEquivalent());
    std::cout << ecm.toString() << std::endl;

    // simulations should suggest both circuits to be equivalent
    ecm.reset();
    ecm.setAlternatingChecker(false);
    ecm.setZXChecker(false);
    ecm.run();
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
    std::cout << ecm.toString() << std::endl;

    // if configured to remove diagonal gates before measurements, the circuits are equivalent
    auto config                                           = ec::Configuration{};
    config.optimizations.removeDiagonalGatesBeforeMeasure = true;
    auto ecm2                                             = ec::EquivalenceCheckingManager(qc1, qc2, config);
    ecm2.run();
    EXPECT_TRUE(ecm2.getResults().consideredEquivalent());
    std::cout << ecm2.toString() << std::endl;
}

TEST_F(GeneralTest, NothingToDo) {
    qc1.addQubitRegister(1U);
    qc1.x(0);
    qc2.addQubitRegister(1U);
    qc2.x(0);

    auto config                             = ec::Configuration{};
    config.execution.runAlternatingChecker  = false;
    config.execution.runSimulationChecker   = false;
    config.execution.runConstructionChecker = false;
    config.execution.runZXChecker           = false;

    auto ecm = ec::EquivalenceCheckingManager(qc1, qc2, config);
    ecm.run();
    EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::NoInformation);
}

TEST_F(GeneralTest, NoGateCancellation) {
    qc1.addQubitRegister(2U);
    qc2.addQubitRegister(2U);

    // ignore swaps
    qc1.swap(0, 1);
    qc2.swap(0, 1);

    // different gates that cannot be cancelled
    qc1.z(0);
    qc1.x(1);
    qc2.x(1);
    qc2.z(0);

    // single qubit gates that cannot be cancelled
    qc1.x(0);
    qc2.x(1);
    qc1.x(1);
    qc2.x(0);

    // two-qubit gates that cannot be cancelled
    qc1.x(0, 1_pc);
    qc2.x(1, 0_pc);
    qc1.x(0, 1_pc);
    qc2.x(1, 0_pc);

    // gates with parameters that cannot be cancelled
    qc1.phase(0, 2.0);
    qc2.phase(0, -2.0);
    qc1.phase(0, -2.0);
    qc2.phase(0, 2.0);

    // gates with different number of controls that cannot be cancelled
    qc1.x(0);
    qc2.x(0, 1_pc);
    qc1.x(0, 1_pc);
    qc2.x(0);

    auto config                               = ec::Configuration{};
    config.optimizations.reorderOperations    = false;
    config.optimizations.fuseSingleQubitGates = false;
    config.optimizations.reconstructSWAPs     = false;
    config.application.alternatingScheme      = ec::ApplicationSchemeType::OneToOne;

    ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
    ecm.run();
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
    std::cout << ecm.toString() << std::endl;
}
