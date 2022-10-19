from __future__ import annotations

import pytest
from mqt import qcec
from qiskit import QuantumCircuit
from qiskit.circuit import Parameter

alpha = Parameter("alpha")
beta = Parameter("beta")


@pytest.fixture
def rz_commute_lhs() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.rz(alpha, 0)
    qc.cx(1, 0)
    qc.rz(beta, 0)
    qc.cx(1, 0)
    return qc


@pytest.fixture
def rz_commute_rhs_correct() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.cx(1, 0)
    qc.rz(beta, 0)
    qc.cx(1, 0)
    qc.rz(alpha, 0)
    return qc


@pytest.fixture
def rz_commute_rhs_incorrect() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.cx(1, 0)
    qc.rz(alpha, 0)
    qc.cx(1, 0)
    qc.rz(beta, 0)
    return qc


@pytest.fixture
def rotation_gate_fuse_lhs() -> QuantumCircuit:
    qc = QuantumCircuit(1)
    qc.rz(alpha, 0)
    qc.rz(beta, 0)
    return qc


@pytest.fixture
def rotation_gate_fuse_rhs_correct() -> QuantumCircuit:
    qc = QuantumCircuit(1)
    qc.rz(alpha + beta, 0)
    return qc


@pytest.fixture
def rotation_gate_fuse_rhs_incorrect() -> QuantumCircuit:
    qc = QuantumCircuit(1)
    qc.rz(alpha - beta, 0)
    return qc


def test_equivalent_rz_commute(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_correct: QuantumCircuit) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_correct)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent


def test_non_equivalent_rz_commute(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_incorrect)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_non_equivalent_random_rz_commute(
    rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit
) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_incorrect, additional_instantiations=0)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_equivalent_roation_gate_fuse(
    rotation_gate_fuse_lhs: QuantumCircuit, rotation_gate_fuse_rhs_correct: QuantumCircuit
) -> None:
    result = qcec.verify(rotation_gate_fuse_lhs, rotation_gate_fuse_rhs_correct)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent


def test_non_equivalent_roation_gate_fuse(
    rotation_gate_fuse_lhs: QuantumCircuit, rotation_gate_fuse_rhs_incorrect: QuantumCircuit
) -> None:
    result = qcec.verify(rotation_gate_fuse_lhs, rotation_gate_fuse_rhs_incorrect)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent
