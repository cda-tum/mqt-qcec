//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "Configuration.hpp"
#include "EquivalenceCheckingManager.hpp"
#include "checker/dd/applicationscheme/ApplicationScheme.hpp"
#include "checker/dd/applicationscheme/GateCostApplicationScheme.hpp"
#include "ir/QuantumComputation.hpp"
#include "qasm3/Importer.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

class CompilationFlowTest : public testing::TestWithParam<std::string> {
protected:
  qc::QuantumComputation qcOriginal;
  qc::QuantumComputation qcTranspiled;

  std::string testOriginalDir = "./circuits/original/";
  std::string testTranspiledDir = "./circuits/transpiled/";

  ec::Configuration configuration{};

  void SetUp() override {
    qcOriginal =
        qasm3::Importer::importf(testOriginalDir + GetParam() + ".qasm");
    qcTranspiled = qasm3::Importer::importf(testTranspiledDir + GetParam() +
                                            "_transpiled.qasm");

    configuration.execution.runAlternatingChecker = true;
    configuration.execution.runConstructionChecker = false;
    configuration.execution.runSimulationChecker = false;

    configuration.application.alternatingScheme =
        ec::ApplicationSchemeType::GateCost;
    configuration.application.costFunction = ec::legacyCostFunction;
  }
};

INSTANTIATE_TEST_SUITE_P(
    CompilationFlowTest, CompilationFlowTest,
    testing::Values("dk27_225", "pcler8_248", "5xp1_194", "alu1_198",
                    "mlp4_245", "dk17_224", "add6_196", "C7552_205", "cu_219",
                    "example2_231", "c2_181", "rd73_312", "cm150a_210",
                    "cm163a_213", "c2_182", "sym9_317", "cm151a_211",
                    "apla_203"),
    [](const testing::TestParamInfo<CompilationFlowTest::ParamType>& inf) {
      auto s = inf.param;
      std::replace(s.begin(), s.end(), '-', '_');
      return s;
    });

TEST_P(CompilationFlowTest, EquivalenceCompilationFlow) {
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, configuration);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceCompilationFlowNoElidePermutations) {
  configuration.optimizations.elidePermutations = false;
  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, configuration);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}

TEST_P(CompilationFlowTest, EquivalenceCompilationFlowParallel) {
  configuration.execution.runSimulationChecker = true;
  configuration.execution.parallel = true;

  ec::EquivalenceCheckingManager ecm(qcOriginal, qcTranspiled, configuration);
  ecm.run();
  std::cout << ecm.getResults() << "\n";
  EXPECT_TRUE(ecm.getResults().consideredEquivalent());
}
