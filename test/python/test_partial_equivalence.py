"""Tests the partial equivalence checking support of QCEC."""

from __future__ import annotations

import pytest
from qiskit import QuantumCircuit

from mqt import qcec


@pytest.fixture()
def original_circuit() -> QuantumCircuit:
    """Fixture for a simple circuit."""
    qc = QuantumCircuit(3, 1)
    qc.cswap(1, 0, 2)
    qc.h(0)
    qc.z(2)
    qc.cswap(1, 0, 2)
    qc.measure(0, 0)
    return qc


@pytest.fixture()
def alternative_circuit() -> QuantumCircuit:
    """Fixture for an alternative version of the simple circuit."""
    qc = QuantumCircuit(3, 1)
    qc.x(1)
    qc.cx(0, 1)
    qc.ch(1, 0)
    qc.measure(0, 0)
    return qc


@pytest.mark.skip()
def test_configuration_pec(original_circuit: QuantumCircuit, alternative_circuit: QuantumCircuit) -> None:
    """Test if the flag for partial equivalence checking works. This test is skipped for now, because partial equivalence checking was not implemented yet."""
    result = qcec.verify(original_circuit, alternative_circuit, check_partial_equivalence=True)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent
