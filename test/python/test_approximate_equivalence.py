"""Tests the partial equivalence checking support of QCEC."""

from __future__ import annotations

import pytest
from qiskit import QuantumCircuit

from mqt import qcec


@pytest.fixture
def original_circuit() -> QuantumCircuit:
    """Fixture for a simple circuit."""
    qc = QuantumCircuit(3)
    qc.mcx([0, 1], 2)
    return qc


@pytest.fixture
def alternative_circuit() -> QuantumCircuit:
    """Fixture for an approximately equivalent version of the simple circuit."""
    qc = QuantumCircuit(3)
    qc.id(0)
    qc.id(1)
    qc.id(2)
    return qc


def test_configuration_pec(original_circuit: QuantumCircuit, alternative_circuit: QuantumCircuit) -> None:
    """Test if the flag for approximate equivalence checking works."""
    config = qcec.Configuration()
    config.execution.run_alternating_checker = True
    config.execution.run_construction_checker = False
    config.execution.run_simulation_checker = False
    config.execution.run_zx_checker = False
    config.functionality.check_approximate_equivalence = True
    config.functionality.approximate_checking_threshold = 0.3
    result = qcec.verify(original_circuit, alternative_circuit, configuration=config)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent


def test_argument_pec(original_circuit: QuantumCircuit, alternative_circuit: QuantumCircuit) -> None:
    """Test if the flag for approximate equivalence checking works."""
    config = qcec.Configuration()
    config.execution.run_alternating_checker = True
    config.execution.run_construction_checker = False
    config.execution.run_simulation_checker = False
    config.execution.run_zx_checker = False
    result = qcec.verify(
        original_circuit,
        alternative_circuit,
        configuration=config,
        check_approximate_equivalence=True,
        approximate_checking_threshold=0.3,
    )
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent
