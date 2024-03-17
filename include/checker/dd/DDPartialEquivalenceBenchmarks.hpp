#pragma once

#include "dd/FunctionalityConstruction.hpp"
#include "dd/Package.hpp"

namespace dd {

/**
  Generate random benchmarks for partial equivalence checking. Returns pairs of
circuits which are partially equivalent, following the method described in the
paper "Partial Equivalence Checking of Quantum
Circuits" (https://arxiv.org/abs/2208.07564) in Section VI. B.
  @param n number of qubits of the resulting circuits
  @param d number of data qubits in the resulting circuits
  @param m number of measured qubits in the resulting circuit
  @return two circuits that are partially equivalent
**/
std::pair<qc::QuantumComputation, qc::QuantumComputation>
generatePartiallyEquivalentCircuits(size_t n, qc::Qubit d, qc::Qubit m);
} // namespace dd
