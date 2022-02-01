/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
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

    auto config                                  = ec::Configuration{};
    config.optimizations.transformDynamicCircuit = true;

    auto ecm = ec::EquivalenceCheckingManager(bv, dbv, config);

    ecm.run();

    EXPECT_TRUE(ecm.getResults().consideredEquivalent());

    std::cout << ecm.toString() << std::endl;

    auto ecm2 = ec::EquivalenceCheckingManager(dbv, bv, config);

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

    auto config                                       = ec::Configuration{};
    config.optimizations.fixOutputPermutationMismatch = true;
    ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
    ecm.run();
    EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}
