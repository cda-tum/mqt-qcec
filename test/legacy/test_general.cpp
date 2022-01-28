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
}

TEST_F(GeneralTest, FixOutputPermutationMismatch) {
    qc1.addQubitRegister(2);
    qc1.emplace_back<qc::StandardOperation>(2, 0, qc::X);
    qc1.emplace_back<qc::StandardOperation>(2, 1, qc::X);
    qc1.setLogicalQubitAncillary(1);
    std::cout << qc1 << std::endl;

    qc2.addQubitRegister(3);
    qc2.emplace_back<qc::StandardOperation>(3, 0, qc::X);
    qc2.emplace_back<qc::StandardOperation>(3, 1, qc::I);
    qc2.emplace_back<qc::StandardOperation>(3, 2, qc::X);
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
