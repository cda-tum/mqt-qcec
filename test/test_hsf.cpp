#include "checker/dd/HybridSchrodingerFeynmanChecker.hpp"
#include "dd/DDpackageConfig.hpp"
#include "ir/QuantumComputation.hpp"

#include <gtest/gtest.h>
#include <string>

TEST(HSFTest, dj10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "dj_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{"./circuits/approximateEquivalenceTest/"
                                  "out_dj_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}

TEST(HSFTest, ghz10) {
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/"
      "ghz_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/"
      "out_ghz_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}

TEST(HSFTest, wstate10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "wstate_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{"./circuits/approximateEquivalenceTest/"
                                  "out_wstate_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}

TEST(HSFTest, graphstate10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "graphstate_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/"
      "out_graphstate_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}

TEST(HSFTest, vqe10) {
  const qc::QuantumComputation c1{"./circuits/approximateEquivalenceTest/"
                                  "vqe_indep_qiskit_10_no_measure.qasm"};
  const qc::QuantumComputation c2{"./circuits/approximateEquivalenceTest/"
                                  "out_vqe_indep_qiskit_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}

TEST(HSFTest, qftNativegates10) {
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/check/"
      "qft_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/check/"
      "out_qft_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}

TEST(HSFTest, qpeExact10) {
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/check/"
      "qpeexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/check/"
      "out_qpeexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}

TEST(HSFTest, qpeInexact10) {
  const qc::QuantumComputation c1{
      "./circuits/approximateEquivalenceTest/check/"
      "qpeinexact_nativegates_ibm_qiskit_opt3_10_no_measure.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/approximateEquivalenceTest/check/"
      "out_qpeinexact_nativegates_ibm_qiskit_opt3_10_high_error.qasm"};
  ec::HybridSchrodingerFeynmanChecker<dd::DDPackageConfig> hsf(c1, c2, 6);
  auto resultDD = hsf.check();
}
