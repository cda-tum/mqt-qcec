from __future__ import annotations

import pytest
from mqt import qcec
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter

alpha = Parameter('alpha')
beta = Parameter('beta')


@pytest.fixture
def original_circuit() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.rz(alpha, 0)
    qc.cx(1, 0)
    qc.rz(beta, 0)
    qc.cx(1, 0)
    return qc


@pytest.fixture
def alternative_circuit_correct() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.cx(1, 0)
    qc.rz(beta, 0)
    qc.cx(1, 0)
    qc.rz(alpha, 0)
    return qc


@pytest.fixture
def alternative_circuit_incorrect() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.cx(1, 0)
    qc.rz(alpha, 0)
    qc.cx(1, 0)
    qc.rz(beta, 0)
    return qc


def test_equivalent_simple(original_circuit: QuantumCircuit, alternative_circuit_correct: QuantumCircuit) -> None:
    result = qcec.verify(original_circuit, alternative_circuit_correct)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent


def test_non_equivalent_simple(original_circuit: QuantumCircuit, alternative_circuit_incorrect: QuantumCircuit) -> None:
    result = qcec.verify(original_circuit, alternative_circuit_incorrect)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_non_equivalent_random_simple(original_circuit: QuantumCircuit, alternative_circuit_incorrect: QuantumCircuit) -> None:
    result = qcec.verify(original_circuit, alternative_circuit_incorrect, n_symbolic_checks=0)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent
