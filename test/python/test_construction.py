"""Test the construction of the EquivalenceCheckingManager class."""

from __future__ import annotations

import pytest

from mqt import qcec
from mqt.core import QuantumComputation


@pytest.fixture()
def example_circuit() -> QuantumComputation:
    """Fixture for a simple circuit."""
    qc = QuantumComputation(1)
    qc.h(0)
    return qc


def test_default_constructor_with_quantum_computation(example_circuit: QuantumComputation) -> None:
    """Test constructing an instance from two qiskit circuits with all default arguments."""
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit)


def test_constructor_with_configuration(example_circuit: QuantumComputation) -> None:
    """Test constructing an instance from circuits and a configuration object."""
    config = qcec.Configuration()
    qcec.EquivalenceCheckingManager(circ1=example_circuit, circ2=example_circuit, config=config)
