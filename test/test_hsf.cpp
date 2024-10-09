#include "EquivalenceCriterion.hpp"
#include "checker/dd/HybridSchrodingerFeynmanChecker.hpp"
#include "dd/DDpackageConfig.hpp"
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

TEST(HSFTest, dj10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "dj_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{"./circuits/approximateEquivalenceTest/"
                                  "out_dj_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.912,
                                                               6);
  auto result = hsf.run();
  EXPECT_EQ(result, ec::EquivalenceCriterion::Equivalent);
}

TEST(HSFTest, ghz10) {
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/"
      "ghz_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/"
      "out_ghz_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.956,
                                                               6);
  auto result = hsf.run();
  EXPECT_EQ(result, ec::EquivalenceCriterion::Equivalent);
}

TEST(HSFTest, wstate10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "wstate_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{"./circuits/approximateEquivalenceTest/"
                                  "out_wstate_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.991,
                                                               6);
  auto result = hsf.run();
  EXPECT_EQ(result, ec::EquivalenceCriterion::Equivalent);
}

TEST(HSFTest, graphstate10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "graphstate_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/"
      "out_graphstate_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.912,
                                                               6);
  auto result = hsf.run();
  EXPECT_EQ(result, ec::EquivalenceCriterion::Equivalent);
}

TEST(HSFTest, vqe10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "vqe_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{"./circuits/approximateEquivalenceTest/"
                                  "out_vqe_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.998,
                                                               6);
  auto result = hsf.run();
  EXPECT_EQ(result, ec::EquivalenceCriterion::Equivalent);
}

TEST(HSFTest, qftNativegates10) {
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/check/"
      "qft_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/check/"
      "out_qft_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.9981,
                                                               6);
  EXPECT_THROW(hsf.run(), std::overflow_error);
}

TEST(HSFTest, qpeExact10) {
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/check/"
      "qpeexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/check/"
      "out_qpeexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.9846,
                                                               6);
  EXPECT_THROW(hsf.run(), std::overflow_error);
}

TEST(HSFTest, qpeInexact10) {
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/check/"
      "qpeinexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/check/"
      "out_qpeinexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 0.9893,
                                                               6);
  EXPECT_THROW(hsf.run(), std::overflow_error);
}
