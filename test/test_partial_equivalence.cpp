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

class PartialEqualityTest : public testing::Test {
  void SetUp() override {
    qc1                                       = qc::QuantumComputation(nqubits);
    qc2                                       = qc::QuantumComputation(nqubits);
    config.optimizations.fuseSingleQubitGates = false;
    config.optimizations.reorderOperations    = false;
    config.optimizations.reconstructSWAPs     = false;
    config.optimizations.fixOutputPermutationMismatch = true;

    config.execution.runSimulationChecker   = false;
    config.execution.runAlternatingChecker  = false;
    config.execution.runConstructionChecker = false;
    config.execution.runZXChecker           = false;
  }

protected:
  std::size_t            nqubits = 3U;
  qc::QuantumComputation qc1;
  qc::QuantumComputation qc2;
  ec::Configuration      config{};
};

TEST_F(PartialEqualityTest, AlternatingCheckerGarbage) {
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

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEqualityTest, AlternatingCheckerGarbage2) {
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

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEqualityTest, AlternatingCheckerGarbageAndAncillary) {
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

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc3, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(),
            ec::EquivalenceCriterion::EquivalentUpToGlobalPhase);
}

TEST_F(PartialEqualityTest, AlternatingCheckerGarbageNotEquivalent) {
  // example from the paper https://arxiv.org/abs/2208.07564
  // these two circuits are only partially equivalent,
  // therefore the equivalence checker returns NotEquivalent
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

  config.execution.runAlternatingChecker = true;
  ec::EquivalenceCheckingManager ecm(qc1, qc2, config);
  ecm.setApplicationScheme(ec::ApplicationSchemeType::Proportional);
  ecm.run();
  EXPECT_EQ(ecm.equivalence(), ec::EquivalenceCriterion::NotEquivalent);
}

using namespace qc::literals;

TEST(PartialEquivalenceTest, TrivialEquivalence) {
  const auto nqubits = 2U;
  const auto dd      = std::make_unique<dd::Package<>>(nqubits);
  const auto inputMatrix =
      dd::CMat{{1, 1, 1, 1}, {1, -1, 1, -1}, {1, 1, -1, -1}, {1, -1, -1, 1}};
  const auto inputDD = dd->makeDDFromMatrix(inputMatrix);

  EXPECT_TRUE(partialEquivalenceCheckDD(inputDD, inputDD, 1, 1, *dd));
  EXPECT_TRUE(partialEquivalenceCheckDD(inputDD, inputDD, 2, 1, *dd));
  EXPECT_TRUE(partialEquivalenceCheckDD(inputDD, inputDD, 1, 2, *dd));
  EXPECT_TRUE(partialEquivalenceCheckDD(inputDD, inputDD, 2, 2, *dd));

  const auto hGate      = dd->makeGateDD(dd::H_MAT, 2, 1);
  const auto cxGate     = dd->makeGateDD(dd::X_MAT, 2, 1_pc, 0);
  const auto bellMatrix = dd->multiply(cxGate, hGate);
  EXPECT_FALSE(partialEquivalenceCheckDD(inputDD, bellMatrix, 1, 1, *dd));
}

TEST(PartialEquivalenceTest, BasicPartialEquivalenceChecking) {
  const auto nqubits = 3U;
  auto       dd      = std::make_unique<dd::Package<>>(nqubits);
  // only the second qubit has differing gates in the two circuits,
  // therefore they should be equivalent if we only measure the first qubit
  const auto hGate    = dd->makeGateDD(dd::H_MAT, 3, 1);
  const auto xGate    = dd->makeGateDD(dd::X_MAT, 3, 1);
  const auto circuit1 = dd->multiply(xGate, hGate);
  const auto circuit2 = dd->makeIdent(3);

  EXPECT_TRUE(partialEquivalenceCheckDD(circuit1, circuit2, 2, 1, *dd));
}

TEST(PartialEquivalenceTest, NotEquivalent) {
  const auto nqubits = 2U;
  const auto dd      = std::make_unique<dd::Package<>>(nqubits);
  // the first qubit has differing gates in the two circuits,
  // therefore they should not be equivalent if we only measure the first qubit
  const auto hGate    = dd->makeGateDD(dd::H_MAT, nqubits, 0);
  const auto xGate    = dd->makeGateDD(dd::X_MAT, nqubits, 0);
  const auto circuit1 = dd->multiply(xGate, hGate);
  const auto circuit2 = dd->makeIdent(2);
  EXPECT_FALSE(partialEquivalenceCheckDD(circuit1, circuit2, 2, 1, *dd));
}

TEST(PartialEquivalenceTest, ExamplePaper) {
  const auto nqubits = 3U;
  const auto dd      = std::make_unique<dd::Package<>>(nqubits);
  const auto controlledSwapGate =
      dd->makeTwoQubitGateDD(dd::SWAP_MAT, nqubits, qc::Controls{1}, 0, 2);
  const auto hGate = dd->makeGateDD(dd::H_MAT, nqubits, 0);
  const auto zGate = dd->makeGateDD(dd::Z_MAT, nqubits, 2);
  const auto xGate = dd->makeGateDD(dd::X_MAT, nqubits, 1);
  const auto controlledHGate =
      dd->makeGateDD(dd::H_MAT, nqubits, qc::Controls{1}, 0);

  const auto c1 = dd->multiply(
      controlledSwapGate,
      dd->multiply(hGate, dd->multiply(zGate, controlledSwapGate)));
  const auto c2 = dd->multiply(controlledHGate, xGate);

  EXPECT_TRUE(partialEquivalenceCheckDD(c1, c2, 3, 1, *dd));
}

TEST(PartialEquivalenceTest, DifferentNumberOfQubits) {
  const auto nqubits = 3U;
  const auto dd      = std::make_unique<dd::Package<>>(nqubits);
  const auto controlledSwapGate =
      dd->makeTwoQubitGateDD(dd::SWAP_MAT, nqubits, qc::Controls{1}, 0, 2);
  const auto hGate           = dd->makeGateDD(dd::H_MAT, nqubits, 0);
  const auto zGate           = dd->makeGateDD(dd::Z_MAT, nqubits, 2);
  const auto xGate           = dd->makeGateDD(dd::X_MAT, 2, 1);
  const auto controlledHGate = dd->makeGateDD(dd::H_MAT, 2, qc::Controls{1}, 0);

  const auto c1 = dd->multiply(
      controlledSwapGate,
      dd->multiply(hGate, dd->multiply(zGate, controlledSwapGate)));
  const auto c2 = dd->multiply(controlledHGate, xGate);

  EXPECT_TRUE(partialEquivalenceCheckDD(c1, c2, 3, 1, *dd));
  EXPECT_FALSE(partialEquivalenceCheckDD(c2, c1, 3, 3, *dd));
  EXPECT_FALSE(partialEquivalenceCheckDD(c2, dd::mEdge::zero(), 2, 1, *dd));
  EXPECT_FALSE(partialEquivalenceCheckDD(c2, dd::mEdge::one(), 2, 1, *dd));
  EXPECT_FALSE(partialEquivalenceCheckDD(dd::mEdge::one(), c1, 2, 1, *dd));
  EXPECT_TRUE(
      partialEquivalenceCheckDD(dd::mEdge::one(), dd::mEdge::one(), 0, 1, *dd));
  EXPECT_TRUE(
      partialEquivalenceCheckDD(dd::mEdge::one(), dd::mEdge::one(), 0, 0, *dd));
}

TEST(PartialEquivalenceTest, ComputeTableTest) {
  const auto nqubits = 3U;
  const auto dd      = std::make_unique<dd::Package<>>(nqubits);
  const auto controlledSwapGate =
      dd->makeTwoQubitGateDD(dd::SWAP_MAT, nqubits, qc::Controls{1}, 2, 0);
  const auto hGate = dd->makeGateDD(dd::H_MAT, nqubits, 0);
  const auto zGate = dd->makeGateDD(dd::Z_MAT, nqubits, 2);
  const auto xGate = dd->makeGateDD(dd::X_MAT, nqubits, 1);
  const auto controlledHGate =
      dd->makeGateDD(dd::H_MAT, nqubits, qc::Controls{1}, 0);

  const auto c1 = dd->multiply(
      controlledSwapGate,
      dd->multiply(hGate, dd->multiply(zGate, controlledSwapGate)));
  const auto c2 = dd->multiply(controlledHGate, xGate);

  EXPECT_TRUE(partialEquivalenceCheckDD(c1, c2, 3, 1, *dd));
  EXPECT_TRUE(partialEquivalenceCheckDD(c1, c2, 3, 1, *dd));
  EXPECT_TRUE(partialEquivalenceCheckDD(c1, c2, 3, 1, *dd));
  EXPECT_TRUE(partialEquivalenceCheckDD(c1, c2, 3, 1, *dd));
  EXPECT_TRUE(partialEquivalenceCheckDD(c1, c2, 3, 1, *dd));
}

TEST(PartialEquivalenceTest, MQTBenchGrover3Qubits) {
  const auto dd = std::make_unique<dd::Package<>>(3);

  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/"
      "grover-noancilla_nativegates_ibm_qiskit_opt0_3.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/grover-noancilla_indep_qiskit_3.qasm"};

  // 3 measured qubits and 3 data qubits, full equivalence
  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
}

TEST(PartialEquivalenceTest, MQTBenchGrover7Qubits) {
  const auto dd = std::make_unique<dd::Package<>>(7);

  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/"
      "grover-noancilla_nativegates_ibm_qiskit_opt0_7.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/"
      "grover-noancilla_nativegates_ibm_qiskit_opt1_7.qasm"};

  // 7 measured qubits and 7 data qubits, full equivalence
  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
}

TEST(PartialEquivalenceTest, SliQECGrover22Qubits) {
  const auto dd = std::make_unique<dd::Package<>>(12);

  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/Grover_1.qasm"}; // 11 qubits, 11 data
                                                          // qubits
  qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/Grover_2.qasm"}; // 12 qubits, 11 data
                                                          // qubits

  // 11 measured qubits and 11 data qubits
  c2.setLogicalQubitAncillary(11);
  c2.setLogicalQubitGarbage(11);

  // adds 10 ancillary qubits -> total number of qubits is 22
  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
}

TEST(PartialEquivalenceTest, SliQECAdd19Qubits) {
  const auto dd = std::make_unique<dd::Package<>>(19);

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
  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
}

TEST(PartialEquivalenceTest, ExamplePaperDifferentQubitOrder) {
  const auto nqubits = 3U;
  auto       dd      = std::make_unique<dd::Package<>>(nqubits);

  qc::QuantumComputation c1{nqubits, 1};
  c1.cswap(1, 2, 0);
  c1.h(2);
  c1.z(0);
  c1.cswap(1, 2, 0);

  qc::QuantumComputation c2{nqubits, 1};
  c2.x(1);
  c2.ch(1, 2);

  c1.setLogicalQubitsGarbage(0, 1);
  c2.setLogicalQubitsGarbage(0, 1);

  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
}

TEST(PartialEquivalenceTest, ExamplePaperDifferentQubitOrderAndNumber) {
  const auto nqubits = 4U;
  auto       dd      = std::make_unique<dd::Package<>>(nqubits);

  qc::QuantumComputation c1{nqubits, 1};
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
  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
  EXPECT_TRUE(partialEquivalenceCheck(c2, c1, *dd));
}

TEST(PartialEquivalenceTest, ZeroAncillaSliQEC19Qubits) {
  auto dd = std::make_unique<dd::Package<>>(19);

  // full equivalence, 10 qubits
  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/entanglement_1.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/entanglement_2.qasm"};

  // calls zeroAncillaePartialEquivalenceCheck
  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));

  // full equivalence, 19 qubits
  const qc::QuantumComputation c3{
      "./circuits/partialEquivalenceTest/add6_196_1.qasm"};
  const qc::QuantumComputation c4{
      "./circuits/partialEquivalenceTest/add6_196_2.qasm"};

  // calls zeroAncillaePartialEquivalenceCheck
  EXPECT_TRUE(partialEquivalenceCheck(c3, c4, *dd));

  // full equivalence, 10 qubits
  const qc::QuantumComputation c5{
      "./circuits/partialEquivalenceTest/bv_1.qasm"};
  const qc::QuantumComputation c6{
      "./circuits/partialEquivalenceTest/bv_2.qasm"};

  // calls zeroAncillaePartialEquivalenceCheck
  EXPECT_TRUE(partialEquivalenceCheck(c5, c6, *dd));
}

TEST(PartialEquivalenceTest, ZeroAncillaSliQECRandomCircuit) {
  auto dd = std::make_unique<dd::Package<>>(10);
  // full equivalence, 10 qubits
  const qc::QuantumComputation c1{
      "./circuits/partialEquivalenceTest/random_1.qasm"};
  const qc::QuantumComputation c2{
      "./circuits/partialEquivalenceTest/random_2.qasm"};

  // calls buildFunctionality for c2^(-1) concatenated with c1
  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
}

TEST(PartialEquivalenceTest, SliQECPeriodFinding8Qubits) {
  auto dd = std::make_unique<dd::Package<>>(8);
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

  EXPECT_TRUE(partialEquivalenceCheck(c1, c2, *dd));
}

void partialEquivalencCheckingBenchmarks(const qc::Qubit minN,
                                         const qc::Qubit maxN,
                                         const size_t    reps,
                                         const bool      addAncilla) {
  std::mt19937 gen(55);

  for (qc::Qubit n = minN; n < maxN; n++) {
    std::chrono::microseconds totalTime{0};
    std::size_t               totalGates{0};
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

      auto dd = std::make_unique<dd::Package<>>(n);

      const auto start = std::chrono::high_resolution_clock::now();

      const bool result = dd::partialEquivalenceCheck(c1, c2, *dd);
      // Get ending timepoint
      const auto stop = std::chrono::high_resolution_clock::now();
      const auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

      EXPECT_TRUE(result);

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
              << (static_cast<double>(totalTime.count()) /
                  static_cast<double>(reps) / 1000000.)
              << " seconds; average number of gates = "
              << (static_cast<double>(totalGates) / static_cast<double>(reps))
              << "\n";
  }
}

TEST(PartialEquivalenceTest, Benchmark) {
  const size_t minN = 2;
  const size_t maxN = 8;
  const size_t reps = 10;
  std::cout << "Partial equivalence check\n";
  partialEquivalencCheckingBenchmarks(minN, maxN, reps, true);
}

TEST(PartialEquivalenceTest, ZeroAncillaBenchmark) {
  const size_t minN = 3;
  const size_t maxN = 15;
  const size_t reps = 10;
  std::cout << "Zero-ancilla partial equivalence check\n";
  partialEquivalencCheckingBenchmarks(minN, maxN, reps, false);
}

TEST(PartialEquivalenceTest, InvalidInput) {
  const auto nqubits = 4U;
  const auto dd      = std::make_unique<dd::Package<>>(nqubits);

  // the circuits don't have the same number of measured qubits
  qc::QuantumComputation c1{nqubits, 1};
  c1.x(1);

  qc::QuantumComputation c2{nqubits, 1};
  c2.x(1);

  c1.setLogicalQubitsGarbage(0, 1);
  c1.setLogicalQubitGarbage(3);

  c2.setLogicalQubitsGarbage(0, 1);

  EXPECT_FALSE(partialEquivalenceCheck(c1, c2, *dd));

  // now they have the same number of measured qubits but a different
  // permutation of garbage qubits
  c2.setLogicalQubitGarbage(2);
  EXPECT_THROW(partialEquivalenceCheck(c1, c2, *dd), std::invalid_argument);
}
