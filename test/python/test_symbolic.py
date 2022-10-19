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


@pytest.fixture
def rotation_gate_fuse_rhs_approximate() -> QuantumCircuit:
    qc = QuantumCircuit(1)
    qc.rz(alpha + beta + 1e-7, 0)
    return qc


@pytest.fixture
def cnot_rx() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.cx(0, 1)
    qc.rx(alpha, 0)
    return qc


@pytest.fixture
def cnot_rx_flipped() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.cx(1, 0)
    qc.rx(alpha, 0)
    return qc


@pytest.fixture
def cnot_rx_flipped_approx() -> QuantumCircuit:
    qc = QuantumCircuit(2)
    qc.cx(1, 0)
    qc.rx(alpha + 1e-12, 0)
    return qc


def test_equivalent_rz_commute(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_correct: QuantumCircuit) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_correct)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent


def test_non_equivalent_rz_commute(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_incorrect)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_non_equivalent_phase_rz_commute(
    rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit
) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_incorrect, additional_instantiations=2)
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


def test_almost_zero_non_equ(
    rotation_gate_fuse_lhs: QuantumCircuit, rotation_gate_fuse_rhs_approximate: QuantumCircuit
) -> None:
    result = qcec.verify(rotation_gate_fuse_lhs, rotation_gate_fuse_rhs_approximate)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_almost_zero_non_equ_random(
    rotation_gate_fuse_lhs: QuantumCircuit, rotation_gate_fuse_rhs_approximate: QuantumCircuit
) -> None:
    result = qcec.verify(rotation_gate_fuse_lhs, rotation_gate_fuse_rhs_approximate, parameterized_tolerance=1e-9)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_cnot_rx_non_equ(cnot_rx: QuantumCircuit, cnot_rx_flipped: QuantumCircuit) -> None:
    result = qcec.verify(cnot_rx, cnot_rx_flipped)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_cnot_rx_non_equ_approx(cnot_rx: QuantumCircuit, cnot_rx_flipped_approx: QuantumCircuit) -> None:
    result = qcec.verify(cnot_rx, cnot_rx_flipped_approx)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent
