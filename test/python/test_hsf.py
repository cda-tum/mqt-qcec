"""Tests the partial equivalence checking support of QCEC."""

from __future__ import annotations

import pytest
from qiskit import QuantumCircuit

from mqt import qcec


@pytest.fixture
def original_circuit() -> QuantumCircuit:
    """Fixture for a simple circuit."""
    qc = QuantumCircuit(4)
    qc.cx(0, 2)
    qc.cx(1, 3)
    return qc


@pytest.fixture
def alternative_circuit() -> QuantumCircuit:
    """Fixture of a second circuit that will be checked for equivalence."""
    qc = QuantumCircuit(4)
    qc.id(0)
    qc.id(1)
    qc.id(2)
    qc.id(3)
    return qc


def test_configuration_pec(original_circuit: QuantumCircuit, alternative_circuit: QuantumCircuit) -> None:
    """Test if the flag for hsf equivalence checking works."""
    config = qcec.Configuration()
    config.execution.run_alternating_checker = False
    config.execution.run_construction_checker = False
    config.execution.run_simulation_checker = False
    config.execution.run_zx_checker = False
    config.execution.run_hsf_checker = True
    config.functionality.check_approximate_equivalence = True
    config.functionality.approximate_checking_threshold = 0.8
    result = qcec.verify(original_circuit, alternative_circuit, configuration=config)
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent


def test_argument_pec(original_circuit: QuantumCircuit, alternative_circuit: QuantumCircuit) -> None:
    """Test if the flag for hsf equivalence checking works."""
    config = qcec.Configuration()
    config.execution.run_alternating_checker = False
    config.execution.run_construction_checker = False
    config.execution.run_simulation_checker = False
    config.execution.run_zx_checker = False
    config.execution.run_hsf_checker = True
    result = qcec.verify(
        original_circuit,
        alternative_circuit,
        configuration=config,
        check_approximate_equivalence=True,
        approximate_checking_threshold=0.8,
    )
    assert result.equivalence == qcec.EquivalenceCriterion.equivalent
