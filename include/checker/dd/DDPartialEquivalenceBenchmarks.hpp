#pragma once

#include "dd/FunctionalityConstruction.hpp"
#include "dd/Package.hpp"

namespace dd {

/**
  Generate random benchmarks for partial equivalence checking.
  @param n number of qubits of the resulting circuits
  @param d number of data qubits in the resulting circuits
  @param m number of measured qubits in the resulting circuit
  @return two circuits that are partially equivalent
**/
std::pair<qc::QuantumComputation, qc::QuantumComputation>
generateRandomBenchmark(size_t n, qc::Qubit d, qc::Qubit m);
} // namespace dd
