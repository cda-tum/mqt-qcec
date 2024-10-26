
#include "Configuration.hpp"
#include "EquivalenceCheckingManager.hpp"
#include "EquivalenceCriterion.hpp"
#include "checker/dd/DDEquivalenceChecker.hpp"
#include "ir/QuantumComputation.hpp"

#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
/**
 * Approximate equivalence checking of an original circuit 'c1' and its
 * approximate version 'c2'. The circuits 'c2' were generated using the BQSKit
 * compiler with synthesis_epsilon=0.3 and error_threshold=0.5.
 *
 */
class HSFTest : public testing::Test {
protected:
  ec::Configuration config{};

  std::string testOriginal = "./circuits/approximateEquivalenceTest/";
  std::string testApproximateDir = "./circuits/approximateEquivalenceTest/";

  void SetUp() override {
    config.execution.runConstructionChecker = false;
    config.execution.runAlternatingChecker = false;
    config.execution.runSimulationChecker = false;
    config.execution.runZXChecker = false;
    config.execution.runHSFChecker = true;
    config.functionality.checkApproximateEquivalence = true;
    config.optimizations = {false, false, false, false, false, false, false};
  }
};

TEST_F(HSFTest, approximateEquivalenceCheckingWithHSF) {
  // Check that the approximate equivalence flag has to be enabled to use the
  // HSF checker
  const qc::QuantumComputation c1{testOriginal +
                                  "dj_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{testApproximateDir +
                                  "out_dj_indep_qiskit_10_high_error.qasm"};
  config.functionality.checkApproximateEquivalence = false;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::NoInformation);
}

TEST_F(HSFTest, dj10) {
  const qc::QuantumComputation c1{testOriginal +
                                  "dj_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{testApproximateDir +
                                  "out_dj_indep_qiskit_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.912;
  config.functionality.checkApproximateEquivalence = true;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, ghz10) {
  const qc::QuantumComputation c1{
      testOriginal + "ghz_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testApproximateDir +
      "out_ghz_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.956;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, wstate10) {
  const qc::QuantumComputation c1{testOriginal +
                                  "wstate_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{testApproximateDir +
                                  "out_wstate_indep_qiskit_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.991;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, graphstate10) {
  const qc::QuantumComputation c1{testOriginal +
                                  "graphstate_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testApproximateDir + "out_graphstate_indep_qiskit_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.912;
  config.execution.runAlternatingChecker = false;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, vqe10) {
  const qc::QuantumComputation c1{testOriginal +
                                  "vqe_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{testApproximateDir +
                                  "out_vqe_indep_qiskit_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.998;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, qftNativegates10) {
  const qc::QuantumComputation c1{
      testOriginal +
      "check/qft_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testApproximateDir +
      "check/out_qft_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.9981;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  EXPECT_THROW(ecm.run(), std::overflow_error);
}

TEST_F(HSFTest, qpeExact10) {
  const qc::QuantumComputation c1{
      testOriginal +
      "check/qpeexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testApproximateDir +
      "check/out_qpeexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.9846;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  EXPECT_THROW(ecm.run(), std::overflow_error);
}

TEST_F(HSFTest, qpeInexact10) {
  const qc::QuantumComputation c1{
      testOriginal +
      "check/qpeinexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testApproximateDir +
      "check/out_qpeinexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.9893;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  EXPECT_THROW(ecm.run(), std::overflow_error);
}
