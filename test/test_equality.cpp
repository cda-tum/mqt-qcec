/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "EquivalenceCheckingManager.hpp"
#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"

#include "gtest/gtest.h"
#include <functional>
#include <sstream>
#include <string>

using namespace dd::literals;

class EqualityTest: public testing::Test {
    void SetUp() override {
        qc1                                       = qc::QuantumComputation(nqubits);
        qc2                                       = qc::QuantumComputation(nqubits);
        config.optimizations.fuseSingleQubitGates = false;
        config.optimizations.reorderOperations    = false;
        config.optimizations.reconstructSWAPs     = false;

        config.execution.runSimulationChecker   = false;
        config.execution.runAlternatingChecker  = false;
        config.execution.runConstructionChecker = false;
    }

    void TearDown() override {}

protected:
    dd::QubitCount               nqubits = 1U;
    qc::QuantumComputation       qc1;
    qc::QuantumComputation       qc2;
    ec::Configuration            config{};
};

TEST_F(EqualityTest, GlobalPhase) {
    qc1.x(0);
    qc2.x(0);

    // add a global phase of -1
    qc2.z(0);
    qc2.x(0);
    qc2.z(0);
    qc2.x(0);

    config.execution.runAlternatingChecker = true;
    ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
    ecm.run();
    EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(EqualityTest, CloseButNotEqualAlternating) {
    qc1.x(0);

    qc2.x(0);
    qc2.phase(0, dd::PI / 1024.);

    config.functionality.traceThreshold    = 1e-2;
    config.execution.runAlternatingChecker = true;
    ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, CloseButNotEqualConstruction) {
    qc1.x(0);

    qc2.x(0);
    qc2.phase(0, dd::PI / 1024.);

    config.functionality.traceThreshold     = 1e-2;
    config.execution.runConstructionChecker = true;
    ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(EqualityTest, CloseButNotEqualAlternatingGlobalPhase) {
    qc1.x(0);

    qc2.x(0);
    qc2.phase(0, dd::PI / 1024.);
    // add a global phase of -1
    qc2.z(0);
    qc2.x(0);
    qc2.z(0);
    qc2.x(0);

    config.functionality.traceThreshold    = 1e-2;
    config.execution.runAlternatingChecker = true;
    ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
    ecm.run();
    std::cout << ecm.toString() << std::endl;
    EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}
