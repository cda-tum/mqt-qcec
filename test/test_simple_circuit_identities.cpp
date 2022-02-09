/*
* This file is part of JKQ QCEC library which is released under the MIT license.
* See file README.md or go to http://iic.jku.at/eda/research/quantum_verification/ for more information.
*/

#include "EquivalenceCheckingManager.hpp"

#include "gtest/gtest.h"
#include <functional>
#include <sstream>
#include <string>

class SimpleCircuitIdentitiesTest: public testing::TestWithParam<std::pair<std::string, std::pair<std::string, std::string>>> {
protected:
    qc::QuantumComputation qcOriginal;
    qc::QuantumComputation qcAlternative;
    ec::Configuration      config{};

    std::unique_ptr<ec::EquivalenceCheckingManager> ecm{};

    void SetUp() override {
        const auto [circ1, circ2] = GetParam().second;
        std::stringstream ss1{circ1};
        qcOriginal.import(ss1, qc::OpenQASM);
        std::stringstream ss2{circ2};
        qcAlternative.import(ss2, qc::OpenQASM);

        config.optimizations.reconstructSWAPs     = false;
        config.optimizations.fuseSingleQubitGates = false;
        config.optimizations.reorderOperations    = false;
        EXPECT_NO_THROW(ecm = std::make_unique<ec::EquivalenceCheckingManager>(qcOriginal, qcAlternative, config););
    }

    void TearDown() override {
        std::cout << ecm->toString() << std::endl;
    }
};

INSTANTIATE_TEST_SUITE_P(TestCircuits, SimpleCircuitIdentitiesTest,
                         testing::Values(
                                 std::pair{"Reverse_CNOT_Direction",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\ncx q[0], q[1];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\nh q[0]; h q[1]; cx q[1], q[0]; h q[0]; h q[1];\n"}},
                                 std::pair{"T_T_is_S",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\nt q[0]; t q[0];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\ns q[0];\n"}},
                                 std::pair{"Sdag_from_Clifford",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\nsdg q[0];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\ns q[0]; s q[0]; s q[0];\n"}},
                                 std::pair{"X_from_Clifford",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\nx q[0];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\nh q[0]; s q[0]; s q[0]; h q[0];\n"}},
                                 std::pair{"CZ_from_CX_and_H",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\ncz q[0], q[1];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\nh q[1]; cx q[0], q[1]; h q[1];\n"}},
                                 std::pair{"CS_from_CX_and_T",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\ncs q[0], q[1];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\nt q[0]; cx q[0], q[1]; tdg q[1]; cx q[0], q[1]; t q[1];\n"}},
                                 std::pair{"iSWAP_decomposition",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\niswap q[0], q[1];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\ns q[0]; s q[1]; h q[0]; cx q[0],q[1]; cx q[1],q[0]; h q[1];\n"}},
                                 std::pair{"Global_Phase",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\nz q[0]; x q[0]; z q[0];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[1];\n x q[0];\n"}}),
                         [](const testing::TestParamInfo<SimpleCircuitIdentitiesTest::ParamType>& info) {
                             std::stringstream ss{};
                             ss << info.param.first;
                             return ss.str(); });

TEST_P(SimpleCircuitIdentitiesTest, DefaultOptionsParallel) {
    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_P(SimpleCircuitIdentitiesTest, DefaultOptionsSequential) {
    ecm->setParallel(false);

    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_P(SimpleCircuitIdentitiesTest, DefaultOptionsOnlySimulation) {
    ecm->setAlternatingChecker(false);
    ecm->setConstructionChecker(false);

    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_P(SimpleCircuitIdentitiesTest, DefaultOptionsOnlyConstruction) {
    ecm->setAlternatingChecker(false);
    ecm->setSimulationChecker(false);
    ecm->setConstructionChecker(true);

    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_P(SimpleCircuitIdentitiesTest, GateCostApplicationScheme) {
    ecm->setSimulationChecker(false);
    ecm->setAlternatingApplicationScheme(ec::ApplicationSchemeType::GateCost);
    ecm->setGateCostFunction(&ec::LegacyIBMCostFunction);
    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_P(SimpleCircuitIdentitiesTest, ReorderingOperations) {
    ecm->reorderOperations();
    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_P(SimpleCircuitIdentitiesTest, FuseSingleQubitGates) {
    ecm->fuseSingleQubitGates();
    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}

TEST_P(SimpleCircuitIdentitiesTest, ReconstructSWAPs) {
    ecm->reconstructSWAPs();
    EXPECT_NO_THROW(ecm->run(););

    EXPECT_TRUE(ecm->getResults().consideredEquivalent());
}
