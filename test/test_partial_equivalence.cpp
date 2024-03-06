//
// This file is part of the MQT QCEC library released under the MIT license.
// See README.md or go to https://github.com/cda-tum/qcec for more information.
//

#include "EquivalenceCheckingManager.hpp"
#include "checker/dd/DDPartialEquivalence.hpp"
#include "dd/Package.hpp"

#include "gtest/gtest.h"
#include <iostream>
#include <vector>

class PartialEquivalenceTest : public testing::Test {
  void SetUp() override {
    qc1 = qc::QuantumComputation(nqubits, nqubits);
    qc2 = qc::QuantumComputation(nqubits, nqubits);
    config.optimizations.fuseSingleQubitGates         = false;
    config.optimizations.reorderOperations            = false;
    config.optimizations.reconstructSWAPs             = false;
    config.optimizations.fixOutputPermutationMismatch = true;

    config.execution.runSimulationChecker   = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runConstructionChecker = false;
    config.execution.runZXChecker           = false;

    config.functionality.checkPartialEquivalence = true;
  }

protected:
  std::size_t            nqubits = 3U;
  qc::QuantumComputation qc1;
  qc::QuantumComputation qc2;
  ec::Configuration      config{};
};

TEST_F(PartialEquivalenceTest, AlternatingCheckerGarbage) {
  // these circuits have the same gates acting on the measured qubit
  // and random gates acting on the two garbage qubits
  qc1.cswap(1, 0, 2);
  qc1.cx(2, 0);
  qc1.h(0);
  qc1.tdg(1);
  qc1.s(1);
  qc1.z(2);

  qc2.cswap(1, 0, 2);
  qc2.cx(2, 0);
  qc2.h(0);
  qc2.h(2);
  qc2.rz(dd::PI_4, 1);
  qc2.ry(0.1, 1);
  qc2.cx(1, 2);

  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(2);
  qc2.setLogicalQubitGarbage(1);

  config.execution.runAlternatingChecker       = true;
  config.functionality.checkPartialEquivalence = false;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEquivalenceTest, AlternatingCheckerGarbage2) {
  // measured qubit: 1
  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(0);

  qc1.h(1);
  qc1.tdg(0);
  qc1.s(0);
  qc1.z(2);

  // measured qubit: 1
  qc2.setLogicalQubitGarbage(0);
  qc2.setLogicalQubitGarbage(2);

  qc2.h(1);
  qc2.h(0);
  qc2.rz(dd::PI_4, 2);
  qc2.ry(0.1, 2);
  qc2.cx(2, 0);

  config.execution.runAlternatingChecker       = true;
  config.functionality.checkPartialEquivalence = false;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEquivalenceTest, AlternatingCheckerGarbageAndAncillary) {
  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc1.setLogicalQubitAncillary(2);

  qc1.h(0);
  qc1.tdg(1);
  qc1.s(1);
  // ancillary qubits are initialized to zero, therefore this gate doesn't
  // change the functionality of the circuit
  qc1.cx(2, 0);

  qc::QuantumComputation qc3(nqubits - 1);
  qc3.setLogicalQubitGarbage(1);

  qc3.h(0);
  qc3.rz(dd::PI_4, 1);
  qc3.ry(0.1, 1);

  config.execution.runAlternatingChecker       = true;
  config.functionality.checkPartialEquivalence = false;
  ec::EquivalenceCheckingManager ecm(qc1, qc3, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEquivalenceTest, AlternatingCheckerGarbageNotEquivalent) {
  // example from the paper https://arxiv.org/abs/2208.07564
  qc1.cswap(1, 0, 2);
  qc1.h(0);
  qc1.z(2);
  qc1.cswap(1, 0, 2);

  qc2.x(1);
  qc2.ch(1, 0);

  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(2);
  qc2.setLogicalQubitGarbage(1);

  // partially equivalent circuits
  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);

  // these two circuits are only partially equivalent,
  // therefore the equivalence checker returns NotEquivalent when we disable
  // partial equality checking
  config.functionality.checkPartialEquivalence = false;
  ec::EquivalenceCheckingManager ecm2(qc1, qc2, config);
  ecm2.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm2.run();
  EXPECT_EQ(ecm2.equivalence(), ec::EquivalenceCriterion::NotEquivalent);
}

using namespace qc::literals;

TEST_F(PartialEquivalenceTest, TrivialEquivalence) {
  auto qc = qc::QuantumComputation(2);

  qc.h(1);
  qc.h(0);
  qc.cx(0, 1);
  qc.swap(1, 0);

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc, qc, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);

  qc.setLogicalQubitAncillary(1);

  ec::EquivalenceCheckingManager ecm2(qc, qc, config);
  ecm2.run();
  EXPECT_EQ(ecm2.equivalence(), ec::EquivalenceCriterion::Equivalent);

  qc.setLogicalQubitGarbage(1);

  ec::EquivalenceCheckingManager ecm3(qc, qc, config);
  ecm3.run();
  EXPECT_EQ(ecm3.equivalence(), ec::EquivalenceCriterion::Equivalent);

  qc.setLogicalQubitGarbage(0);

  ec::EquivalenceCheckingManager ecm4(qc, qc, config);
  ecm4.run();
  EXPECT_EQ(ecm4.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, BasicPartialEquivalenceChecking) {
  // only the second qubit has differing gates in the two circuits,
  // therefore they should be equivalent if we only measure the first qubit
  qc1.h(1);
  qc1.x(1);

  qc1.measure(0, 0);
  qc2.measure(0, 0);

  qc1.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(1);
  qc1.setLogicalQubitGarbage(2);
  qc2.setLogicalQubitGarbage(2);

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, ConstructionCheckerNotEquivalent) {
  // the first qubit has differing gates in the two circuits,
  // therefore they should not be equivalent if we only measure the first qubit
  qc1.h(0);
  qc1.x(0);

  qc1.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(1);

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::NotEquivalent);
}

TEST_F(PartialEquivalenceTest, ConstructionCheckerGarbageNotEquivalent) {
  // example from the paper https://arxiv.org/abs/2208.07564
  qc1.cswap(1, 0, 2);
  qc1.h(0);
  qc1.z(2);
  qc1.cswap(1, 0, 2);

  qc2.x(1);
  qc2.ch(1, 0);

  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc2.setLogicalQubitGarbage(2);
  qc2.setLogicalQubitGarbage(1);

  // partially equivalent circuits
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);

  // these two circuits are only partially equivalent,
  // therefore the equivalence checker returns NotEquivalent when we disable
  // partial equality checking
  config.functionality.checkPartialEquivalence = false;
  ec::EquivalenceCheckingManager ecm2(qc1, qc2, config);
  ecm2.run();
  EXPECT_EQ(ecm2.equivalence(), ec::EquivalenceCriterion::NotEquivalent);
}

TEST_F(PartialEquivalenceTest, ConstructionCheckerDifferentNumberOfQubits) {
  qc1.cswap(1, 0, 2);
  qc1.h(0);
  qc1.z(2);
  qc1.cswap(1, 0, 2);

  auto qc3 = qc::QuantumComputation(2);

  qc3.x(1);
  qc3.ch(1, 0);

  qc1.setLogicalQubitGarbage(2);
  qc1.setLogicalQubitGarbage(1);
  qc3.setLogicalQubitGarbage(1);

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc3, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, MQTBenchGrover3Qubits) {
  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/"
      "grover-noancilla_nativegates_ibm_qiskit_opt0_3.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/grover-noancilla_indep_qiskit_3.qasm"};

  // 3 measured qubits and 3 data qubits, full equivalence
  // construction checker
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);

  // alternating checker
  config.execution.runConstructionChecker = false;
  config.execution.runAlternatingChecker  = true;
  ec::EquivalenceCheckingManager ecm2(c1, c2, config);
  ecm2.run();
  EXPECT_EQ(ecm2.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEquivalenceTest, MQTBenchGrover7Qubits) {
  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/"
      "grover-noancilla_nativegates_ibm_qiskit_opt0_7.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/"
      "grover-noancilla_nativegates_ibm_qiskit_opt1_7.qasm"};

  // 7 measured qubits and 7 data qubits, full equivalence
  // construction checker
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);

  // alternating checker
  config.execution.runConstructionChecker = false;
  config.execution.runAlternatingChecker  = true;
  ec::EquivalenceCheckingManager ecm2(c1, c2, config);
  ecm2.run();
  EXPECT_EQ(ecm2.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, SliQECGrover22Qubits) {
  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/Grover_1.qasm"}; // 11 qubits, 11 data
                                                          // qubits
  qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/Grover_2.qasm"}; // 12 qubits, 11 data
                                                          // qubits

  // 11 measured qubits and 11 data qubits
  c2.setLogicalQubitAncillary(11);
  c2.setLogicalQubitGarbage(11);

  // construction checker
  // adds 10 ancillary qubits -> total number of qubits is 22
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, SliQECAdd19Qubits) {
  // full equivalence, 19 qubits
  // but this test uses algorithm for partial equivalence, not the "zero
  // ancillae" version
  qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/add6_196_1.qasm"};
  qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/add6_196_2.qasm"};

  // just for benchmarking reasons, we only measure 8 qubits
  c1.setLogicalQubitsAncillary(8, 18);
  c2.setLogicalQubitsAncillary(8, 18);
  c1.setLogicalQubitsGarbage(8, 18);
  c2.setLogicalQubitsGarbage(8, 18);

  // doesn't add ancillary qubits -> total number of qubits is 19
  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, MoveMeasuredQubitsToFrontConstructionChecker) {
  qc1.cswap(1, 2, 0);
  qc1.h(2);
  qc1.z(0);
  qc1.cswap(1, 2, 0);
  qc1.measure(2, 0);

  qc2.x(1);
  qc2.ch(1, 2);
  qc2.h(0);
  qc2.measure(2, 0);

  qc1.setLogicalQubitsGarbage(0, 1);
  qc2.setLogicalQubitsGarbage(0, 1);

  config.execution.runConstructionChecker        = true;
  config.optimizations.moveMeasuredQubitsToFront = true;

  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, MoveMeasuredQubitsToFrontAlternatingChecker) {
  qc1.cswap(1, 2, 0);
  qc1.h(2);
  qc1.z(0);
  qc1.cswap(1, 2, 0);
  qc1.measure(2, 0);

  qc2.x(1);
  qc2.ch(1, 2);
  qc2.h(0);
  qc2.measure(2, 0);

  qc1.setLogicalQubitsGarbage(0, 1);
  qc2.setLogicalQubitsGarbage(0, 1);

  config.execution.runAlternatingChecker         = true;
  config.optimizations.moveMeasuredQubitsToFront = true;

  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, ExamplePaperDifferentQubitOrderAndNumber) {
  qc::QuantumComputation c1{4, 1};
  c1.cswap(1, 2, 0);
  c1.h(2);
  c1.z(0);
  c1.cswap(1, 2, 0);

  qc::QuantumComputation c2{3, 1};
  c2.x(1);
  c2.ch(1, 2);

  c1.setLogicalQubitsGarbage(0, 1);
  c1.setLogicalQubitGarbage(3);
  c1.setLogicalQubitAncillary(3);

  c2.setLogicalQubitsGarbage(0, 1);

  config.execution.runConstructionChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, AlternatingCheckerSliQEC19Qubits) {
  config.execution.runAlternatingChecker = true;

  // full equivalence, 10 qubits
  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/entanglement_1.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/entanglement_2.qasm"};

  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);

  // full equivalence, 19 qubits
  const qc::QuantumComputation c3{
      "./circuits/partialEquivalenceTest/add6_196_1.qasm"};
  const qc::QuantumComputation c4{
      "./circuits/partialEquivalenceTest/add6_196_2.qasm"};

  ec::EquivalenceCheckingManager ecm2(c1, c2, config);
  ecm2.run();
  EXPECT_EQ(ecm2.equivalence(), ec::EquivalenceCriterion::Equivalent);

  // full equivalence, 10 qubits
  const qc::QuantumComputation c5{
      "./circuits/partialEquivalenceTest/bv_1.qasm"};
  const qc::QuantumComputation c6{
      "./circuits/partialEquivalenceTest/bv_2.qasm"};

  // calls zeroAncillaePartialEquivalenceCheck
  ec::EquivalenceCheckingManager ecm3(c1, c2, config);
  ecm3.run();
  EXPECT_EQ(ecm3.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, AlternatingCheckerSliQECRandomCircuit) {
  config.execution.runAlternatingChecker = true;
  // full equivalence, 10 qubits
  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/random_1.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/random_2.qasm"};

  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

TEST_F(PartialEquivalenceTest, ConstructionCheckerSliQECPeriodFinding8Qubits) {
  config.execution.runConstructionChecker = true;
  // 8 qubits, 3 data qubits
  qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/period_finding_1.qasm"};
  // 8 qubits, 3 data qubits
  qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/period_finding_2.qasm"};

  // 3 measured qubits and 3 data qubits

  c2.setLogicalQubitsAncillary(3, 7);
  c2.setLogicalQubitsGarbage(3, 7);

  c1.setLogicalQubitsAncillary(3, 7);
  c1.setLogicalQubitsGarbage(3, 7);

  ec::EquivalenceCheckingManager ecm(c1, c2, config);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::Equivalent);
}

void partialEquivalencCheckingBenchmarks(const qc::Qubit          minN,
                                         const qc::Qubit          maxN,
                                         const size_t             reps,
                                         const bool               addAncilla,
                                         const ec::Configuration& config) {
  std::mt19937 gen(55);

  for (qc::Qubit n = minN; n < maxN; n++) {
    double      totalTime{0};
    std::size_t totalGates{0};
    for (size_t k = 0; k < reps; k++) {
      qc::Qubit d{0};
      if (addAncilla) {
        std::uniform_int_distribution<qc::Qubit> nrDataQubits(1, n);
        d = nrDataQubits(gen);
      } else {
        d = n;
      }
      std::uniform_int_distribution<qc::Qubit> nrDataQubits(1, d);
      const qc::Qubit                          m = nrDataQubits(gen);

      const auto [c1, c2] = dd::generateRandomBenchmark(n, d, m);

      // check equivalence
      ec::EquivalenceCheckingManager ecm(c1, c2, config);
      ecm.run();
      EXPECT_TRUE(ecm.getResults().consideredEquivalent());

      const auto duration = ecm.getResults().checkTime;

      // std::cout << "\nnumber of qubits = " << n << "; data qubits = " << d
      //           << "; measured qubits = " << m
      //           << "; number of gates = " << c2.size() << "\n";
      // std::cout << "time: " << static_cast<double>(duration.count()) /
      // 1000000.
      //           << " seconds\n";
      totalTime += duration;
      totalGates += c2.size();
    }
    std::cout << "\nnumber of qubits = " << n << "; number of reps = " << reps
              << "; average time = "
              << (totalTime / static_cast<double>(reps) / 1000000.)
              << " seconds; average number of gates = "
              << (static_cast<double>(totalGates) / static_cast<double>(reps))
              << "\n";
  }
}

TEST_F(PartialEquivalenceTest, Benchmark) {
  config.execution.runConstructionChecker = true;
  const size_t minN                       = 2;
  const size_t maxN                       = 8;
  const size_t reps                       = 10;
  std::cout << "Partial equivalence check\n";
  partialEquivalencCheckingBenchmarks(minN, maxN, reps, true, config);
}

TEST_F(PartialEquivalenceTest, ZeroAncillaBenchmark) {
  config.execution.runAlternatingChecker = true;
  const size_t minN                      = 3;
  const size_t maxN                      = 15;
  const size_t reps                      = 10;
  std::cout << "Zero-ancilla partial equivalence check\n";
  partialEquivalencCheckingBenchmarks(minN, maxN, reps, false, config);
}
