from __future__ import annotations

import datetime

import pytest
from mqt import qcec
from mqt.qcec.compilation_flow_profiles import AncillaMode
from qiskit import QuantumCircuit, transpile
from qiskit.circuit import Parameter
from qiskit.providers.fake_provider import FakeAthens

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


@pytest.fixture
def original_circuit() -> QuantumCircuit:
    qc = QuantumCircuit(3)
    qc.h(0)
    qc.cx(0, 1)
    qc.cx(0, 2)
    qc.rx(alpha, 0)
    qc.measure_all()
    return qc


def test_equivalent_rz_commute(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_correct: QuantumCircuit) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_correct)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent


def test_non_equivalent_rz_commute(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_incorrect, timeout=datetime.timedelta(seconds=3600))
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent


def test_non_equivalent_phase_rz_commute(
    rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit
) -> None:
    result = qcec.verify(
        rz_commute_lhs, rz_commute_rhs_incorrect, additional_instantiations=2, timeout=datetime.timedelta(seconds=3600)
    )
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


@pytest.mark.parametrize("optimization_level", [0, 1, 2, 3])
def test_verify_compilation_on_optimization_levels(original_circuit: QuantumCircuit, optimization_level: int) -> None:
    """
    Test the verification of the compilation of a circuit
    to the 5-qubit IBMQ Athens architecture with various optimization levels.
    """
    compiled_circuit = transpile(original_circuit, backend=FakeAthens(), optimization_level=optimization_level)
    result = qcec.verify_compilation(
        original_circuit, compiled_circuit, optimization_level, timeout=datetime.timedelta(seconds=3600)
    )
    assert (
        result.equivalence == qcec.EquivalenceCriterion.equivalent
        or result.equivalence == qcec.EquivalenceCriterion.equivalent_up_to_global_phase
    )


@pytest.mark.parametrize("optimization_level", [0, 1, 2, 3])
def test_verify_compilation_on_optimization_levels_config(
    original_circuit: QuantumCircuit, optimization_level: int
) -> None:
    """
    Test the verification of the compilation of a circuit
    to the 5-qubit IBMQ Athens architecture with various optimization levels.
    """
    config = qcec.Configuration()
    config.execution.run_zx_checker = False
    compiled_circuit = transpile(original_circuit, backend=FakeAthens(), optimization_level=optimization_level)
    result = qcec.verify_compilation(
        original_circuit, compiled_circuit, optimization_level, AncillaMode.NO_ANCILLA, config
    )
    assert (
        result.equivalence == qcec.EquivalenceCriterion.equivalent
        or result.equivalence == qcec.EquivalenceCriterion.equivalent_up_to_global_phase
    )


def test_performed_insantiations(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit) -> None:
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_incorrect, additional_instantiations=10)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent
    assert 1 < result.performed_instantiations < 10


def test_with_config(rz_commute_lhs: QuantumCircuit, rz_commute_rhs_incorrect: QuantumCircuit) -> None:
    config = qcec.Configuration()
    config.parameterized.additional_instantiations = 10
    result = qcec.verify(rz_commute_lhs, rz_commute_rhs_incorrect, config)
    assert result.equivalence == qcec.EquivalenceCriterion.not_equivalent
    assert 1 < result.performed_instantiations < 10
