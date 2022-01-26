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
    qc::QuantumComputation qc_original;
    qc::QuantumComputation qc_alternative;
    ec::Configuration      config{};

    void SetUp() override {
        const auto [circ1, circ2] = GetParam().second;
        std::stringstream ss1{circ1};
        qc_original.import(ss1, qc::OpenQASM);
        std::stringstream ss2{circ2};
        qc_alternative.import(ss2, qc::OpenQASM);

        config.simulation.maxSims = std::min(config.simulation.maxSims, std::size_t(1) << qc_original.getNqubits());
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
                                 std::pair{"CZ_from_CX_and_H",
                                           std::pair{"OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\ncz q[0], q[1];\n",
                                                     "OPENQASM 2.0;\ninclude \"qelib1.inc\";\nqreg q[2];\nh q[1]; cx q[0], q[1]; h q[1];\n"}}),
                         [](const testing::TestParamInfo<SimpleCircuitIdentitiesTest::ParamType>& info) {
                             std::stringstream ss{};
                             ss << info.param.first;
                             return ss.str(); });

TEST_P(SimpleCircuitIdentitiesTest, DefaultOptionsParallel) {
    std::unique_ptr<ec::EquivalenceCheckingManager> ecm{};
    EXPECT_NO_THROW(
            ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc_original, qc_alternative, config););

    EXPECT_NO_THROW(
            ecm->run(););

    EXPECT_EQ(ecm->equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_P(SimpleCircuitIdentitiesTest, DefaultOptionsSequential) {
    config.execution.parallel = false;
    std::unique_ptr<ec::EquivalenceCheckingManager> ecm{};
    EXPECT_NO_THROW(
            ecm = std::make_unique<ec::EquivalenceCheckingManager>(qc_original, qc_alternative, config););

    EXPECT_NO_THROW(
            ecm->run(););

    EXPECT_EQ(ecm->equivalence(), ec::EquivalenceCriterion::Equivalent);
}
