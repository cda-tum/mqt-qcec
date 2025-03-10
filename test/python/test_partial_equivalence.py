"""Tests the partial equivalence checking support of QCEC."""

from __future__ import annotations

import pytest
from qiskit import QuantumCircuit

from mqt.qcec import verify
from mqt.qcec.pyqcec import Configuration, EquivalenceCriterion


@pytest.fixture
def original_circuit() -> QuantumCircuit:
    """Fixture for a simple circuit."""
    qc = QuantumCircuit(3, 1)
    qc.cswap(1, 0, 2)
    qc.h(0)
    qc.z(2)
    qc.cswap(1, 0, 2)
    qc.measure(0, 0)
    return qc


@pytest.fixture
def alternative_circuit() -> QuantumCircuit:
    """Fixture for a partially equivalent version of the simple circuit."""
    qc = QuantumCircuit(3, 1)
    qc.x(1)
    qc.ch(1, 0)
    qc.measure(0, 0)
    return qc


def test_configuration_pec(original_circuit: QuantumCircuit, alternative_circuit: QuantumCircuit) -> None:
    """Test if the flag for partial equivalence checking works."""
    config = Configuration()
    config.functionality.check_partial_equivalence = True
    result = verify(original_circuit, alternative_circuit, configuration=config)
    assert result.equivalence == EquivalenceCriterion.equivalent


def test_argument_pec(original_circuit: QuantumCircuit, alternative_circuit: QuantumCircuit) -> None:
    """Test if the flag for partial equivalence checking works."""
    result = verify(original_circuit, alternative_circuit, check_partial_equivalence=True)
    assert result.equivalence == EquivalenceCriterion.equivalent
