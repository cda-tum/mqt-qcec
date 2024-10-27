
#include "Configuration.hpp"
#include "EquivalenceCheckingManager.hpp"
#include "EquivalenceCriterion.hpp"
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

  std::string testDir = "./circuits/approximateEquivalenceTest/";

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
  // Test that enabling the approximate equivalence flag is required to use the
  // HSF checker
  const qc::QuantumComputation c1{testDir +
                                  "dj_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{testDir +
                                  "out_dj_indep_qiskit_10_high_error.qasm"};
  config.functionality.checkApproximateEquivalence = false;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
}

TEST_F(HSFTest, dj10ParallelChecking) {
  // Test the parallel equivalence checking flow with only the HSF checker
  // enabled
  const qc::QuantumComputation c1{testDir +
                                  "dj_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{testDir +
                                  "out_dj_indep_qiskit_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.912;
  config.execution.parallel = true;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, vqe10ParallelChecking) {
  // Test the parallel equivalence checking flow with only the HSF checker
  // enabled (parallel checking is enabled per default)
  const qc::QuantumComputation c1{testDir +
                                  "vqe_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{testDir +
                                  "out_vqe_indep_qiskit_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.998;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

/*
Note: This test fails due to a timeout. The issue arises because the std::future
object blocks if a thread is stuck in a long-running operation. Although the HSF
checker has completed, we wait for the alternating/construction checker to
finish, which take significantly longer. This could potentially be resolved by
using std::jthread.
*/
// TEST_F(HSFTest, wstate10ParallelChecking) {
//   // Test the parallel equivalence checking flow where the HSF checker
//   delivers
//   // results first
//   const qc::QuantumComputation c1{testDir +
//                                   "wstate_indep_qiskit_10_no_measure.qasm"};
//   const qc::QuantumComputation c2{testDir +
//                                   "out_wstate_indep_qiskit_10_high_error.qasm"};
//   config.functionality.approximateCheckingThreshold = 0.991;
//   config.execution.runConstructionChecker = true;
//   config.execution.runAlternatingChecker = true;
//   config.execution.parallel = true;
//   ec::EquivalenceCheckingManager ecm(c1, c2, config);
//   ecm.run();
//   EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
// }

TEST_F(HSFTest, ghz10ParallelChecking) {
  // Test the parallel equivalence checking flow where the alternating or
  // construction checker delivers results first
  const qc::QuantumComputation c1{
      testDir + "ghz_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testDir + "out_ghz_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.956;
  config.execution.runConstructionChecker = true;
  config.execution.runAlternatingChecker = true;
  config.execution.parallel = true;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, ghz10SequentialChecking) {
  // Test the sequential equivalence checking flow where the alternating or
  // construction checker delivers results first
  const qc::QuantumComputation c1{
      testDir + "ghz_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testDir + "out_ghz_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.956;
  config.execution.runConstructionChecker = true;
  config.execution.runAlternatingChecker = true;
  config.execution.parallel = false;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, graphstate10SequentialChecking) {
  // Test the sequential equivalence checking flow with only the HSF checker
  // enabled
  const qc::QuantumComputation c1{testDir +
                                  "graphstate_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testDir + "out_graphstate_indep_qiskit_10_high_error.qasm"};
  config.functionality.approximateCheckingThreshold = 0.912;
  config.execution.parallel = false;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(HSFTest, qftNativegates10DecisionOverflow) {
  // Test that an overflow_error is thrown if the number of decisions exceeds
  // the maximum number of decisions
  const qc::QuantumComputation c1{
      testDir + "check/qft_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testDir + "check/out_qft_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  EXPECT_THROW(ecm.run(), std::overflow_error);
}

TEST_F(HSFTest, qpeExact10DecisionOverflow) {
  // Test that an overflow_error is thrown if the number of decisions exceeds
  // the maximum number of decisions
  const qc::QuantumComputation c1{
      testDir +
      "check/qpeexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testDir +
      "check/out_qpeexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  EXPECT_THROW(ecm.run(), std::overflow_error);
}

TEST_F(HSFTest, qpeInexact10DecisionOverflow) {
  // Test that an overflow_error is thrown if the number of decisions exceeds
  // the maximum number of decisions
  const qc::QuantumComputation c1{
      testDir +
      "check/qpeinexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      testDir +
      "check/out_qpeinexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  EXPECT_THROW(ecm.run(), std::overflow_error);
}
